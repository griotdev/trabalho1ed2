#include "unity.h"
#include "quadra.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ========== Criação e getters ========== */

void test_quadra_criar(void) {
    Quadra q = quadra_criar("cep01", 10.0, 20.0, 30.0, 40.0);
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("cep01", quadra_cep(q));
    TEST_ASSERT_EQUAL_DOUBLE(10.0, quadra_x(q));
    TEST_ASSERT_EQUAL_DOUBLE(20.0, quadra_y(q));
    TEST_ASSERT_EQUAL_DOUBLE(30.0, quadra_w(q));
    TEST_ASSERT_EQUAL_DOUBLE(40.0, quadra_h(q));
    quadra_destruir(q);
}

void test_quadra_criar_null(void) {
    TEST_ASSERT_NULL(quadra_criar(NULL, 0, 0, 0, 0));
}

void test_quadra_destruir_null(void) {
    quadra_destruir(NULL);
    TEST_PASS();
}

void test_quadra_getters_null(void) {
    TEST_ASSERT_NULL(quadra_cep(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_x(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_y(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_w(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_h(NULL));
}

/* ========== Estilo visual ========== */

void test_quadra_estilo_padrao(void) {
    Quadra q = quadra_criar("cep02", 0, 0, 10, 10);
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, quadra_sw(q));
    TEST_ASSERT_EQUAL_STRING("#CCCCCC", quadra_cfill(q));
    TEST_ASSERT_EQUAL_STRING("#000000", quadra_cstrk(q));
    quadra_destruir(q);
}

void test_quadra_set_estilo(void) {
    Quadra q = quadra_criar("cep03", 0, 0, 10, 10);
    quadra_set_estilo(q, 2.5, "red", "blue");
    TEST_ASSERT_EQUAL_DOUBLE(2.5, quadra_sw(q));
    TEST_ASSERT_EQUAL_STRING("red", quadra_cfill(q));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_cstrk(q));
    quadra_destruir(q);
}

/* ========== Serialização ========== */

void test_quadra_serializar_desserializar(void) {
    Quadra q = quadra_criar("cep_teste", 100.5, 200.3, 50.0, 75.0);
    quadra_set_estilo(q, 3.0, "green", "yellow");

    /* Serializar: usamos sizeof(struct) estimado — precisamos do tamanho real */
    /* Como é ponteiro opaco, usamos o tamanho do registro que o hashfile espera */
    char buf[256];
    memset(buf, 0, sizeof(buf));
    quadra_serializar(q, buf, sizeof(buf));

    /* Desserializar */
    Quadra q2 = quadra_desserializar(buf);
    TEST_ASSERT_NOT_NULL(q2);
    TEST_ASSERT_EQUAL_STRING("cep_teste", quadra_cep(q2));
    TEST_ASSERT_EQUAL_DOUBLE(100.5, quadra_x(q2));
    TEST_ASSERT_EQUAL_DOUBLE(200.3, quadra_y(q2));
    TEST_ASSERT_EQUAL_DOUBLE(50.0, quadra_w(q2));
    TEST_ASSERT_EQUAL_DOUBLE(75.0, quadra_h(q2));
    TEST_ASSERT_EQUAL_DOUBLE(3.0, quadra_sw(q2));
    TEST_ASSERT_EQUAL_STRING("green", quadra_cfill(q2));
    TEST_ASSERT_EQUAL_STRING("yellow", quadra_cstrk(q2));

    quadra_destruir(q);
    quadra_destruir(q2);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_quadra_criar);
    RUN_TEST(test_quadra_criar_null);
    RUN_TEST(test_quadra_destruir_null);
    RUN_TEST(test_quadra_getters_null);
    RUN_TEST(test_quadra_estilo_padrao);
    RUN_TEST(test_quadra_set_estilo);
    RUN_TEST(test_quadra_serializar_desserializar);
    return UNITY_END();
}
