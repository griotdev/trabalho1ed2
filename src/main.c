#include <stdio.h>
#include <string.h>

/**
 * @file main.c
 * @brief Ponto de entrada do executável ted.
 *
 * Argumentos:
 *   -e  diretório de entrada
 *   -f  arquivo .geo
 *   -pm arquivo .pm
 *   -q  arquivo .qry
 *   -o  diretório de saída
 */

int main(int argc, char *argv[]) {
    char *dir_entrada = NULL;
    char *arq_geo     = NULL;
    char *arq_pm      = NULL;
    char *arq_qry     = NULL;
    char *dir_saida   = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            dir_entrada = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            arq_geo = argv[++i];
        } else if (strcmp(argv[i], "-pm") == 0 && i + 1 < argc) {
            arq_pm = argv[++i];
        } else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            arq_qry = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            dir_saida = argv[++i];
        }
    }

    if (dir_entrada == NULL || arq_geo == NULL || dir_saida == NULL) {
        fprintf(stderr, "Uso: ted -e <dir_entrada> -f <arq.geo> [-pm <arq.pm>] [-q <arq.qry>] -o <dir_saida>\n");
        return 1;
    }

    printf("SIG Bitnoopolis\n");
    printf("  Entrada:  %s\n", dir_entrada);
    printf("  GEO:      %s\n", arq_geo);
    printf("  PM:       %s\n", arq_pm ? arq_pm : "(nenhum)");
    printf("  QRY:      %s\n", arq_qry ? arq_qry : "(nenhum)");
    printf("  Saida:    %s\n", dir_saida);

    /* TODO: integrar parsers e hashfiles nas próximas fases */

    return 0;
}
