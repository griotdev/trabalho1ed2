#include "unity.h"
#include "habitante.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}



void test_habitante_criar(void) {
    Habitante h = habitante_criar("12345678901", "Joao", "Silva", 'M', "01/01/2000");
    TEST_ASSERT_NOT_NULL(h);
    TEST_ASSERT_EQUAL_STRING("12345678901", habitante_cpf(h));
    TEST_ASSERT_EQUAL_STRING("Joao", habitante_nome(h));
    TEST_ASSERT_EQUAL_STRING("Silva", habitante_sobrenome(h));
    TEST_ASSERT_EQUAL_CHAR('M', habitante_sexo(h));
    TEST_ASSERT_EQUAL_STRING("01/01/2000", habitante_nasc(h));
    TEST_ASSERT_EQUAL_INT(0, habitante_e_morador(h));
    habitante_destruir(h);
}

void test_habitante_criar_null(void) {
    TEST_ASSERT_NULL(habitante_criar(NULL, "N", "S", 'M', "01/01"));
    TEST_ASSERT_NULL(habitante_criar("123", NULL, "S", 'M', "01/01"));
    TEST_ASSERT_NULL(habitante_criar("123", "N", NULL, 'M', "01/01"));
    TEST_ASSERT_NULL(habitante_criar("123", "N", "S", 'M', NULL));
}

void test_habitante_getters_null(void) {
    TEST_ASSERT_NULL(habitante_cpf(NULL));
    TEST_ASSERT_NULL(habitante_nome(NULL));
    TEST_ASSERT_NULL(habitante_sobrenome(NULL));
    TEST_ASSERT_EQUAL_CHAR('?', habitante_sexo(NULL));
    TEST_ASSERT_NULL(habitante_nasc(NULL));
}



void test_habitante_endereco(void) {
    Habitante h = habitante_criar("99988877766", "Maria", "Santos", 'F', "15/06/1990");
    TEST_ASSERT_EQUAL_INT(0, habitante_e_morador(h));

    habitante_set_endereco(h, "cep15", "S", 45, "apto3");
    TEST_ASSERT_EQUAL_INT(1, habitante_e_morador(h));
    TEST_ASSERT_EQUAL_STRING("cep15", habitante_cep(h));
    TEST_ASSERT_EQUAL_STRING("S", habitante_face(h));
    TEST_ASSERT_EQUAL_INT(45, habitante_num(h));
    TEST_ASSERT_EQUAL_STRING("apto3", habitante_compl(h));

    
    habitante_remover_endereco(h);
    TEST_ASSERT_EQUAL_INT(0, habitante_e_morador(h));
    TEST_ASSERT_EQUAL_STRING("", habitante_cep(h));
    TEST_ASSERT_EQUAL_INT(0, habitante_num(h));

    habitante_destruir(h);
}

void test_habitante_mudar_endereco(void) {
    Habitante h = habitante_criar("11122233344", "Pedro", "Costa", 'M', "20/03/1985");

    habitante_set_endereco(h, "cep10", "N", 100, "casa");
    TEST_ASSERT_EQUAL_STRING("cep10", habitante_cep(h));

    
    habitante_set_endereco(h, "cep20", "L", 50, "sobrado");
    TEST_ASSERT_EQUAL_INT(1, habitante_e_morador(h));
    TEST_ASSERT_EQUAL_STRING("cep20", habitante_cep(h));
    TEST_ASSERT_EQUAL_STRING("L", habitante_face(h));
    TEST_ASSERT_EQUAL_INT(50, habitante_num(h));
    TEST_ASSERT_EQUAL_STRING("sobrado", habitante_compl(h));

    habitante_destruir(h);
}



void test_habitante_serializar_desserializar(void) {
    Habitante h = habitante_criar("55566677788", "Ana", "Oliveira", 'F', "10/12/1995");
    habitante_set_endereco(h, "cep07", "O", 30, "bloco2");

    char buf[512];
    memset(buf, 0, sizeof(buf));
    habitante_serializar(h, buf, sizeof(buf));

    Habitante h2 = habitante_desserializar(buf);
    TEST_ASSERT_NOT_NULL(h2);
    TEST_ASSERT_EQUAL_STRING("55566677788", habitante_cpf(h2));
    TEST_ASSERT_EQUAL_STRING("Ana", habitante_nome(h2));
    TEST_ASSERT_EQUAL_STRING("Oliveira", habitante_sobrenome(h2));
    TEST_ASSERT_EQUAL_CHAR('F', habitante_sexo(h2));
    TEST_ASSERT_EQUAL_STRING("10/12/1995", habitante_nasc(h2));
    TEST_ASSERT_EQUAL_INT(1, habitante_e_morador(h2));
    TEST_ASSERT_EQUAL_STRING("cep07", habitante_cep(h2));
    TEST_ASSERT_EQUAL_STRING("O", habitante_face(h2));
    TEST_ASSERT_EQUAL_INT(30, habitante_num(h2));
    TEST_ASSERT_EQUAL_STRING("bloco2", habitante_compl(h2));

    habitante_destruir(h);
    habitante_destruir(h2);
}

void test_habitante_serializar_sem_teto(void) {
    Habitante h = habitante_criar("00011122233", "Carlos", "Lima", 'M', "05/08/2001");

    char buf[512];
    memset(buf, 0, sizeof(buf));
    habitante_serializar(h, buf, sizeof(buf));

    Habitante h2 = habitante_desserializar(buf);
    TEST_ASSERT_NOT_NULL(h2);
    TEST_ASSERT_EQUAL_INT(0, habitante_e_morador(h2));
    TEST_ASSERT_EQUAL_STRING("Carlos", habitante_nome(h2));

    habitante_destruir(h);
    habitante_destruir(h2);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_habitante_criar);
    RUN_TEST(test_habitante_criar_null);
    RUN_TEST(test_habitante_getters_null);
    RUN_TEST(test_habitante_endereco);
    RUN_TEST(test_habitante_mudar_endereco);
    RUN_TEST(test_habitante_serializar_desserializar);
    RUN_TEST(test_habitante_serializar_sem_teto);
    return UNITY_END();
}
