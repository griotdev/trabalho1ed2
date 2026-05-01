#include "hashfile.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    int tamRegistro;
    int tamBloco;
    int offsetChave;
    int tamChave;
    int pG;
    long offsetDiretorio;
} HFCabecalho;

struct HashFile_s {
    FILE       *fp;
    HFCabecalho cab;
    int         capacidade;
    long       *diretorio;
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

static int indice_diretorio(const HashFile hf, const void *chave) {
    unsigned int h = hash_bytes(chave, hf->cab.tamChave);
    unsigned int mascara = (1u << hf->cab.pG) - 1u;
    return (int)(h & mascara);
}

static const void *extrair_chave(const HashFile hf, const void *registro) {
    return (const char *)registro + hf->cab.offsetChave;
}

static void ler_registro(const HashFile hf, long off_bucket, int pos, void *saida) {
    long off = off_bucket + 2 * sizeof(int) + (long)pos * hf->cab.tamRegistro;
    fseek(hf->fp, off, SEEK_SET);
    fread(saida, (size_t)hf->cab.tamRegistro, 1, hf->fp);
}

static void escrever_registro(const HashFile hf, long off_bucket, int pos, const void *registro) {
    long off = off_bucket + 2 * sizeof(int) + (long)pos * hf->cab.tamRegistro;
    fseek(hf->fp, off, SEEK_SET);
    fwrite(registro, (size_t)hf->cab.tamRegistro, 1, hf->fp);
}

static int buscar_no_bucket(const HashFile hf, long off_bucket, const void *chave) {
    int pL, ocupados;
    fseek(hf->fp, off_bucket, SEEK_SET);
    fread(&pL, sizeof(int), 1, hf->fp);
    fread(&ocupados, sizeof(int), 1, hf->fp);

    char *buf = (char *)malloc((size_t)hf->cab.tamRegistro);
    if (buf == NULL) return -1;

    for (int i = 0; i < ocupados; i++) {
        ler_registro(hf, off_bucket, i, buf);
        const void *k = extrair_chave(hf, buf);
        if (memcmp(k, chave, (size_t)hf->cab.tamChave) == 0) {
            free(buf);
            return i;
        }
    }
    free(buf);
    return -1;
}

static bool duplicar_diretorio(HashFile hf) {
    int tam_antigo = 1 << hf->cab.pG;
    int novo_pG = hf->cab.pG + 1;
    int tam_novo = 1 << novo_pG;

    long *novo_dir = (long *)malloc((size_t)tam_novo * sizeof(long));
    if (novo_dir == NULL) return false;

    for (int i = 0; i < tam_antigo; i++) {
        novo_dir[i] = hf->diretorio[i];
        novo_dir[i + tam_antigo] = hf->diretorio[i];
    }

    free(hf->diretorio);
    hf->diretorio = novo_dir;
    hf->cab.pG = novo_pG;

    fseek(hf->fp, 0, SEEK_END);
    hf->cab.offsetDiretorio = ftell(hf->fp);
    fwrite(hf->diretorio, sizeof(long), (size_t)tam_novo, hf->fp);

    fseek(hf->fp, 0, SEEK_SET);
    fwrite(&hf->cab, sizeof(HFCabecalho), 1, hf->fp);
    fflush(hf->fp);

    return true;
}

static void inserir_no_bucket(HashFile hf, long off_bucket, const void *registro) {
    int pL, ocupados;
    fseek(hf->fp, off_bucket, SEEK_SET);
    fread(&pL, sizeof(int), 1, hf->fp);
    fread(&ocupados, sizeof(int), 1, hf->fp);

    escrever_registro(hf, off_bucket, ocupados, registro);
    ocupados++;

    fseek(hf->fp, off_bucket + sizeof(int), SEEK_SET);
    fwrite(&ocupados, sizeof(int), 1, hf->fp);
}

static bool dividir_bucket(HashFile hf, int idx_dir) {
    long off_orig = hf->diretorio[idx_dir];

    int pL_orig, ocup_orig;
    fseek(hf->fp, off_orig, SEEK_SET);
    fread(&pL_orig, sizeof(int), 1, hf->fp);
    fread(&ocup_orig, sizeof(int), 1, hf->fp);

    int nova_pL = pL_orig + 1;

    fseek(hf->fp, 0, SEEK_END);
    long off_novo = ftell(hf->fp);
    int pL_novo = nova_pL;
    int ocup_novo = 0;

    char *bloco_vazio = (char *)calloc(1, (size_t)hf->cab.tamBloco);
    if (!bloco_vazio) return false;
    memcpy(bloco_vazio, &pL_novo, sizeof(int));
    memcpy(bloco_vazio + sizeof(int), &ocup_novo, sizeof(int));
    fwrite(bloco_vazio, (size_t)hf->cab.tamBloco, 1, hf->fp);
    free(bloco_vazio);

    char *todos_regs = (char *)malloc((size_t)hf->capacidade * (size_t)hf->cab.tamRegistro);
    if (!todos_regs) return false;

    fseek(hf->fp, off_orig + 2 * sizeof(int), SEEK_SET);
    fread(todos_regs, (size_t)hf->cab.tamRegistro, (size_t)ocup_orig, hf->fp);

    fseek(hf->fp, off_orig, SEEK_SET);
    fwrite(&nova_pL, sizeof(int), 1, hf->fp);
    int zero = 0;
    fwrite(&zero, sizeof(int), 1, hf->fp);

    int bit_seletor = 1 << (nova_pL - 1);
    int tam_dir = 1 << hf->cab.pG;
    for (int i = 0; i < tam_dir; i++) {
        if (hf->diretorio[i] == off_orig && (i & bit_seletor) != 0) {
            hf->diretorio[i] = off_novo;
        }
    }

    for (int i = 0; i < ocup_orig; i++) {
        char *reg = todos_regs + i * hf->cab.tamRegistro;
        const void *k = extrair_chave(hf, reg);
        unsigned int h = hash_bytes(k, hf->cab.tamChave);
        if ((h & bit_seletor) != 0) {
            inserir_no_bucket(hf, off_novo, reg);
        } else {
            inserir_no_bucket(hf, off_orig, reg);
        }
    }

    free(todos_regs);

    fseek(hf->fp, hf->cab.offsetDiretorio, SEEK_SET);
    fwrite(hf->diretorio, sizeof(long), (size_t)tam_dir, hf->fp);
    fflush(hf->fp);

    return true;
}

HashFile hf_criar(const char *caminho, int num_buckets, int tam_registro,
                  int tam_bloco, int offset_chave, int tam_chave) {
    if (caminho == NULL || tam_registro <= 0 || tam_bloco <= 0 || tam_chave <= 0) {
        return NULL;
    }
    (void)num_buckets;

    int capacidade = (tam_bloco - 2 * (int)sizeof(int)) / tam_registro;
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

    hf->cab.tamRegistro   = tam_registro;
    hf->cab.tamBloco      = tam_bloco;
    hf->cab.offsetChave   = offset_chave;
    hf->cab.tamChave      = tam_chave;
    hf->cab.pG            = 1;
    hf->cab.offsetDiretorio = (long)sizeof(HFCabecalho) + 2L * (long)tam_bloco;

    fseek(fp, sizeof(HFCabecalho), SEEK_SET);

    char *bloco_vazio = (char *)calloc(1, (size_t)tam_bloco);
    if (bloco_vazio == NULL) {
        fclose(fp);
        free(hf);
        return NULL;
    }
    int pL = 1;
    memcpy(bloco_vazio, &pL, sizeof(int));
    fwrite(bloco_vazio, (size_t)tam_bloco, 1, fp);
    fwrite(bloco_vazio, (size_t)tam_bloco, 1, fp);
    free(bloco_vazio);

    hf->diretorio = (long *)malloc(2 * sizeof(long));
    hf->diretorio[0] = (long)sizeof(HFCabecalho);
    hf->diretorio[1] = (long)sizeof(HFCabecalho) + (long)tam_bloco;

    fwrite(hf->diretorio, sizeof(long), 2, fp);

    fseek(fp, 0, SEEK_SET);
    fwrite(&hf->cab, sizeof(HFCabecalho), 1, fp);
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

    hf->capacidade = (hf->cab.tamBloco - 2 * (int)sizeof(int)) / hf->cab.tamRegistro;

    int tam_dir = 1 << hf->cab.pG;
    hf->diretorio = (long *)malloc((size_t)tam_dir * sizeof(long));
    fseek(fp, hf->cab.offsetDiretorio, SEEK_SET);
    fread(hf->diretorio, sizeof(long), (size_t)tam_dir, fp);

    return hf;
}

void hf_fechar(HashFile hf) {
    if (hf == NULL) return;
    if (hf->fp != NULL) {
        fseek(hf->fp, hf->cab.offsetDiretorio, SEEK_SET);
        fwrite(hf->diretorio, sizeof(long), (size_t)(1 << hf->cab.pG), hf->fp);
        fclose(hf->fp);
    }
    free(hf->diretorio);
    free(hf);
}

bool hf_inserir(HashFile hf, const void *registro) {
    if (hf == NULL || registro == NULL) return false;

    while (true) {
        const void *chave = extrair_chave(hf, registro);
        int idx = indice_diretorio(hf, chave);
        long off = hf->diretorio[idx];

        if (buscar_no_bucket(hf, off, chave) >= 0) {
            return false;
        }

        int pL, ocupados;
        fseek(hf->fp, off, SEEK_SET);
        fread(&pL, sizeof(int), 1, hf->fp);
        fread(&ocupados, sizeof(int), 1, hf->fp);

        if (ocupados < hf->capacidade) {
            inserir_no_bucket(hf, off, registro);
            fflush(hf->fp);
            return true;
        }

        if (pL == hf->cab.pG) {
            if (!duplicar_diretorio(hf)) return false;
        } else {
            if (!dividir_bucket(hf, idx)) return false;
        }
    }
}

bool hf_buscar(HashFile hf, const void *chave, void *saida) {
    if (hf == NULL || chave == NULL) return false;

    int idx = indice_diretorio(hf, chave);
    long off = hf->diretorio[idx];
    int pos = buscar_no_bucket(hf, off, chave);
    if (pos < 0) return false;

    ler_registro(hf, off, pos, saida);
    return true;
}

bool hf_remover(HashFile hf, const void *chave) {
    if (hf == NULL || chave == NULL) return false;

    int idx = indice_diretorio(hf, chave);
    long off = hf->diretorio[idx];
    int pos = buscar_no_bucket(hf, off, chave);
    if (pos < 0) return false;

    int pL, ocupados;
    fseek(hf->fp, off, SEEK_SET);
    fread(&pL, sizeof(int), 1, hf->fp);
    fread(&ocupados, sizeof(int), 1, hf->fp);

    if (pos < ocupados - 1) {
        char *ultimo = (char *)malloc((size_t)hf->cab.tamRegistro);
        if (ultimo == NULL) return false;
        ler_registro(hf, off, ocupados - 1, ultimo);
        escrever_registro(hf, off, pos, ultimo);
        free(ultimo);
    }

    ocupados--;
    fseek(hf->fp, off + sizeof(int), SEEK_SET);
    fwrite(&ocupados, sizeof(int), 1, hf->fp);
    fflush(hf->fp);
    return true;
}

void hf_dump(HashFile hf, FILE *saida) {
    if (hf == NULL || saida == NULL) return;

    fprintf(saida, "DUMP\n");
    fprintf(saida, "*Dump cabecalho\n");
    fprintf(saida, "sizeRecordd %d \n", hf->cab.tamRegistro);
    fprintf(saida, "sizeBlock %d \n",   hf->cab.tamBloco);
    fprintf(saida, "offsetKey %d \n",   hf->cab.offsetChave);
    fprintf(saida, "sizeKey %d \n",     hf->cab.tamChave);
    fprintf(saida, "PG %d \n",          hf->cab.pG);
    fprintf(saida, "offsetDiretorio %ld \n", hf->cab.offsetDiretorio);

    fprintf(saida, "* Dump table\n");
    int tam_dir = 1 << hf->cab.pG;
    for (int i = 0; i < tam_dir; i++) {
        fprintf(saida, "[%d] %ld\n", i, hf->diretorio[i]);
    }

    fprintf(saida, "*Dump buckets\n");
    char *buf = (char *)malloc((size_t)hf->cab.tamRegistro);
    if (buf == NULL) return;

    for (int i = 0; i < tam_dir; i++) {
        bool ja_impresso = false;
        for (int j = 0; j < i; j++) {
            if (hf->diretorio[i] == hf->diretorio[j]) {
                ja_impresso = true;
                break;
            }
        }
        if (ja_impresso) continue;

        long off = hf->diretorio[i];
        int pL, ocupados;
        fseek(hf->fp, off, SEEK_SET);
        fread(&pL, sizeof(int), 1, hf->fp);
        fread(&ocupados, sizeof(int), 1, hf->fp);

        fprintf(saida, "BLOCO OFF: %ld (pL: %d)\n", off, pL);
        for (int r = 0; r < ocupados; r++) {
            ler_registro(hf, off, r, buf);

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

    int tam_dir = 1 << hf->cab.pG;
    char *buf = (char *)malloc((size_t)hf->cab.tamRegistro);
    if (buf == NULL) return;

    for (int i = 0; i < tam_dir; i++) {
        bool ja_visto = false;
        for (int j = 0; j < i; j++) {
            if (hf->diretorio[i] == hf->diretorio[j]) {
                ja_visto = true;
                break;
            }
        }
        if (ja_visto) continue;

        long off = hf->diretorio[i];
        int pL, ocupados;
        fseek(hf->fp, off, SEEK_SET);
        fread(&pL, sizeof(int), 1, hf->fp);
        fread(&ocupados, sizeof(int), 1, hf->fp);

        for (int r = 0; r < ocupados; r++) {
            ler_registro(hf, off, r, buf);
            callback(buf, ctx);
        }
    }
    free(buf);
}

int hf_num_buckets(HashFile hf) {
    if (hf == NULL) return 0;
    int tam_dir = 1 << hf->cab.pG;
    int distintos = 0;
    for (int i = 0; i < tam_dir; i++) {
        bool ja_contado = false;
        for (int j = 0; j < i; j++) {
            if (hf->diretorio[i] == hf->diretorio[j]) {
                ja_contado = true;
                break;
            }
        }
        if (!ja_contado) distintos++;
    }
    return distintos;
}

int hf_tamanho_registro(HashFile hf) {
    if (hf == NULL) return 0;
    return hf->cab.tamRegistro;
}
