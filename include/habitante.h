#ifndef HABITANTE_H
#define HABITANTE_H

/**
 * @file habitante.h
 * @brief TAD Habitante — ponteiro opaco.
 *
 * Habitante modela uma pessoa de Bitnópolis.
 * Se possuir endereço, é também Morador.
 */

typedef struct Habitante_s *Habitante;

Habitante   habitante_criar(const char *cpf, const char *nome,
                            const char *sobrenome, char sexo,
                            const char *nasc);
void        habitante_destruir(Habitante h);

const char *habitante_cpf(Habitante h);
const char *habitante_nome(Habitante h);
const char *habitante_sobrenome(Habitante h);
char        habitante_sexo(Habitante h);
const char *habitante_nasc(Habitante h);

/* Endereço (morador) */
int         habitante_e_morador(Habitante h);
void        habitante_set_endereco(Habitante h, const char *cep,
                                   const char *face, int num,
                                   const char *compl);
void        habitante_remover_endereco(Habitante h);
const char *habitante_cep(Habitante h);
const char *habitante_face(Habitante h);
int         habitante_num(Habitante h);
const char *habitante_compl(Habitante h);

/* Serialização para hashfile */
int        habitante_sizeof_registro(void);
int        habitante_offset_chave(void);
int        habitante_tam_chave(void);
void       habitante_serializar(Habitante h, void *buf, int tam);
Habitante  habitante_desserializar(const void *buf);

#endif
