#include "qry.h"
#include "quadra.h"
#include "habitante.h"
#include "svg.h"
#include "geo_calc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *cep;
    HashFile    hf_hab;
    FILE       *txt;
    int         tam_reg_hab;
} CtxRQ;

static void rq_despejar_moradores(const void *registro, void *ctx) {
    CtxRQ *c = (CtxRQ *)ctx;
    Habitante h = habitante_desserializar(registro);
    if (h == NULL) return;

    if (habitante_e_morador(h) && strcmp(habitante_cep(h), c->cep) == 0) {
        fprintf(c->txt, "%s %s %s\n", habitante_cpf(h),
                habitante_nome(h), habitante_sobrenome(h));

        habitante_remover_endereco(h);
        char *buf = (char *)malloc((size_t)c->tam_reg_hab);
        if (buf != NULL) {
            char chave[15];
            memset(chave, 0, sizeof(chave));
            strncpy(chave, habitante_cpf(h), 14);

            habitante_serializar(h, buf, c->tam_reg_hab);
            hf_remover(c->hf_hab, chave);
            hf_inserir(c->hf_hab, buf);
            free(buf);
        }
    }
    habitante_destruir(h);
}

typedef struct {
    const char *cep;
    int faces[4];
    int total;
} CtxPQ;

static void pq_contar(const void *registro, void *ctx) {
    CtxPQ *c = (CtxPQ *)ctx;
    Habitante h = habitante_desserializar(registro);
    if (h == NULL) return;

    if (habitante_e_morador(h) && strcmp(habitante_cep(h), c->cep) == 0) {
        const char *face = habitante_face(h);
        if (face[0] == 'N') c->faces[0]++;
        else if (face[0] == 'S') c->faces[1]++;
        else if (face[0] == 'L') c->faces[2]++;
        else if (face[0] == 'O') c->faces[3]++;
        c->total++;
    }
    habitante_destruir(h);
}

typedef struct {
    int total_hab;
    int total_mor;
    int masc;
    int fem;
} CtxCenso;

static void censo_contar(const void *registro, void *ctx) {
    CtxCenso *c = (CtxCenso *)ctx;
    Habitante h = habitante_desserializar(registro);
    if (h == NULL) return;

    c->total_hab++;
    if (habitante_e_morador(h)) c->total_mor++;
    if (habitante_sexo(h) == 'M') c->masc++;
    else if (habitante_sexo(h) == 'F') c->fem++;
    habitante_destruir(h);
}

static void cmd_rq(const char *cep, HashFile hf_quadras, HashFile hf_hab,
                   FILE *svg_f, FILE *txt) {
    int tam_reg_q = quadra_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg_q);
    if (buf == NULL) return;

    char chave[20];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, cep, 19);

    if (hf_buscar(hf_quadras, chave, buf)) {
        Quadra q = quadra_desserializar(buf);
        if (q != NULL) {
            double ax = quadra_x(q);
            double ay = quadra_y(q);

            CtxRQ ctx;
            ctx.cep = cep;
            ctx.hf_hab = hf_hab;
            ctx.txt = txt;
            ctx.tam_reg_hab = habitante_sizeof_registro();
            hf_iterar(hf_hab, rq_despejar_moradores, &ctx);

            svg_linha(svg_f, ax - 5, ay - 5, ax + 5, ay + 5, "red", 2.0);
            svg_linha(svg_f, ax - 5, ay + 5, ax + 5, ay - 5, "red", 2.0);

            hf_remover(hf_quadras, chave);
            quadra_destruir(q);
        }
    }
    free(buf);
}

