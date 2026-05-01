#ifndef GEO_CALC_H
#define GEO_CALC_H

/**
 * @file geo_calc.h
 * @brief Cálculos geométricos de endereçamento em Bitnópolis.
 *
 * Converte endereço (CEP/face/num) em coordenadas (x, y).
 */

/**
 * Calcula as coordenadas (x, y) de um endereço dado a quadra e seus atributos.
 * @param qx, qy   coordenadas da âncora (canto sudeste) da quadra
 * @param qw, qh   largura e altura da quadra
 * @param face      'N', 'S', 'L' ou 'O'
 * @param num       número do endereço (distância da frente da casa à âncora)
 * @param[out] rx, ry  coordenadas resultantes
 */
void geo_calc_endereco(double qx, double qy, double qw, double qh,
                       char face, int num, double *rx, double *ry);

#endif
