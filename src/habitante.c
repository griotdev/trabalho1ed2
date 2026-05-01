#include "habitante.h"

#include <stdlib.h>
#include <string.h>

/* ========== Tamanhos dos campos ========== */

#define HAB_CPF_TAM       15
#define HAB_NOME_TAM      50
#define HAB_SOBRENOME_TAM 50
#define HAB_NASC_TAM      11
#define HAB_CEP_TAM       20
#define HAB_FACE_TAM       2
#define HAB_COMPL_TAM     20

/* Struct interna — NÃO exposta no .h (ponteiro opaco) */
struct Habitante_s {
    char cpf[HAB_CPF_TAM];
    char nome[HAB_NOME_TAM];
    char sobrenome[HAB_SOBRENOME_TAM];
    char sexo;
    char nasc[HAB_NASC_TAM];
    int  e_morador;
    char cep[HAB_CEP_TAM];
    char face[HAB_FACE_TAM];
    int  num;
    char compl[HAB_COMPL_TAM];
};

/* ========== Construtor / Destrutor ========== */

Habitante habitante_criar(const char *cpf, const char *nome,
                          const char *sobrenome, char sexo,
                          const char *nasc) {
    if (cpf == NULL || nome == NULL || sobrenome == NULL || nasc == NULL)
        return NULL;

    Habitante h = (Habitante)malloc(sizeof(struct Habitante_s));
    if (h == NULL) return NULL;

    memset(h, 0, sizeof(struct Habitante_s));
    strncpy(h->cpf, cpf, HAB_CPF_TAM - 1);
    strncpy(h->nome, nome, HAB_NOME_TAM - 1);
    strncpy(h->sobrenome, sobrenome, HAB_SOBRENOME_TAM - 1);
    h->sexo = sexo;
    strncpy(h->nasc, nasc, HAB_NASC_TAM - 1);
    h->e_morador = 0;
    return h;
}

void habitante_destruir(Habitante h) {
    free(h);
}

/* ========== Getters básicos ========== */

const char *habitante_cpf(Habitante h) {
    return h == NULL ? NULL : h->cpf;
}

const char *habitante_nome(Habitante h) {
    return h == NULL ? NULL : h->nome;
}

const char *habitante_sobrenome(Habitante h) {
    return h == NULL ? NULL : h->sobrenome;
}

char habitante_sexo(Habitante h) {
    return h == NULL ? '?' : h->sexo;
}

const char *habitante_nasc(Habitante h) {
    return h == NULL ? NULL : h->nasc;
}

/* ========== Endereço (morador) ========== */

int habitante_e_morador(Habitante h) {
    return h == NULL ? 0 : h->e_morador;
}

void habitante_set_endereco(Habitante h, const char *cep,
                            const char *face, int num,
                            const char *compl_) {
    if (h == NULL) return;

    h->e_morador = 1;

    memset(h->cep, 0, HAB_CEP_TAM);
    if (cep != NULL) strncpy(h->cep, cep, HAB_CEP_TAM - 1);

    memset(h->face, 0, HAB_FACE_TAM);
    if (face != NULL) strncpy(h->face, face, HAB_FACE_TAM - 1);

    h->num = num;

    memset(h->compl, 0, HAB_COMPL_TAM);
    if (compl_ != NULL) strncpy(h->compl, compl_, HAB_COMPL_TAM - 1);
}

void habitante_remover_endereco(Habitante h) {
    if (h == NULL) return;
    h->e_morador = 0;
    memset(h->cep, 0, HAB_CEP_TAM);
    memset(h->face, 0, HAB_FACE_TAM);
    h->num = 0;
    memset(h->compl, 0, HAB_COMPL_TAM);
}

const char *habitante_cep(Habitante h) {
    return h == NULL ? NULL : h->cep;
}

const char *habitante_face(Habitante h) {
    return h == NULL ? NULL : h->face;
}

int habitante_num(Habitante h) {
    return h == NULL ? 0 : h->num;
}

const char *habitante_compl(Habitante h) {
    return h == NULL ? NULL : h->compl;
}

/* ========== Serialização ========== */

void habitante_serializar(Habitante h, void *buf, int tam) {
    if (h == NULL || buf == NULL) return;
    int copia = tam < (int)sizeof(struct Habitante_s) ? tam : (int)sizeof(struct Habitante_s);
    memset(buf, 0, (size_t)tam);
    memcpy(buf, h, (size_t)copia);
}

Habitante habitante_desserializar(const void *buf) {
    if (buf == NULL) return NULL;

    Habitante h = (Habitante)malloc(sizeof(struct Habitante_s));
    if (h == NULL) return NULL;

    memcpy(h, buf, sizeof(struct Habitante_s));
    /* Garantir null-terminadores */
    h->cpf[HAB_CPF_TAM - 1] = '\0';
    h->nome[HAB_NOME_TAM - 1] = '\0';
    h->sobrenome[HAB_SOBRENOME_TAM - 1] = '\0';
    h->nasc[HAB_NASC_TAM - 1] = '\0';
    h->cep[HAB_CEP_TAM - 1] = '\0';
    h->face[HAB_FACE_TAM - 1] = '\0';
    h->compl[HAB_COMPL_TAM - 1] = '\0';
    return h;
}
