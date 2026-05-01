#include "geo_calc.h"

#include <stddef.h>

void geo_calc_endereco(double qx, double qy, double qw, double qh,
                       char face, int num, double *rx, double *ry) {
    if (rx == NULL || ry == NULL) return;

    double se_x = qx + qw;
    double se_y = qy + qh;

    switch (face) {
        case 'S': // Face superior (top)
            *rx = se_x - (double)num;
            *ry = qy;
            break;
        case 'N': // Face inferior (bottom)
            *rx = se_x - (double)num;
            *ry = se_y;
            break;
        case 'L': // Face direita (right), partindo do Sul (topo) para o Norte (baixo)
            *rx = se_x;
            *ry = qy + (double)num;
            break;
        case 'O': // Face esquerda (left), partindo do Sul (topo) para o Norte (baixo)
            *rx = qx;
            *ry = qy + (double)num;
            break;
        default:
            *rx = qx;
            *ry = qy;
            break;
    }
}
