#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hashfile.h"
#include "quadra.h"
#include "habitante.h"
#include "geo.h"
#include "pm.h"
#include "qry.h"
#include "svg.h"

/**
 * Ponto de entrada do executável ted.
 *
 * Argumentos:
 *   -e  diretório de entrada
 *   -f  arquivo .geo
 *   -pm arquivo .pm
 *   -q  arquivo .qry
 *   -o  diretório de saída
 */

static void extrair_nome_sem_ext(const char *arquivo, char *saida, int tam) {
    const char *base = strrchr(arquivo, '/');
    if (base == NULL) base = strrchr(arquivo, '\\');
    base = base ? base + 1 : arquivo;

    strncpy(saida, base, (size_t)(tam - 1));
    saida[tam - 1] = '\0';

    char *ponto = strrchr(saida, '.');
    if (ponto != NULL) *ponto = '\0';
}

static void desenhar_quadras(const void *registro, void *ctx) {
    FILE *svg_f = (FILE *)ctx;
    Quadra q = quadra_desserializar(registro);
    if (q == NULL) return;

    svg_retangulo(svg_f, quadra_x(q), quadra_y(q),
                  quadra_w(q), quadra_h(q),
                  quadra_cfill(q), quadra_cstrk(q), quadra_sw(q));

    svg_texto(svg_f, quadra_x(q) + 2, quadra_y(q) + 12,
              quadra_cep(q), "black", 8);

    quadra_destruir(q);
}

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
        fprintf(stderr, "Uso: ted -e <dir_entrada> -f <arq.geo> "
                        "[-pm <arq.pm>] [-q <arq.qry>] -o <dir_saida>\n");
        return 1;
    }

    /* Montar caminhos completos */
    char caminho_geo[512], caminho_pm[512], caminho_qry[512];
    snprintf(caminho_geo, sizeof(caminho_geo), "%s/%s", dir_entrada, arq_geo);

    if (arq_pm != NULL)
        snprintf(caminho_pm, sizeof(caminho_pm), "%s/%s", dir_entrada, arq_pm);
    if (arq_qry != NULL)
        snprintf(caminho_qry, sizeof(caminho_qry), "%s/%s", dir_entrada, arq_qry);

    /* Extrair nomes base */
    char nome_geo[128], nome_qry[128];
    extrair_nome_sem_ext(arq_geo, nome_geo, sizeof(nome_geo));
    if (arq_qry != NULL)
        extrair_nome_sem_ext(arq_qry, nome_qry, sizeof(nome_qry));

    /* Caminhos de saída */
    char caminho_hf_q[512], caminho_hf_h[512];
    char caminho_hfd_q[512], caminho_hfd_h[512];
    char caminho_svg[512], caminho_txt[512];

    snprintf(caminho_hf_q, sizeof(caminho_hf_q), "%s/%s-quadras.hf", dir_saida, nome_geo);
    snprintf(caminho_hf_h, sizeof(caminho_hf_h), "%s/%s-habitantes.hf", dir_saida, nome_geo);
    snprintf(caminho_hfd_q, sizeof(caminho_hfd_q), "%s/%s-quadras.hfd", dir_saida, nome_geo);
    snprintf(caminho_hfd_h, sizeof(caminho_hfd_h), "%s/%s-habitantes.hfd", dir_saida, nome_geo);

    if (arq_qry != NULL) {
        snprintf(caminho_svg, sizeof(caminho_svg), "%s/%s-%s.svg", dir_saida, nome_geo, nome_qry);
        snprintf(caminho_txt, sizeof(caminho_txt), "%s/%s-%s.txt", dir_saida, nome_geo, nome_qry);
    } else {
        snprintf(caminho_svg, sizeof(caminho_svg), "%s/%s.svg", dir_saida, nome_geo);
        caminho_txt[0] = '\0';
    }

    /* Criar hashfiles */
    int tam_q = quadra_sizeof_registro();
    HashFile hf_quadras = hf_criar(caminho_hf_q, 16, tam_q, 4096,
                                   quadra_offset_chave(), quadra_tam_chave());
    if (hf_quadras == NULL) {
        fprintf(stderr, "Erro ao criar hashfile de quadras\n");
        return 1;
    }

    int tam_h = habitante_sizeof_registro();
    HashFile hf_habitantes = hf_criar(caminho_hf_h, 32, tam_h, 4096,
                                      habitante_offset_chave(), habitante_tam_chave());
    if (hf_habitantes == NULL) {
        fprintf(stderr, "Erro ao criar hashfile de habitantes\n");
        hf_fechar(hf_quadras);
        return 1;
    }

    /* Processar .geo */
    geo_processar(caminho_geo, hf_quadras);

    /* Processar .pm */
    if (arq_pm != NULL)
        pm_processar(caminho_pm, hf_habitantes);

    /* Processar .qry */
    FILE *txt = NULL;
    if (arq_qry != NULL && caminho_txt[0] != '\0') {
        txt = fopen(caminho_txt, "w");
    }

    FILE *svg_f = svg_abrir(caminho_svg, 1500, 1500);

    /* Desenhar quadras no SVG */
    hf_iterar(hf_quadras, desenhar_quadras, svg_f);

    if (arq_qry != NULL) {
        qry_processar(caminho_qry, hf_quadras, hf_habitantes, svg_f, txt);
    }

    svg_fechar(svg_f);

    if (txt != NULL) fclose(txt);

    /* Gerar dumps .hfd */
    FILE *hfd_q = fopen(caminho_hfd_q, "w");
    if (hfd_q != NULL) { hf_dump(hf_quadras, hfd_q); fclose(hfd_q); }

    FILE *hfd_h = fopen(caminho_hfd_h, "w");
    if (hfd_h != NULL) { hf_dump(hf_habitantes, hfd_h); fclose(hfd_h); }

    /* Fechar hashfiles */
    hf_fechar(hf_quadras);
    hf_fechar(hf_habitantes);

    return 0;
}
