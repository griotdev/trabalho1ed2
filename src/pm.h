#ifndef PM_H
#define PM_H

#include "hashfile.h"

/**
 * Processa o arquivo .pm e insere habitantes/moradores no hashfile.
 */
void pm_processar(const char *caminho, HashFile hf_habitantes);

#endif
