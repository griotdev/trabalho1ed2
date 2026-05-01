#include "pm.h"
#include "habitante.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pm_processar(const char *caminho, HashFile hf_habitantes) {
    if (caminho == NULL || hf_habitantes == NULL) return;

    FILE *fp = fopen(caminho, "r");
    if (fp == NULL) return;

    char linha[512];
    int tam_reg = habitante_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg);
    if (buf == NULL) {
        fclose(fp);
        return;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        char cmd[16];
        if (sscanf(linha, "%15s", cmd) != 1) continue;
        if (cmd[0] == '#') continue;

        if (strcmp(cmd, "p") == 0) {
            char cpf[15], nome[50], sobrenome[50], nasc[11];
            char sexo_str[4];
            if (sscanf(linha, "p %14s %49s %49s %3s %10s",
                       cpf, nome, sobrenome, sexo_str, nasc) == 5) {
                Habitante h = habitante_criar(cpf, nome, sobrenome,
                                              sexo_str[0], nasc);
                if (h != NULL) {
                    memset(buf, 0, (size_t)tam_reg);
                    habitante_serializar(h, buf, tam_reg);
                    hf_inserir(hf_habitantes, buf);
                    habitante_destruir(h);
                }
            }
        } else if (strcmp(cmd, "m") == 0) {
            char cpf[15], cep[20], face[4], compl[20];
            int num;
            if (sscanf(linha, "m %14s %19s %3s %d %19s",
                       cpf, cep, face, &num, compl) == 5) {
                memset(buf, 0, (size_t)tam_reg);
                char chave[15];
                memset(chave, 0, sizeof(chave));
                strncpy(chave, cpf, sizeof(chave) - 1);

                if (hf_buscar(hf_habitantes, chave, buf)) {
                    Habitante h = habitante_desserializar(buf);
                    if (h != NULL) {
                        habitante_set_endereco(h, cep, face, num, compl);
                        memset(buf, 0, (size_t)tam_reg);
                        habitante_serializar(h, buf, tam_reg);
                        hf_remover(hf_habitantes, chave);
                        hf_inserir(hf_habitantes, buf);
                        habitante_destruir(h);
                    }
                }
            }
        }
    }

    free(buf);
    fclose(fp);
}
