#include "unity.h"
#include "pm.h"
#include "habitante.h"
#include "hashfile.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *PM_TESTE = "tst_pm_temp.pm";
static const char *HF_TESTE = "tst_pm_temp.hf";

void setUp(void) {}

void tearDown(void) {
    remove(PM_TESTE);
    remove(HF_TESTE);
}

static void criar_pm(const char *conteudo) {
    FILE *f = fopen(PM_TESTE, "w");
    fprintf(f, "%s", conteudo);
    fclose(f);
}

void test_pm_inserir_habitante(void) {
    criar_pm(
        "p 12345678901 Joao Silva M 01/01/2000\n"
        "p 99988877766 Maria Santos F 15/06/1990\n"
    );

    int tam_reg = habitante_sizeof_registro();
    HashFile hf = hf_criar(HF_TESTE, 8, tam_reg, 4096,
                           habitante_offset_chave(), habitante_tam_chave());
    TEST_ASSERT_NOT_NULL(hf);

    pm_processar(PM_TESTE, hf);

    char chave[15], buf[512];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "12345678901", sizeof(chave));
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, buf));

    Habitante h = habitante_desserializar(buf);
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_STRING("12345678901", habitante_cpf(h));
    TEST_ASSERT_EQUAL_STRING("Joao", habitante_nome(h));
    TEST_ASSERT_EQUAL_STRING("Silva", habitante_sobrenome(h));
    TEST_ASSERT_EQUAL_CHAR('M', habitante_sexo(h));
    TEST_ASSERT_EQUAL_INT(0, habitante_e_morador(h));
    habitante_destruir(h);

    memset(chave, 0, sizeof(chave));
    strncpy(chave, "99988877766", sizeof(chave));
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, buf));

    h = habitante_desserializar(buf);
    TEST_ASSERT_EQUAL_STRING("Maria", habitante_nome(h));
    TEST_ASSERT_EQUAL_CHAR('F', habitante_sexo(h));
    habitante_destruir(h);

    hf_fechar(hf);
}

void test_pm_tornar_morador(void) {
    criar_pm(
        "p 11122233344 Pedro Costa M 20/03/1985\n"
        "m 11122233344 cep15 S 45 apto3\n"
    );

    int tam_reg = habitante_sizeof_registro();
    HashFile hf = hf_criar(HF_TESTE, 8, tam_reg, 4096,
                           habitante_offset_chave(), habitante_tam_chave());
    pm_processar(PM_TESTE, hf);

    char chave[15], buf[512];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "11122233344", sizeof(chave));
    TEST_ASSERT_TRUE(hf_buscar(hf, chave, buf));

    Habitante h = habitante_desserializar(buf);
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_STRING("Pedro", habitante_nome(h));
    TEST_ASSERT_EQUAL_INT(1, habitante_e_morador(h));
    TEST_ASSERT_EQUAL_STRING("cep15", habitante_cep(h));
    TEST_ASSERT_EQUAL_STRING("S", habitante_face(h));
    TEST_ASSERT_EQUAL_INT(45, habitante_num(h));
    TEST_ASSERT_EQUAL_STRING("apto3", habitante_compl(h));
    habitante_destruir(h);

    hf_fechar(hf);
}

void test_pm_morador_sem_habitante(void) {
    criar_pm(
        "m 00000000000 cep01 N 10 casa\n"
    );

    int tam_reg = habitante_sizeof_registro();
    HashFile hf = hf_criar(HF_TESTE, 4, tam_reg, 4096,
                           habitante_offset_chave(), habitante_tam_chave());
    pm_processar(PM_TESTE, hf);

    char chave[15], buf[512];
    memset(chave, 0, sizeof(chave));
    strncpy(chave, "00000000000", sizeof(chave));
    TEST_ASSERT_FALSE(hf_buscar(hf, chave, buf));

    hf_fechar(hf);
}

void test_pm_arquivo_vazio(void) {
    criar_pm("");

    int tam_reg = habitante_sizeof_registro();
    HashFile hf = hf_criar(HF_TESTE, 4, tam_reg, 4096,
                           habitante_offset_chave(), habitante_tam_chave());
    pm_processar(PM_TESTE, hf);

    hf_fechar(hf);
    TEST_PASS();
}

void test_pm_null_params(void) {
    pm_processar(NULL, NULL);
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pm_inserir_habitante);
    RUN_TEST(test_pm_tornar_morador);
    RUN_TEST(test_pm_morador_sem_habitante);
    RUN_TEST(test_pm_arquivo_vazio);
    RUN_TEST(test_pm_null_params);
    return UNITY_END();
}
