#include "geo.h"
#include "quadra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Formato do .geo:
 *   cq sw cfill cstrk     — define estilo das próximas quadras
 *   q  cep x y w h        — insere quadra
 *
 * Linhas começando com '#' ou vazias são ignoradas.
 */

void geo_processar(const char *caminho, HashFile hf_quadras) {
    if (caminho == NULL || hf_quadras == NULL) return;

    FILE *fp = fopen(caminho, "r");
    if (fp == NULL) return;

    /* Estado do estilo atual (comando cq) */
    double est_sw = 1.0;
    char   est_cfill[16];
    char   est_cstrk[16];
    strncpy(est_cfill, "#CCCCCC", sizeof(est_cfill));
    strncpy(est_cstrk, "#000000", sizeof(est_cstrk));

    char linha[512];
    int tam_reg = quadra_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg);
    if (buf == NULL) {
        fclose(fp);
        return;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        char cmd[16];
        if (sscanf(linha, "%15s", cmd) != 1) continue;
        if (cmd[0] == '#') continue;

        if (strcmp(cmd, "cq") == 0) {
            /* cq sw cfill cstrk */
            char cfill[16], cstrk[16];
            double sw;
            if (sscanf(linha, "cq %lf %15s %15s", &sw, cfill, cstrk) == 3) {
                est_sw = sw;
                strncpy(est_cfill, cfill, sizeof(est_cfill) - 1);
                est_cfill[sizeof(est_cfill) - 1] = '\0';
                strncpy(est_cstrk, cstrk, sizeof(est_cstrk) - 1);
                est_cstrk[sizeof(est_cstrk) - 1] = '\0';
            }
        } else if (strcmp(cmd, "q") == 0) {
            /* q cep x y w h */
            char cep[20];
            double x, y, w, h;
            if (sscanf(linha, "q %19s %lf %lf %lf %lf", cep, &x, &y, &w, &h) == 5) {
                Quadra q = quadra_criar(cep, x, y, w, h);
                if (q != NULL) {
                    quadra_set_estilo(q, est_sw, est_cfill, est_cstrk);
                    memset(buf, 0, (size_t)tam_reg);
                    quadra_serializar(q, buf, tam_reg);
                    hf_inserir(hf_quadras, buf);
                    quadra_destruir(q);
                }
            }
        }
        /* Comandos desconhecidos são silenciosamente ignorados */
    }

    free(buf);
    fclose(fp);
}
