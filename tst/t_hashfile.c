#include "unity.h"
#include "hashfile.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Caminho do arquivo de teste */
static const char *HF_TESTE = "tst_hf_temp.hf";

void setUp(void) {}

void tearDown(void) {
    remove(HF_TESTE);
}

/* ========== Testes de criação ========== */

void test_hf_criar_basico(void) {
    HashFile hf = hf_criar(HF_TESTE, 4, 32, 512, 0, 4);
    TEST_ASSERT_NOT_NULL(hf);
    TEST_ASSERT_EQUAL_INT(2, hf_num_buckets(hf));
    TEST_ASSERT_EQUAL_INT(32, hf_tamanho_registro(hf));
    hf_fechar(hf);
}

void test_hf_criar_params_invalidos(void) {
    TEST_ASSERT_NULL(hf_criar(NULL, 4, 32, 512, 0, 4));
    TEST_ASSERT_NULL(hf_criar(HF_TESTE, 4, 0, 512, 0, 4));
    TEST_ASSERT_NULL(hf_criar(HF_TESTE, 4, 32, 0, 0, 4));
}

/* ========== Testes de inserção e busca ========== */

/* Registro de teste: chave int (4 bytes) + payload de 28 bytes */
typedef struct {
    int    chave;
    char   dados[28];
} RegTeste;

void test_hf_inserir_e_buscar(void) {
    HashFile hf = hf_criar(HF_TESTE, 4, sizeof(RegTeste), 512, 0, sizeof(int));
    TEST_ASSERT_NOT_NULL(hf);

    RegTeste r1 = { .chave = 100 };
    strncpy(r1.dados, "registro_cem", sizeof(r1.dados));

    RegTeste r2 = { .chave = 200 };
    strncpy(r2.dados, "registro_duzentos", sizeof(r2.dados));

    TEST_ASSERT_TRUE(hf_inserir(hf, &r1));
    TEST_ASSERT_TRUE(hf_inserir(hf, &r2));

    /* Buscar */
    RegTeste saida;
    int chave_busca = 100;
    TEST_ASSERT_TRUE(hf_buscar(hf, &chave_busca, &saida));
    TEST_ASSERT_EQUAL_INT(100, saida.chave);
    TEST_ASSERT_EQUAL_STRING("registro_cem", saida.dados);

    chave_busca = 200;
    TEST_ASSERT_TRUE(hf_buscar(hf, &chave_busca, &saida));
    TEST_ASSERT_EQUAL_INT(200, saida.chave);

    /* Buscar chave inexistente */
    chave_busca = 999;
    TEST_ASSERT_FALSE(hf_buscar(hf, &chave_busca, &saida));

    hf_fechar(hf);
}

void test_hf_inserir_duplicada(void) {
    HashFile hf = hf_criar(HF_TESTE, 4, sizeof(RegTeste), 512, 0, sizeof(int));
    TEST_ASSERT_NOT_NULL(hf);

    RegTeste r = { .chave = 42 };
    strncpy(r.dados, "original", sizeof(r.dados));

    TEST_ASSERT_TRUE(hf_inserir(hf, &r));
    TEST_ASSERT_FALSE(hf_inserir(hf, &r));  /* duplicada */

    hf_fechar(hf);
}

/* ========== Teste de remoção ========== */

void test_hf_remover(void) {
    HashFile hf = hf_criar(HF_TESTE, 4, sizeof(RegTeste), 512, 0, sizeof(int));
    TEST_ASSERT_NOT_NULL(hf);

    RegTeste r1 = { .chave = 10 };
    RegTeste r2 = { .chave = 20 };
    RegTeste r3 = { .chave = 30 };
    strncpy(r1.dados, "dez", sizeof(r1.dados));
    strncpy(r2.dados, "vinte", sizeof(r2.dados));
    strncpy(r3.dados, "trinta", sizeof(r3.dados));

    hf_inserir(hf, &r1);
    hf_inserir(hf, &r2);
    hf_inserir(hf, &r3);

    /* Remover r2 */
    int chave = 20;
    TEST_ASSERT_TRUE(hf_remover(hf, &chave));

    /* r2 não existe mais */
    RegTeste saida;
    TEST_ASSERT_FALSE(hf_buscar(hf, &chave, &saida));

    /* r1 e r3 ainda existem */
    chave = 10;
    TEST_ASSERT_TRUE(hf_buscar(hf, &chave, &saida));
    TEST_ASSERT_EQUAL_INT(10, saida.chave);

    chave = 30;
    TEST_ASSERT_TRUE(hf_buscar(hf, &chave, &saida));
    TEST_ASSERT_EQUAL_INT(30, saida.chave);

    /* Remover inexistente */
    chave = 999;
    TEST_ASSERT_FALSE(hf_remover(hf, &chave));

    hf_fechar(hf);
}

/* ========== Teste de persistência (fechar e reabrir) ========== */

