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



static void extrair_nome_sem_ext(const char *arquivo, char *saida, int tam) {
    const char *base = strrchr(arquivo, '/');
    if (base == NULL) base = strrchr(arquivo, '\\');
    base = base ? base + 1 : arquivo;

    strncpy(saida, base, (size_t)(tam - 1));
    saida[tam - 1] = '\0';

    char *ponto = strrchr(saida, '.');
    if (ponto != NULL) *ponto = '\0';
}

typedef struct {
    double max_x;
    double max_y;
} BBox;

static void calcular_bbox(const void *registro, void *ctx) {
    BBox *bb = (BBox *)ctx;
    Quadra q = quadra_desserializar(registro);
    if (q == NULL) return;
    double xf = quadra_x(q) + quadra_w(q);
    double yf = quadra_y(q) + quadra_h(q);
    if (xf > bb->max_x) bb->max_x = xf;
    if (yf > bb->max_y) bb->max_y = yf;
    quadra_destruir(q);
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

static void gerar_svg_quadras(const char *caminho, double largura,
                              double altura, HashFile hf_quadras) {
    FILE *svg_f = svg_abrir(caminho, largura, altura);
    if (svg_f == NULL) return;

    hf_iterar(hf_quadras, desenhar_quadras, svg_f);
    svg_fechar(svg_f);
}

int main(int argc, char *argv[]) {
    char *dir_entrada = NULL;
    char *arq_geo     = NULL;
    char *arq_pm      = NULL;
    char *arq_qry     = NULL;
    char *dir_saida   = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0) {
            if (i + 1 < argc && argv[i+1][0] != '-') dir_entrada = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc && argv[i+1][0] != '-') arq_geo = argv[++i];
        } else if (strcmp(argv[i], "-pm") == 0) {
            if (i + 1 < argc && argv[i+1][0] != '-') arq_pm = argv[++i];
        } else if (strcmp(argv[i], "-q") == 0) {
            if (i + 1 < argc && argv[i+1][0] != '-') arq_qry = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc && argv[i+1][0] != '-') dir_saida = argv[++i];
        }
    }

    if (arq_geo == NULL || dir_saida == NULL) {
        fprintf(stderr, "Uso: ted [-e <dir_entrada>] -f <arq.geo> "
                        "[-pm <arq.pm>] [-q <arq.qry>] -o <dir_saida>\n");
        return 1;
    }

    if (dir_entrada == NULL) {
        dir_entrada = ".";
    }

    
    char caminho_geo[512], caminho_pm[512], caminho_qry[512];
    snprintf(caminho_geo, sizeof(caminho_geo), "%s/%s", dir_entrada, arq_geo);

    if (arq_pm != NULL)
        snprintf(caminho_pm, sizeof(caminho_pm), "%s/%s", dir_entrada, arq_pm);
    if (arq_qry != NULL)
        snprintf(caminho_qry, sizeof(caminho_qry), "%s/%s", dir_entrada, arq_qry);

    
    char nome_geo[128], nome_qry[128];
    extrair_nome_sem_ext(arq_geo, nome_geo, sizeof(nome_geo));
    if (arq_qry != NULL)
        extrair_nome_sem_ext(arq_qry, nome_qry, sizeof(nome_qry));

    
    char caminho_hf_q[512], caminho_hf_h[512];
    char caminho_hfd_q[512], caminho_hfd_h[512];
    char caminho_svg_geo[512], caminho_svg_qry[512], caminho_txt[512];

    snprintf(caminho_hf_q, sizeof(caminho_hf_q), "%s/%s-quadras.hf", dir_saida, nome_geo);
    snprintf(caminho_hf_h, sizeof(caminho_hf_h), "%s/%s-habitantes.hf", dir_saida, nome_geo);
    snprintf(caminho_hfd_q, sizeof(caminho_hfd_q), "%s/%s-quadras.hfd", dir_saida, nome_geo);
    snprintf(caminho_hfd_h, sizeof(caminho_hfd_h), "%s/%s-habitantes.hfd", dir_saida, nome_geo);
    snprintf(caminho_svg_geo, sizeof(caminho_svg_geo), "%s/%s.svg", dir_saida, nome_geo);

    if (arq_qry != NULL) {
        snprintf(caminho_svg_qry, sizeof(caminho_svg_qry), "%s/%s-%s.svg", dir_saida, nome_geo, nome_qry);
        snprintf(caminho_txt, sizeof(caminho_txt), "%s/%s-%s.txt", dir_saida, nome_geo, nome_qry);
    } else {
        caminho_svg_qry[0] = '\0';
        caminho_txt[0] = '\0';
    }

    
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

    
    geo_processar(caminho_geo, hf_quadras);

    
    if (arq_pm != NULL)
        pm_processar(caminho_pm, hf_habitantes);

    FILE *txt = NULL;
    if (arq_qry != NULL && caminho_txt[0] != '\0') {
        txt = fopen(caminho_txt, "w");
    }

    BBox bb = {0.0, 0.0};
    hf_iterar(hf_quadras, calcular_bbox, &bb);
    if (bb.max_x < 100) bb.max_x = 1500;
    if (bb.max_y < 100) bb.max_y = 1500;
    bb.max_x += 50;
    bb.max_y += 50;

    gerar_svg_quadras(caminho_svg_geo, bb.max_x, bb.max_y, hf_quadras);

    char caminho_svg_tmp[520];
    snprintf(caminho_svg_tmp, sizeof(caminho_svg_tmp), "%s.tmp", caminho_svg_qry);
    FILE *svg_tmp = NULL;

    if (arq_qry != NULL) {
        svg_tmp = fopen(caminho_svg_tmp, "w");
        qry_processar(caminho_qry, hf_quadras, hf_habitantes, svg_tmp, txt);
        if (svg_tmp != NULL) fclose(svg_tmp);

        FILE *svg_f = svg_abrir(caminho_svg_qry, bb.max_x, bb.max_y);
        hf_iterar(hf_quadras, desenhar_quadras, svg_f);

        svg_tmp = fopen(caminho_svg_tmp, "r");
        if (svg_tmp != NULL) {
            char buf_tmp[1024];
            while (fgets(buf_tmp, sizeof(buf_tmp), svg_tmp) != NULL) {
                fputs(buf_tmp, svg_f);
            }
            fclose(svg_tmp);
            remove(caminho_svg_tmp);
        }

        svg_fechar(svg_f);
    }

    if (txt != NULL) fclose(txt);

    
    FILE *hfd_q = fopen(caminho_hfd_q, "w");
    if (hfd_q != NULL) { hf_dump(hf_quadras, hfd_q); fclose(hfd_q); }

    FILE *hfd_h = fopen(caminho_hfd_h, "w");
    if (hfd_h != NULL) { hf_dump(hf_habitantes, hfd_h); fclose(hfd_h); }

    
    hf_fechar(hf_quadras);
    hf_fechar(hf_habitantes);

    return 0;
}
