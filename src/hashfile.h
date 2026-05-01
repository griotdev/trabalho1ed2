#ifndef HASHFILE_H
#define HASHFILE_H

#include <stdbool.h>
#include <stdio.h>

/**
 * @file hashfile.h
 * @brief TAD para Hashfile Dinâmico/Extensível em DISCO.
 *
 * Persiste registros de tamanho fixo em arquivo binário (.hf).
 * A chave de cada registro é extraída via offset+tamanho informados na criação.
 *
 * Ponteiro opaco: a struct interna NÃO é exposta neste .h.
 */

typedef struct HashFile_s *HashFile;

/**
 * Cria um novo hashfile em disco.
 * @param caminho    caminho do arquivo .hf
 * @param num_buckets número inicial de buckets
 * @param tam_registro tamanho fixo de cada registro (bytes)
 * @param tam_bloco   tamanho de cada bloco/bucket em disco (bytes)
 * @param offset_chave offset da chave dentro do registro
 * @param tam_chave    tamanho da chave (bytes)
 * @return handle ou NULL em caso de erro
 */
HashFile hf_criar(const char *caminho, int num_buckets, int tam_registro,
                  int tam_bloco, int offset_chave, int tam_chave);

/**
 * Abre um hashfile existente.
 */
HashFile hf_abrir(const char *caminho);

/**
 * Fecha o hashfile e libera recursos.
 */
void hf_fechar(HashFile hf);

/**
 * Insere um registro no hashfile.
 * @return true se inserido com sucesso; false se chave duplicada ou erro.
 */
bool hf_inserir(HashFile hf, const void *registro);

/**
 * Busca um registro pela chave.
 * @param chave ponteiro para o valor da chave
 * @param saida buffer onde o registro encontrado será copiado
 * @return true se encontrado
 */
bool hf_buscar(HashFile hf, const void *chave, void *saida);

/**
 * Remove um registro pela chave.
 * @return true se removido
 */
bool hf_remover(HashFile hf, const void *chave);

/**
 * Gera o dump textual (.hfd) do hashfile no FILE informado.
 */
void hf_dump(HashFile hf, FILE *saida);

/**
 * Itera sobre todos os registros ocupados, chamando callback para cada um.
 * @param callback função chamada com (registro, contexto) para cada registro
 * @param ctx      contexto opaco passado ao callback
 */
void hf_iterar(HashFile hf, void (*callback)(const void *registro, void *ctx), void *ctx);

/* Getters para testes */
int hf_num_buckets(HashFile hf);
int hf_tamanho_registro(HashFile hf);

#endif
