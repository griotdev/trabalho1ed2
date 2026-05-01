#include "unity.h"
#include "geo.h"
#include "quadra.h"
#include "hashfile.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *GEO_TESTE   = "tst_geo_temp.geo";
static const char *HF_TESTE    = "tst_geo_temp.hf";

void setUp(void) {}

void tearDown(void) {
    remove(GEO_TESTE);
    remove(HF_TESTE);
}


static void criar_geo(const char *conteudo) {
    FILE *f = fopen(GEO_TESTE, "w");
    fprintf(f, "%s", conteudo);
    fclose(f);
}



void test_geo_quadra_simples(void) {
    criar_geo(
        "q cep01 10.0 20.0 30.0 40.0\n"
        "q cep02 50.0 60.0 70.0 80.0\n"
    );

    int tam_reg = quadra_sizeof_registro();
    HashFile hf = hf_criar(HF_TESTE, 8, tam_reg, 2048,
                           quadra_offset_chave(), quadra_tam_chave());
    TEST_ASSERT_NOT_NULL(hf);

    geo_processar(GEO_TESTE, hf);

    
    char chave[20];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "cep01", sizeof(chave));

    char buf[256];
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, buf));

    Quadra q = quadra_desserializar(buf);
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("cep01", quadra_cep(q));
    TEST_ASSERT_EQUAL_DOUBLE(10.0, quadra_x(q));
    TEST_ASSERT_EQUAL_DOUBLE(20.0, quadra_y(q));
    TEST_ASSERT_EQUAL_DOUBLE(30.0, quadra_w(q));
    TEST_ASSERT_EQUAL_DOUBLE(40.0, quadra_h(q));
    quadra_destruir(q);

    
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "cep02", sizeof(chave));
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, buf));

    q = quadra_desserializar(buf);
    TEST_ASSERT_EQUAL_STRING("cep02", quadra_cep(q));
    TEST_ASSERT_EQUAL_DOUBLE(50.0, quadra_x(q));
    quadra_destruir(q);

    hf_fechar(hf);
}

void test_geo_com_estilo(void) {
    criar_geo(
        "cq 2.5 red blue\n"
        "q cep10 1.0 2.0 3.0 4.0\n"
        "cq 1.0 green yellow\n"
        "q cep20 5.0 6.0 7.0 8.0\n"
    );

    int tam_reg = quadra_sizeof_registro();
    HashFile hf = hf_criar(HF_TESTE, 8, tam_reg, 2048,
                           quadra_offset_chave(), quadra_tam_chave());
    geo_processar(GEO_TESTE, hf);

    
    char chave[20], buf[256];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "cep10", sizeof(chave));
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, buf));

    Quadra q = quadra_desserializar(buf);
    TEST_ASSERT_EQUAL_STRING("red", quadra_cfill(q));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_cstrk(q));
    TEST_ASSERT_EQUAL_DOUBLE(2.5, quadra_sw(q));
    quadra_destruir(q);

    
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "cep20", sizeof(chave));
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, buf));

    q = quadra_desserializar(buf);
    TEST_ASSERT_EQUAL_STRING("green", quadra_cfill(q));
    TEST_ASSERT_EQUAL_STRING("yellow", quadra_cstrk(q));
    quadra_destruir(q);

    hf_fechar(hf);
}

void test_geo_arquivo_vazio(void) {
    criar_geo("");

    int tam_reg = quadra_sizeof_registro();
    HashFile hf = hf_criar(HF_TESTE, 4, tam_reg, 2048,
                           quadra_offset_chave(), quadra_tam_chave());
    geo_processar(GEO_TESTE, hf);

    
    char chave[20], buf[256];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "cep01", sizeof(chave));
    TEST_ASSERT_FALSE(hf_buscar(hf, chave, buf));

    hf_fechar(hf);
}

void test_geo_comentarios_ignorados(void) {
    criar_geo(
        "# Isso e um comentario\n"
        "q cep01 1.0 2.0 3.0 4.0\n"
        "# Outro comentario\n"
    );

    int tam_reg = quadra_sizeof_registro();
    HashFile hf = hf_criar(HF_TESTE, 4, tam_reg, 2048,
                           quadra_offset_chave(), quadra_tam_chave());
    geo_processar(GEO_TESTE, hf);

    char chave[20], buf[256];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "cep01", sizeof(chave));
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, buf));

    hf_fechar(hf);
}

void test_geo_null_params(void) {
    geo_processar(NULL, NULL);
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_geo_quadra_simples);
    RUN_TEST(test_geo_com_estilo);
    RUN_TEST(test_geo_arquivo_vazio);
    RUN_TEST(test_geo_comentarios_ignorados);
    RUN_TEST(test_geo_null_params);
    return UNITY_END();
}
