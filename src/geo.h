#ifndef GEO_H
#define GEO_H

#include "hashfile.h"

/**
 * Processa o arquivo .geo e insere quadras no hashfile.
 */
void geo_processar(const char *caminho, HashFile hf_quadras);

#endif