static void cmd_pq(const char *cep, HashFile hf_quadras, HashFile hf_hab,
                   FILE *svg_f) {
    int tam_reg_q = quadra_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg_q);
    if (buf == NULL) return;

    char chave[20];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, cep, 19);

    if (hf_buscar(hf_quadras, chave, buf)) {
        Quadra q = quadra_desserializar(buf);
        if (q != NULL) {
            CtxPQ ctx;
            ctx.cep = cep;
            memset(ctx.faces, 0, sizeof(ctx.faces));
            ctx.total = 0;
            hf_iterar(hf_hab, pq_contar, &ctx);

            double x = quadra_x(q), y = quadra_y(q);
            double w = quadra_w(q), h = quadra_h(q);
            char num[16];

            sprintf(num, "%d", ctx.faces[0]);
            svg_texto(svg_f, x + w / 2, y + h + 12, num, "black", 10);

            sprintf(num, "%d", ctx.faces[1]);
            svg_texto(svg_f, x + w / 2, y - 4, num, "black", 10);

            sprintf(num, "%d", ctx.faces[2]);
            svg_texto(svg_f, x - 12, y + h / 2, num, "black", 10);

            sprintf(num, "%d", ctx.faces[3]);
            svg_texto(svg_f, x + w + 4, y + h / 2, num, "black", 10);

            sprintf(num, "%d", ctx.total);
            svg_texto(svg_f, x + w / 2 - 4, y + h / 2, num, "blue", 12);

            quadra_destruir(q);
        }
    }
    free(buf);
}

static void cmd_censo(HashFile hf_hab, FILE *txt) {
    CtxCenso ctx;
    memset(&ctx, 0, sizeof(ctx));
    hf_iterar(hf_hab, censo_contar, &ctx);

    int sem_teto = ctx.total_hab - ctx.total_mor;
    fprintf(txt, "Habitantes: %d\n", ctx.total_hab);
    fprintf(txt, "Moradores: %d (%.2f%%)\n",
            ctx.total_mor,
            ctx.total_hab > 0 ? 100.0 * ctx.total_mor / ctx.total_hab : 0.0);
    fprintf(txt, "Sexo M: %d (%.2f%%)\n",
            ctx.masc,
            ctx.total_hab > 0 ? 100.0 * ctx.masc / ctx.total_hab : 0.0);
    fprintf(txt, "Sexo F: %d (%.2f%%)\n",
            ctx.fem,
            ctx.total_hab > 0 ? 100.0 * ctx.fem / ctx.total_hab : 0.0);
    fprintf(txt, "Sem-teto: %d (%.2f%%)\n",
            sem_teto,
            ctx.total_hab > 0 ? 100.0 * sem_teto / ctx.total_hab : 0.0);
}

static void cmd_hq(const char *cpf, HashFile hf_hab, FILE *txt) {
    int tam_reg = habitante_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg);
    if (buf == NULL) return;

    char chave[15];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, cpf, 14);

    if (hf_buscar(hf_hab, chave, buf)) {
        Habitante h = habitante_desserializar(buf);
        if (h != NULL) {
            fprintf(txt, "CPF: %s\nNome: %s %s\nSexo: %c\nNascimento: %s\n",
                    habitante_cpf(h), habitante_nome(h),
                    habitante_sobrenome(h), habitante_sexo(h),
                    habitante_nasc(h));
            if (habitante_e_morador(h)) {
                fprintf(txt, "Endereco: %s/%s/%d %s\n",
                        habitante_cep(h), habitante_face(h),
                        habitante_num(h), habitante_compl(h));
            } else {
                fprintf(txt, "Sem endereco\n");
            }
            habitante_destruir(h);
        }
    }
    free(buf);
}

static void cmd_nasc(const char *cpf, const char *nome, const char *sobrenome,
                     char sexo, const char *nasc, HashFile hf_hab) {
    Habitante h = habitante_criar(cpf, nome, sobrenome, sexo, nasc);
    if (h == NULL) return;

    int tam_reg = habitante_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg);
    if (buf != NULL) {
        memset(buf, 0, (size_t)tam_reg);
        habitante_serializar(h, buf, tam_reg);
        hf_inserir(hf_hab, buf);
        free(buf);
    }
    habitante_destruir(h);
}

