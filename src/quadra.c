#include "quadra.h"
#include <stdlib.h>

/* Struct interna — NÃO exposta no .h (ponteiro opaco) */
struct Quadra_s {
    char   cep[20];
    double x, y, w, h;
    double sw;
    char   cfill[16];
    char   cstrk[16];
};

Quadra quadra_criar(const char *cep, double x, double y, double w, double h) {
    (void)cep; (void)x; (void)y; (void)w; (void)h;
    return NULL; /* stub */
}

void quadra_destruir(Quadra q) { (void)q; }

const char *quadra_cep(Quadra q) { (void)q; return NULL; }
double quadra_x(Quadra q) { (void)q; return 0; }
double quadra_y(Quadra q) { (void)q; return 0; }
double quadra_w(Quadra q) { (void)q; return 0; }
double quadra_h(Quadra q) { (void)q; return 0; }

void quadra_set_estilo(Quadra q, double sw, const char *cfill, const char *cstrk) {
    (void)q; (void)sw; (void)cfill; (void)cstrk;
}
double      quadra_sw(Quadra q)    { (void)q; return 0; }
const char *quadra_cfill(Quadra q) { (void)q; return NULL; }
const char *quadra_cstrk(Quadra q) { (void)q; return NULL; }

void   quadra_serializar(Quadra q, void *buf, int tam) { (void)q; (void)buf; (void)tam; }
Quadra quadra_desserializar(const void *buf) { (void)buf; return NULL; }
