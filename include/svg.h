#ifndef SVG_H
#define SVG_H

#include <stdio.h>

/**
 * @file svg.h
 * @brief Módulo de geração de arquivos SVG.
 */

FILE *svg_abrir(const char *caminho, double largura, double altura);
void  svg_fechar(FILE *f);

void svg_retangulo(FILE *f, double x, double y, double w, double h,
                   const char *fill, const char *stroke, double sw);

void svg_linha(FILE *f, double x1, double y1, double x2, double y2,
               const char *cor, double sw);

void svg_circulo(FILE *f, double cx, double cy, double r,
                 const char *fill, const char *stroke, double sw);

void svg_texto(FILE *f, double x, double y, const char *texto,
               const char *cor, double tam);

#endif
