#ifndef QRY_H
#define QRY_H

#include "hashfile.h"
#include <stdio.h>

/**
 * Processa o arquivo .qry, executando consultas e modificações.
 */
void qry_processar(const char *caminho,
                   HashFile hf_quadras, HashFile hf_habitantes,
                   FILE *svg, FILE *txt);

#endif
