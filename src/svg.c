#include "svg.h"

#include <stdlib.h>

FILE *svg_abrir(const char *caminho, double largura, double altura) {
    if (caminho == NULL) return NULL;

    FILE *f = fopen(caminho, "w");
    if (f == NULL) return NULL;

    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" "
               "width=\"%.0f\" height=\"%.0f\">\n", largura, altura);
    return f;
}

void svg_fechar(FILE *f) {
    if (f == NULL) return;
    fprintf(f, "</svg>\n");
    fclose(f);
}

void svg_retangulo(FILE *f, double x, double y, double w, double h,
                   const char *fill, const char *stroke, double sw) {
    if (f == NULL) return;
    fprintf(f, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" "
               "fill=\"%s\" stroke=\"%s\" stroke-width=\"%.2f\" />\n",
            x, y, w, h,
            fill ? fill : "none",
            stroke ? stroke : "none",
            sw);
}

void svg_linha(FILE *f, double x1, double y1, double x2, double y2,
               const char *cor, double sw) {
    if (f == NULL) return;
    fprintf(f, "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" "
               "stroke=\"%s\" stroke-width=\"%.2f\" />\n",
            x1, y1, x2, y2,
            cor ? cor : "black",
            sw);
}

void svg_circulo(FILE *f, double cx, double cy, double r,
                 const char *fill, const char *stroke, double sw) {
    if (f == NULL) return;
    fprintf(f, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" "
               "fill=\"%s\" stroke=\"%s\" stroke-width=\"%.2f\" />\n",
            cx, cy, r,
            fill ? fill : "none",
            stroke ? stroke : "none",
            sw);
}

void svg_texto(FILE *f, double x, double y, const char *texto,
               const char *cor, double tam) {
    if (f == NULL || texto == NULL) return;
    fprintf(f, "  <text x=\"%.2f\" y=\"%.2f\" fill=\"%s\" "
               "font-size=\"%.1f\">%s</text>\n",
            x, y,
            cor ? cor : "black",
            tam, texto);
}
