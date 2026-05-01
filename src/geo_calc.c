#include "geo_calc.h"

#include <stddef.h>

void geo_calc_endereco(double qx, double qy, double qw, double qh,
                       char face, int num, double *rx, double *ry) {
    if (rx == NULL || ry == NULL) return;

    double se_x = qx + qw;
    double se_y = qy + qh;

    switch (face) {
        case 'S':
            *rx = se_x - (double)num;
            *ry = se_y;
            break;
        case 'L':
            *rx = se_x;
            *ry = se_y - (double)num;
            break;
        case 'N':
            *rx = se_x - (double)num;
            *ry = qy;
            break;
        case 'O':
            *rx = qx;
            *ry = se_y - (double)num;
            break;
        default:
            *rx = qx;
            *ry = qy;
            break;
    }
}
