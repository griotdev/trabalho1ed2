#ifndef MORADOR_H
#define MORADOR_H

/**
 * @file morador.h
 * @brief TAD Morador — view sobre Habitante com endereço.
 *
 * Morador não possui struct própria: é um Habitante cujo campo
 * e_morador == 1 e que possui endereço (CEP/face/num) preenchido.
 *
 * Este módulo fornece funções utilitárias de conveniência.
 */

#include "habitante.h"

/* Torna um habitante morador, atribuindo-lhe endereço */
void morador_atribuir(Habitante h, const char *cep, const char *face,
                      int num, const char *compl);

/* Despeja o morador (remove endereço, torna sem-teto) */
void morador_despejar(Habitante h);

/* Verifica se o habitante é morador */
int morador_eh_morador(Habitante h);

#endif
