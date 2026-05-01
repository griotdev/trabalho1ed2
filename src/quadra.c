#include "quadra.h"

#include <stdlib.h>
#include <string.h>

#define QUADRA_CEP_TAM   20
#define QUADRA_CFILL_TAM 12
#define QUADRA_CSTRK_TAM 12

struct Quadra_s {
    char   cep[QUADRA_CEP_TAM];
    double x, y, w, h;
    float  sw;
    char   cfill[QUADRA_CFILL_TAM];
    char   cstrk[QUADRA_CSTRK_TAM];
};

Quadra quadra_criar(const char *cep, double x, double y, double w, double h) {
    if (cep == NULL) return NULL;

    Quadra q = (Quadra)malloc(sizeof(struct Quadra_s));
    if (q == NULL) return NULL;

    memset(q, 0, sizeof(struct Quadra_s));
    strncpy(q->cep, cep, QUADRA_CEP_TAM - 1);
    q->x = x;
    q->y = y;
    q->w = w;
    q->h = h;
    q->sw = 1.0f;
    strncpy(q->cfill, "#CCCCCC", QUADRA_CFILL_TAM - 1);
    strncpy(q->cstrk, "#000000", QUADRA_CSTRK_TAM - 1);
    return q;
}

void quadra_destruir(Quadra q) {
    free(q);
}

const char *quadra_cep(Quadra q) {
    if (q == NULL) return NULL;
    return q->cep;
}

double quadra_x(Quadra q) { return q == NULL ? 0.0 : q->x; }
double quadra_y(Quadra q) { return q == NULL ? 0.0 : q->y; }
double quadra_w(Quadra q) { return q == NULL ? 0.0 : q->w; }
double quadra_h(Quadra q) { return q == NULL ? 0.0 : q->h; }

void quadra_set_estilo(Quadra q, double sw, const char *cfill, const char *cstrk) {
    if (q == NULL) return;
    q->sw = (float)sw;
    if (cfill != NULL) {
        memset(q->cfill, 0, QUADRA_CFILL_TAM);
        strncpy(q->cfill, cfill, QUADRA_CFILL_TAM - 1);
    }
    if (cstrk != NULL) {
        memset(q->cstrk, 0, QUADRA_CSTRK_TAM);
        strncpy(q->cstrk, cstrk, QUADRA_CSTRK_TAM - 1);
    }
}

double      quadra_sw(Quadra q)    { return q == NULL ? 0.0 : (double)q->sw; }
const char *quadra_cfill(Quadra q) { return q == NULL ? NULL : q->cfill; }
const char *quadra_cstrk(Quadra q) { return q == NULL ? NULL : q->cstrk; }

int quadra_sizeof_registro(void) {
    return (int)sizeof(struct Quadra_s);
}

int quadra_offset_chave(void) {
    return 0;
}

int quadra_tam_chave(void) {
    return QUADRA_CEP_TAM;
}

void quadra_serializar(Quadra q, void *buf, int tam) {
    if (q == NULL || buf == NULL) return;
    int copia = tam < (int)sizeof(struct Quadra_s) ? tam : (int)sizeof(struct Quadra_s);
    memset(buf, 0, (size_t)tam);
    memcpy(buf, q, (size_t)copia);
}

Quadra quadra_desserializar(const void *buf) {
    if (buf == NULL) return NULL;

    Quadra q = (Quadra)malloc(sizeof(struct Quadra_s));
    if (q == NULL) return NULL;

    memcpy(q, buf, sizeof(struct Quadra_s));
    q->cep[QUADRA_CEP_TAM - 1] = '\0';
    q->cfill[QUADRA_CFILL_TAM - 1] = '\0';
    q->cstrk[QUADRA_CSTRK_TAM - 1] = '\0';
    return q;
}