static void cmd_rip(const char *cpf, HashFile hf_quadras, HashFile hf_hab,
                    FILE *svg_f, FILE *txt) {
    int tam_reg = habitante_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg);
    if (buf == NULL) return;

    char chave[15];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, cpf, 14);

    if (hf_buscar(hf_hab, chave, buf)) {
        Habitante h = habitante_desserializar(buf);
        if (h != NULL) {
            fprintf(txt, "CPF: %s\nNome: %s %s\nSexo: %c\nNascimento: %s\n",
                    habitante_cpf(h), habitante_nome(h),
                    habitante_sobrenome(h), habitante_sexo(h),
                    habitante_nasc(h));

            if (habitante_e_morador(h)) {
                fprintf(txt, "Endereco: %s/%s/%d %s\n",
                        habitante_cep(h), habitante_face(h),
                        habitante_num(h), habitante_compl(h));

                int tam_q = quadra_sizeof_registro();
                char *bufq = (char *)malloc((size_t)tam_q);
                if (bufq != NULL) {
                    char cep_chave[20];
                    memset(cep_chave, 0, sizeof(cep_chave));
                    strncpy(cep_chave, habitante_cep(h), 19);

                    if (hf_buscar(hf_quadras, cep_chave, bufq)) {
                        Quadra q = quadra_desserializar(bufq);
                        if (q != NULL) {
                            double rx, ry;
                            geo_calc_endereco(quadra_x(q), quadra_y(q),
                                              quadra_w(q), quadra_h(q),
                                              habitante_face(h)[0],
                                              habitante_num(h), &rx, &ry);
                            svg_linha(svg_f, rx, ry - 8, rx, ry + 8, "red", 2.0);
                            svg_linha(svg_f, rx - 8, ry, rx + 8, ry, "red", 2.0);
                            quadra_destruir(q);
                        }
                    }
                    free(bufq);
                }
            }
            habitante_destruir(h);
        }
        hf_remover(hf_hab, chave);
    }
    free(buf);
}

static void cmd_mud(const char *cpf, const char *cep, const char *face,
                    int num, const char *compl_,
                    HashFile hf_quadras, HashFile hf_hab, FILE *svg_f) {
    int tam_reg = habitante_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg);
    if (buf == NULL) return;

    char chave[15];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, cpf, 14);

    if (hf_buscar(hf_hab, chave, buf)) {
        Habitante h = habitante_desserializar(buf);
        if (h != NULL) {
            habitante_set_endereco(h, cep, face, num, compl_);
            memset(buf, 0, (size_t)tam_reg);
            habitante_serializar(h, buf, tam_reg);
            hf_remover(hf_hab, chave);
            hf_inserir(hf_hab, buf);

            int tam_q = quadra_sizeof_registro();
            char *bufq = (char *)malloc((size_t)tam_q);
            if (bufq != NULL) {
                char cep_chave[20];
                memset(cep_chave, 0, sizeof(cep_chave));
                strncpy(cep_chave, cep, 19);

                if (hf_buscar(hf_quadras, cep_chave, bufq)) {
                    Quadra q = quadra_desserializar(bufq);
                    if (q != NULL) {
                        double rx, ry;
                        geo_calc_endereco(quadra_x(q), quadra_y(q),
                                          quadra_w(q), quadra_h(q),
                                          face[0], num, &rx, &ry);
                        svg_retangulo(svg_f, rx - 5, ry - 5, 10, 10,
                                      "red", "red", 1.0);
                        svg_texto(svg_f, rx - 4, ry + 3, cpf, "white", 4);
                        quadra_destruir(q);
                    }
                }
                free(bufq);
            }
            habitante_destruir(h);
        }
    }
    free(buf);
}

