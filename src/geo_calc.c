#include "geo_calc.h"

#include <stddef.h>

void geo_calc_endereco(double qx, double qy, double qw, double qh,
                       char face, int num, double *rx, double *ry) {
    if (rx == NULL || ry == NULL) return;

    switch (face) {
        case 'S':
            *rx = qx + (double)num;
            *ry = qy;
            break;
        case 'N':
            *rx = qx + (double)num;
            *ry = qy + qh;
            break;
        case 'L':
            *rx = qx;
            *ry = qy + (double)num;
            break;
        case 'O':
            *rx = qx + qw;
            *ry = qy + (double)num;
            break;
        default:
            *rx = qx;
            *ry = qy;
            break;
    }
}
