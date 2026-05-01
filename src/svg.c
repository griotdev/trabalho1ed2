#include "svg.h"
#include <stdlib.h>

FILE *svg_abrir(const char *caminho, double largura, double altura) {
    (void)caminho; (void)largura; (void)altura;
    return NULL; /* stub */
}

void svg_fechar(FILE *f) { (void)f; }

void svg_retangulo(FILE *f, double x, double y, double w, double h,
                   const char *fill, const char *stroke, double sw) {
    (void)f; (void)x; (void)y; (void)w; (void)h;
    (void)fill; (void)stroke; (void)sw;
}

void svg_linha(FILE *f, double x1, double y1, double x2, double y2,
               const char *cor, double sw) {
    (void)f; (void)x1; (void)y1; (void)x2; (void)y2; (void)cor; (void)sw;
}

void svg_circulo(FILE *f, double cx, double cy, double r,
                 const char *fill, const char *stroke, double sw) {
    (void)f; (void)cx; (void)cy; (void)r; (void)fill; (void)stroke; (void)sw;
}

void svg_texto(FILE *f, double x, double y, const char *texto,
               const char *cor, double tam) {
    (void)f; (void)x; (void)y; (void)texto; (void)cor; (void)tam;
}