static void cmd_dspj(const char *cpf, HashFile hf_quadras, HashFile hf_hab,
                     FILE *svg_f, FILE *txt) {
    int tam_reg = habitante_sizeof_registro();
    char *buf = (char *)malloc((size_t)tam_reg);
    if (buf == NULL) return;

    char chave[15];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, cpf, 14);

    if (hf_buscar(hf_hab, chave, buf)) {
        Habitante h = habitante_desserializar(buf);
        if (h != NULL) {
            fprintf(txt, "CPF: %s\nNome: %s %s\n",
                    habitante_cpf(h), habitante_nome(h),
                    habitante_sobrenome(h));

            if (habitante_e_morador(h)) {
                fprintf(txt, "Endereco anterior: %s/%s/%d %s\n",
                        habitante_cep(h), habitante_face(h),
                        habitante_num(h), habitante_compl(h));

                int tam_q = quadra_sizeof_registro();
                char *bufq = (char *)malloc((size_t)tam_q);
                if (bufq != NULL) {
                    char cep_chave[20];
                    memset(cep_chave, 0, sizeof(cep_chave));
                    strncpy(cep_chave, habitante_cep(h), 19);

                    if (hf_buscar(hf_quadras, cep_chave, bufq)) {
                        Quadra q = quadra_desserializar(bufq);
                        if (q != NULL) {
                            double rx, ry;
                            geo_calc_endereco(quadra_x(q), quadra_y(q),
                                              quadra_w(q), quadra_h(q),
                                              habitante_face(h)[0],
                                              habitante_num(h), &rx, &ry);
                            svg_circulo(svg_f, rx, ry, 5, "black", "black", 1.0);
                            quadra_destruir(q);
                        }
                    }
                    free(bufq);
                }

                habitante_remover_endereco(h);
                memset(buf, 0, (size_t)tam_reg);
                habitante_serializar(h, buf, tam_reg);
                hf_remover(hf_hab, chave);
                hf_inserir(hf_hab, buf);
            }
            habitante_destruir(h);
        }
    }
    free(buf);
}

void qry_processar(const char *caminho,
                   HashFile hf_quadras, HashFile hf_habitantes,
                   FILE *svg_f, FILE *txt) {
    if (caminho == NULL || hf_quadras == NULL || hf_habitantes == NULL) return;

    FILE *fp = fopen(caminho, "r");
    if (fp == NULL) return;

    char linha[512];

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        char cmd[16];
        if (sscanf(linha, "%15s", cmd) != 1) continue;
        if (cmd[0] == '#') continue;

        if (txt != NULL) fprintf(txt, "[*] %s", linha);

        if (strcmp(cmd, "rq") == 0) {
            char cep[20];
            if (sscanf(linha, "rq %19s", cep) == 1)
                cmd_rq(cep, hf_quadras, hf_habitantes, svg_f, txt);

        } else if (strcmp(cmd, "pq") == 0) {
            char cep[20];
            if (sscanf(linha, "pq %19s", cep) == 1)
                cmd_pq(cep, hf_quadras, hf_habitantes, svg_f);

        } else if (strcmp(cmd, "censo") == 0) {
            cmd_censo(hf_habitantes, txt);

        } else if (strcmp(cmd, "h?") == 0) {
            char cpf[15];
            if (sscanf(linha, "h? %14s", cpf) == 1)
                cmd_hq(cpf, hf_habitantes, txt);

        } else if (strcmp(cmd, "nasc") == 0) {
            char cpf[15], nome[50], sobrenome[50], sexo_str[4], nasc[11];
            if (sscanf(linha, "nasc %14s %49s %49s %3s %10s",
                       cpf, nome, sobrenome, sexo_str, nasc) == 5)
                cmd_nasc(cpf, nome, sobrenome, sexo_str[0], nasc, hf_habitantes);

        } else if (strcmp(cmd, "rip") == 0) {
            char cpf[15];
            if (sscanf(linha, "rip %14s", cpf) == 1)
                cmd_rip(cpf, hf_quadras, hf_habitantes, svg_f, txt);

        } else if (strcmp(cmd, "mud") == 0) {
            char cpf[15], cep[20], face[4], compl[20];
            int num;
            if (sscanf(linha, "mud %14s %19s %3s %d %19s",
                       cpf, cep, face, &num, compl) == 5)
                cmd_mud(cpf, cep, face, num, compl,
                        hf_quadras, hf_habitantes, svg_f);

        } else if (strcmp(cmd, "dspj") == 0) {
            char cpf[15];
            if (sscanf(linha, "dspj %14s", cpf) == 1)
                cmd_dspj(cpf, hf_quadras, hf_habitantes, svg_f, txt);
        }
    }

    fclose(fp);
}