void test_hf_persistencia(void) {
    /* Criar e inserir */
    HashFile hf = hf_criar(HF_TESTE, 4, sizeof(RegTeste), 512, 0, sizeof(int));
    TEST_ASSERT_NOT_NULL(hf);

    RegTeste r = { .chave = 777 };
    strncpy(r.dados, "persistente", sizeof(r.dados));
    TEST_ASSERT_TRUE(hf_inserir(hf, &r));
    hf_fechar(hf);

    /* Reabrir e verificar */
    hf = hf_abrir(HF_TESTE);
    TEST_ASSERT_NOT_NULL(hf);
    TEST_ASSERT_EQUAL_INT(2, hf_num_buckets(hf));

    RegTeste saida;
    int chave = 777;
    TEST_ASSERT_TRUE(hf_buscar(hf, &chave, &saida));
    TEST_ASSERT_EQUAL_INT(777, saida.chave);
    TEST_ASSERT_EQUAL_STRING("persistente", saida.dados);

    hf_fechar(hf);
}

/* ========== Teste com chave string ========== */

typedef struct {
    char   cep[20];
    double x, y, w, h;
} RegQuadra;

void test_hf_chave_string(void) {
    HashFile hf = hf_criar(HF_TESTE, 8, sizeof(RegQuadra),
                           1024, 0, 20);  /* chave = cep (20 bytes) */
    TEST_ASSERT_NOT_NULL(hf);

    RegQuadra q1;
    memset(&q1, 0, sizeof(q1));
    strncpy(q1.cep, "cep01", sizeof(q1.cep));
    q1.x = 10.0; q1.y = 20.0; q1.w = 30.0; q1.h = 40.0;

    RegQuadra q2;
    memset(&q2, 0, sizeof(q2));
    strncpy(q2.cep, "cep02", sizeof(q2.cep));
    q2.x = 50.0; q2.y = 60.0; q2.w = 70.0; q2.h = 80.0;

    TEST_ASSERT_TRUE(hf_inserir(hf, &q1));
    TEST_ASSERT_TRUE(hf_inserir(hf, &q2));

    /* Buscar por CEP */
    char chave[20];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "cep01", sizeof(chave));

    RegQuadra saida;
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, &saida));
    TEST_ASSERT_EQUAL_STRING("cep01", saida.cep);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, saida.x);

    /* CEP inexistente */
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "cep99", sizeof(chave));
    TEST_ASSERT_FALSE(hf_buscar(hf, chave, &saida));

    hf_fechar(hf);
}

/* ========== Teste de dump ========== */

void test_hf_dump(void) {
    HashFile hf = hf_criar(HF_TESTE, 2, sizeof(RegTeste), 512, 0, sizeof(int));
    TEST_ASSERT_NOT_NULL(hf);

    RegTeste r = { .chave = 55 };
    strncpy(r.dados, "teste_dump", sizeof(r.dados));
    hf_inserir(hf, &r);

    /* Dump para arquivo temporário */
    FILE *fdump = fopen("tst_hf_temp.hfd", "w");
    TEST_ASSERT_NOT_NULL(fdump);
    hf_dump(hf, fdump);
    fclose(fdump);

    /* Verificar que o dump não está vazio */
    fdump = fopen("tst_hf_temp.hfd", "r");
    TEST_ASSERT_NOT_NULL(fdump);
    char linha[256];
    TEST_ASSERT_NOT_NULL(fgets(linha, sizeof(linha), fdump));
    TEST_ASSERT_EQUAL_STRING("DUMP\n", linha);
    fclose(fdump);

    remove("tst_hf_temp.hfd");
    hf_fechar(hf);
}

/* ========== Teste de iteração ========== */

static void contar_callback(const void *registro, void *ctx) {
    (void)registro;
    int *contador = (int *)ctx;
    (*contador)++;
}

void test_hf_iterar(void) {
    HashFile hf = hf_criar(HF_TESTE, 4, sizeof(RegTeste), 512, 0, sizeof(int));
    TEST_ASSERT_NOT_NULL(hf);

    for (int i = 0; i < 10; i++) {
        RegTeste r;
        r.chave = i * 100;
        memset(r.dados, 0, sizeof(r.dados));
        hf_inserir(hf, &r);
    }

    int contador = 0;
    hf_iterar(hf, contar_callback, &contador);
    TEST_ASSERT_EQUAL_INT(10, contador);

    hf_fechar(hf);
}

/* ========== Teste NULL safety ========== */

void test_hf_null_safety(void) {
    hf_fechar(NULL);
    TEST_ASSERT_FALSE(hf_inserir(NULL, NULL));
    TEST_ASSERT_FALSE(hf_buscar(NULL, NULL, NULL));
    TEST_ASSERT_FALSE(hf_remover(NULL, NULL));
    hf_dump(NULL, NULL);
    hf_iterar(NULL, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(0, hf_num_buckets(NULL));
    TEST_ASSERT_EQUAL_INT(0, hf_tamanho_registro(NULL));
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hf_criar_basico);
    RUN_TEST(test_hf_criar_params_invalidos);
    RUN_TEST(test_hf_inserir_e_buscar);
    RUN_TEST(test_hf_inserir_duplicada);
    RUN_TEST(test_hf_remover);
    RUN_TEST(test_hf_persistencia);
    RUN_TEST(test_hf_chave_string);
    RUN_TEST(test_hf_dump);
    RUN_TEST(test_hf_iterar);
    RUN_TEST(test_hf_null_safety);
    return UNITY_END();
}
