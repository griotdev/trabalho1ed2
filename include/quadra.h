#ifndef QUADRA_H
#define QUADRA_H

/**
 * @file quadra.h
 * @brief TAD Quadra — ponteiro opaco.
 */

typedef struct Quadra_s *Quadra;

Quadra      quadra_criar(const char *cep, double x, double y, double w, double h);
void        quadra_destruir(Quadra q);
const char *quadra_cep(Quadra q);
double      quadra_x(Quadra q);
double      quadra_y(Quadra q);
double      quadra_w(Quadra q);
double      quadra_h(Quadra q);

/* Estilo visual */
void        quadra_set_estilo(Quadra q, double sw, const char *cfill, const char *cstrk);
double      quadra_sw(Quadra q);
const char *quadra_cfill(Quadra q);
const char *quadra_cstrk(Quadra q);

/* Serialização para hashfile */
void   quadra_serializar(Quadra q, void *buf, int tam);
Quadra quadra_desserializar(const void *buf);

#endif
