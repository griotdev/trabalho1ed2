#include "hashfile.h"
#include <stdlib.h>

/*
 * STUB — implementação completa virá na Fase 3.
 * As structs internas ficam SOMENTE aqui (ponteiro opaco).
 */

struct HashFile_s {
    int placeholder;
};

HashFile hf_criar(const char *caminho, int num_buckets, int tam_registro,
                  int tam_bloco, int offset_chave, int tam_chave) {
    (void)caminho; (void)num_buckets; (void)tam_registro;
    (void)tam_bloco; (void)offset_chave; (void)tam_chave;
    return NULL;
}

HashFile hf_abrir(const char *caminho) {
    (void)caminho;
    return NULL;
}

void hf_fechar(HashFile hf) {
    (void)hf;
}

bool hf_inserir(HashFile hf, const void *registro) {
    (void)hf; (void)registro;
    return false;
}

bool hf_buscar(HashFile hf, const void *chave, void *saida) {
    (void)hf; (void)chave; (void)saida;
    return false;
}

bool hf_remover(HashFile hf, const void *chave) {
    (void)hf; (void)chave;
    return false;
}

void hf_dump(HashFile hf, FILE *saida) {
    (void)hf; (void)saida;
}

void hf_iterar(HashFile hf, void (*callback)(const void *registro, void *ctx), void *ctx) {
    (void)hf; (void)callback; (void)ctx;
}

int hf_num_buckets(HashFile hf) {
    (void)hf;
    return 0;
}

int hf_tamanho_registro(HashFile hf) {
    (void)hf;
    return 0;
}
