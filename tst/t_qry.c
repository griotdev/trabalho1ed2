#include "unity.h"
#include "qry.h"
#include "geo.h"
#include "pm.h"
#include "quadra.h"
#include "habitante.h"
#include "hashfile.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *GEO_TESTE = "tst_qry_temp.geo";
static const char *PM_TESTE  = "tst_qry_temp.pm";
static const char *QRY_TESTE = "tst_qry_temp.qry";
static const char *HF_Q_TESTE = "tst_qry_q_temp.hf";
static const char *HF_H_TESTE = "tst_qry_h_temp.hf";
static const char *SVG_TESTE = "tst_qry_temp.svg";
static const char *TXT_TESTE = "tst_qry_temp.txt";

void setUp(void) {}

void tearDown(void) {
    remove(GEO_TESTE);
    remove(PM_TESTE);
    remove(QRY_TESTE);
    remove(HF_Q_TESTE);
    remove(HF_H_TESTE);
    remove(SVG_TESTE);
    remove(TXT_TESTE);
}

static void criar_arquivo(const char *caminho, const char *conteudo) {
    FILE *f = fopen(caminho, "w");
    if (f) {
        fprintf(f, "%s", conteudo);
        fclose(f);
    }
}

void test_qry_comandos(void) {
    criar_arquivo(GEO_TESTE,
        "q cep01 10.0 10.0 50.0 50.0\n"
        "q cep02 100.0 100.0 50.0 50.0\n"
    );
    criar_arquivo(PM_TESTE,
        "p 111 Joao Silva M 01/01/2000\n"
        "p 222 Maria Santos F 02/02/2000\n"
        "m 111 cep01 S 10 casa\n"
        "m 222 cep02 N 20 apto\n"
    );
    criar_arquivo(QRY_TESTE,
        "censo\n"
        "h? 111\n"
        "pq cep01\n"
        "nasc 333 Bebe Silva M 03/03/2020\n"
        "mud 222 cep01 L 15 sobrado\n"
        "rip 111\n"
        "dspj 222\n"
        "rq cep02\n"
    );

    int tam_q = quadra_sizeof_registro();
    int tam_h = habitante_sizeof_registro();

    HashFile hf_q = hf_criar(HF_Q_TESTE, 8, tam_q, 4096, quadra_offset_chave(), quadra_tam_chave());
    HashFile hf_h = hf_criar(HF_H_TESTE, 8, tam_h, 4096, habitante_offset_chave(), habitante_tam_chave());

    geo_processar(GEO_TESTE, hf_q);
    pm_processar(PM_TESTE, hf_h);

    FILE *svg = fopen(SVG_TESTE, "w");
    FILE *txt = fopen(TXT_TESTE, "w");

    qry_processar(QRY_TESTE, hf_q, hf_h, svg, txt);

    fclose(svg);
    fclose(txt);

    /* Verificacoes basicas do TXT gerado */
    FILE *f = fopen(TXT_TESTE, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[1024];
    int leu_censo = 0;
    while (fgets(buf, sizeof(buf), f)) {
        if (strstr(buf, "[*] censo")) leu_censo = 1;
    }
    fclose(f);
    TEST_ASSERT_TRUE(leu_censo);

    hf_fechar(hf_q);
    hf_fechar(hf_h);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_qry_comandos);
    return UNITY_END();
}
