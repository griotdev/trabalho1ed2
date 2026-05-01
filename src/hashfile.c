#include "hashfile.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    int numBuckets;
    int tamRegistro;
    int tamBloco;
    int offsetChave;
    int tamChave;
    long offsetTable;
    long offsetBuckets;
    long offsetOverflow;
} HFCabecalho;

struct HashFile_s {
    FILE       *fp;
    HFCabecalho cab;
    int         capacidade;
};

static unsigned int hash_bytes(const void *dados, int tam) {
    const unsigned char *p = (const unsigned char *)dados;
    unsigned int h = 2166136261u;
    for (int i = 0; i < tam; i++) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

static int indice_bucket(const HashFile hf, const void *chave) {
    unsigned int h = hash_bytes(chave, hf->cab.tamChave);
    return (int)(h % (unsigned int)hf->cab.numBuckets);
}

static long offset_bucket(const HashFile hf, int idx) {
    return hf->cab.offsetBuckets + (long)idx * (long)hf->cab.tamBloco;
}

static const void *extrair_chave(const HashFile hf, const void *registro) {
    return (const char *)registro + hf->cab.offsetChave;
}

static int ler_ocupados(const HashFile hf, int idx) {
    int ocupados = 0;
    fseek(hf->fp, offset_bucket(hf, idx), SEEK_SET);
    fread(&ocupados, sizeof(int), 1, hf->fp);
    return ocupados;
}

static void escrever_ocupados(const HashFile hf, int idx, int ocupados) {
    fseek(hf->fp, offset_bucket(hf, idx), SEEK_SET);
    fwrite(&ocupados, sizeof(int), 1, hf->fp);
    fflush(hf->fp);
}

static void ler_registro(const HashFile hf, int idx, int pos, void *saida) {
    long off = offset_bucket(hf, idx) + (long)sizeof(int)
               + (long)pos * (long)hf->cab.tamRegistro;
    fseek(hf->fp, off, SEEK_SET);
    fread(saida, (size_t)hf->cab.tamRegistro, 1, hf->fp);
}

static void escrever_registro(const HashFile hf, int idx, int pos,
                              const void *registro) {
    long off = offset_bucket(hf, idx) + (long)sizeof(int)
               + (long)pos * (long)hf->cab.tamRegistro;
    fseek(hf->fp, off, SEEK_SET);
    fwrite(registro, (size_t)hf->cab.tamRegistro, 1, hf->fp);
    fflush(hf->fp);
}

static int buscar_no_bucket(const HashFile hf, int idx, const void *chave) {
    int ocupados = ler_ocupados(hf, idx);
    char *buf = (char *)malloc((size_t)hf->cab.tamRegistro);
    if (buf == NULL) return -1;

    for (int i = 0; i < ocupados; i++) {
        ler_registro(hf, idx, i, buf);
        const void *k = extrair_chave(hf, buf);
        if (memcmp(k, chave, (size_t)hf->cab.tamChave) == 0) {
            free(buf);
            return i;
        }
    }
    free(buf);
    return -1;
}

HashFile hf_criar(const char *caminho, int num_buckets, int tam_registro,
                  int tam_bloco, int offset_chave, int tam_chave) {
    if (caminho == NULL || num_buckets <= 0 || tam_registro <= 0
        || tam_bloco <= 0 || tam_chave <= 0) {
        return NULL;
    }

    int capacidade = (tam_bloco - (int)sizeof(int)) / tam_registro;
    if (capacidade <= 0) return NULL;

    FILE *fp = fopen(caminho, "w+b");
    if (fp == NULL) return NULL;

    HashFile hf = (HashFile)malloc(sizeof(struct HashFile_s));
    if (hf == NULL) {
        fclose(fp);
        return NULL;
    }

    hf->fp = fp;
    hf->capacidade = capacidade;

    hf->cab.numBuckets    = num_buckets;
    hf->cab.tamRegistro   = tam_registro;
    hf->cab.tamBloco      = tam_bloco;
    hf->cab.offsetChave   = offset_chave;
    hf->cab.tamChave      = tam_chave;
    hf->cab.offsetTable   = (long)sizeof(HFCabecalho);
    hf->cab.offsetBuckets = hf->cab.offsetTable
                            + (long)num_buckets * (long)sizeof(long);
    hf->cab.offsetOverflow = -1;

    fseek(fp, 0, SEEK_SET);
    fwrite(&hf->cab, sizeof(HFCabecalho), 1, fp);

    fseek(fp, hf->cab.offsetTable, SEEK_SET);
    for (int i = 0; i < num_buckets; i++) {
        long off = offset_bucket(hf, i);
        fwrite(&off, sizeof(long), 1, fp);
    }

    char *bloco_vazio = (char *)calloc(1, (size_t)tam_bloco);
    if (bloco_vazio == NULL) {
        fclose(fp);
        free(hf);
        return NULL;
    }
    for (int i = 0; i < num_buckets; i++) {
        fseek(fp, offset_bucket(hf, i), SEEK_SET);
        fwrite(bloco_vazio, (size_t)tam_bloco, 1, fp);
    }
    free(bloco_vazio);

    fflush(fp);
    return hf;
}

HashFile hf_abrir(const char *caminho) {
    if (caminho == NULL) return NULL;

    FILE *fp = fopen(caminho, "r+b");
    if (fp == NULL) return NULL;

    HashFile hf = (HashFile)malloc(sizeof(struct HashFile_s));
    if (hf == NULL) {
        fclose(fp);
        return NULL;
    }

    hf->fp = fp;

    fseek(fp, 0, SEEK_SET);
    if (fread(&hf->cab, sizeof(HFCabecalho), 1, fp) != 1) {
        fclose(fp);
        free(hf);
        return NULL;
    }

    hf->capacidade = (hf->cab.tamBloco - (int)sizeof(int)) / hf->cab.tamRegistro;
    return hf;
}

void hf_fechar(HashFile hf) {
    if (hf == NULL) return;
    if (hf->fp != NULL) fclose(hf->fp);
    free(hf);
}

bool hf_inserir(HashFile hf, const void *registro) {
    if (hf == NULL || registro == NULL) return false;

    const void *chave = extrair_chave(hf, registro);
    int idx = indice_bucket(hf, chave);

    if (buscar_no_bucket(hf, idx, chave) >= 0) {
        return false;
    }

    int ocupados = ler_ocupados(hf, idx);
    if (ocupados >= hf->capacidade) {
        return false;
    }

    escrever_registro(hf, idx, ocupados, registro);
    escrever_ocupados(hf, idx, ocupados + 1);
    return true;
}

bool hf_buscar(HashFile hf, const void *chave, void *saida) {
    if (hf == NULL || chave == NULL) return false;

    int idx = indice_bucket(hf, chave);
    int pos = buscar_no_bucket(hf, idx, chave);
    if (pos < 0) return false;

    ler_registro(hf, idx, pos, saida);
    return true;
}

bool hf_remover(HashFile hf, const void *chave) {
    if (hf == NULL || chave == NULL) return false;

    int idx = indice_bucket(hf, chave);
    int pos = buscar_no_bucket(hf, idx, chave);
    if (pos < 0) return false;

    int ocupados = ler_ocupados(hf, idx);

    if (pos < ocupados - 1) {
        char *ultimo = (char *)malloc((size_t)hf->cab.tamRegistro);
        if (ultimo == NULL) return false;
        ler_registro(hf, idx, ocupados - 1, ultimo);
        escrever_registro(hf, idx, pos, ultimo);
        free(ultimo);
    }

    escrever_ocupados(hf, idx, ocupados - 1);
    return true;
}

void hf_dump(HashFile hf, FILE *saida) {
    if (hf == NULL || saida == NULL) return;

    fprintf(saida, "DUMP\n");
    fprintf(saida, "*Dump cabecalho\n");
    fprintf(saida, "numBucketsd %d \n", hf->cab.numBuckets);
    fprintf(saida, "sizeRecordd %d \n", hf->cab.tamRegistro);
    fprintf(saida, "sizeBlock %d \n",   hf->cab.tamBloco);
    fprintf(saida, "offsetKey %d \n",   hf->cab.offsetChave);
    fprintf(saida, "sizeKey %d \n",     hf->cab.tamChave);
    fprintf(saida, "offsetTable %ld \n", hf->cab.offsetTable);
    fprintf(saida, "offsetBuckets %ld \n", hf->cab.offsetBuckets);
    fprintf(saida, "offsetOverflow %ld\n", hf->cab.offsetOverflow);

    fprintf(saida, "* Dump table\n");
    for (int i = 0; i < hf->cab.numBuckets; i++) {
        long off = offset_bucket(hf, i);
        fprintf(saida, "[%d] %ld\n", i, off);
    }

    fprintf(saida, "*Dump buckets\n");
    char *buf = (char *)malloc((size_t)hf->cab.tamRegistro);
    if (buf == NULL) return;

    for (int b = 0; b < hf->cab.numBuckets; b++) {
        int ocupados = ler_ocupados(hf, b);
        fprintf(saida, "BLOCO: %d\n", b);
        for (int r = 0; r < ocupados; r++) {
            ler_registro(hf, b, r, buf);

            fprintf(saida, "1 | ");

            if (hf->cab.tamChave == (int)sizeof(int)) {
                int chave_int;
                memcpy(&chave_int, buf + hf->cab.offsetChave, sizeof(int));
                fprintf(saida, "%d", chave_int);
            } else {
                fprintf(saida, "%.*s", hf->cab.tamChave,
                        buf + hf->cab.offsetChave);
            }

            fprintf(saida, " | ");
            for (int j = 0; j < hf->cab.tamRegistro; j++) {
                unsigned char c = (unsigned char)buf[j];
                if (c >= 32 && c < 127) {
                    fputc(c, saida);
                } else {
                    fputc('.', saida);
                }
            }
            fprintf(saida, " |\n");
        }
    }

    free(buf);
}

void hf_iterar(HashFile hf,
               void (*callback)(const void *registro, void *ctx),
               void *ctx) {
    if (hf == NULL || callback == NULL) return;

    char *buf = (char *)malloc((size_t)hf->cab.tamRegistro);
    if (buf == NULL) return;

    for (int b = 0; b < hf->cab.numBuckets; b++) {
        int ocupados = ler_ocupados(hf, b);
        for (int r = 0; r < ocupados; r++) {
            ler_registro(hf, b, r, buf);
            callback(buf, ctx);
        }
    }

    free(buf);
}

int hf_num_buckets(HashFile hf) {
    if (hf == NULL) return 0;
    return hf->cab.numBuckets;
}

int hf_tamanho_registro(HashFile hf) {
    if (hf == NULL) return 0;
    return hf->cab.tamRegistro;
}
