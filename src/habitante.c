#include "habitante.h"
#include <stdlib.h>

/* Struct interna — NÃO exposta no .h (ponteiro opaco) */
struct Habitante_s {
    char   cpf[15];
    char   nome[50];
    char   sobrenome[50];
    char   sexo;
    char   nasc[11];
    int    e_morador;
    char   cep[20];
    char   face[2];
    int    num;
    char   compl[20];
};

Habitante habitante_criar(const char *cpf, const char *nome,
                          const char *sobrenome, char sexo,
                          const char *nasc) {
    (void)cpf; (void)nome; (void)sobrenome; (void)sexo; (void)nasc;
    return NULL; /* stub */
}

void        habitante_destruir(Habitante h)    { (void)h; }
const char *habitante_cpf(Habitante h)         { (void)h; return NULL; }
const char *habitante_nome(Habitante h)        { (void)h; return NULL; }
const char *habitante_sobrenome(Habitante h)   { (void)h; return NULL; }
char        habitante_sexo(Habitante h)        { (void)h; return '?'; }
const char *habitante_nasc(Habitante h)        { (void)h; return NULL; }

int habitante_e_morador(Habitante h)           { (void)h; return 0; }
void habitante_set_endereco(Habitante h, const char *cep,
                            const char *face, int num,
                            const char *compl_) {
    (void)h; (void)cep; (void)face; (void)num; (void)compl_;
}
void        habitante_remover_endereco(Habitante h) { (void)h; }
const char *habitante_cep(Habitante h)         { (void)h; return NULL; }
const char *habitante_face(Habitante h)        { (void)h; return NULL; }
int         habitante_num(Habitante h)         { (void)h; return 0; }
const char *habitante_compl(Habitante h)       { (void)h; return NULL; }

void habitante_serializar(Habitante h, void *buf, int tam) {
    (void)h; (void)buf; (void)tam;
}
Habitante habitante_desserializar(const void *buf) {
    (void)buf; return NULL;
}
