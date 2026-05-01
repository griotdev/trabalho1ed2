#include "unity.h"
#include "svg.h"
#include <stdio.h>
#include <string.h>

static const char *SVG_TESTE = "tst_svg_temp.svg";

void setUp(void) {}

void tearDown(void) {
    remove(SVG_TESTE);
}

void test_svg_fluxo_basico(void) {
    FILE *f = svg_abrir(SVG_TESTE, 800, 600);
    TEST_ASSERT_NOT_NULL(f);
    
    svg_retangulo(f, 10, 20, 100, 50, "red", "black", 2.0);
    svg_circulo(f, 50, 50, 10, "blue", "none", 1.0);
    svg_linha(f, 0, 0, 100, 100, "green", 3.5);
    svg_texto(f, 30, 40, "Hello", "yellow", 12.0);
    
    svg_fechar(f);
    
    f = fopen(SVG_TESTE, "r");
    TEST_ASSERT_NOT_NULL(f);
    
    char buf[1024];
    int linhas = 0;
    while (fgets(buf, sizeof(buf), f)) {
        linhas++;
    }
    fclose(f);
    
    
    TEST_ASSERT_EQUAL_INT(6, linhas);
}

void test_svg_null_safety(void) {
    TEST_ASSERT_NULL(svg_abrir(NULL, 100, 100));
    svg_retangulo(NULL, 0,0,0,0, NULL, NULL, 0);
    svg_circulo(NULL, 0,0,0, NULL, NULL, 0);
    svg_linha(NULL, 0,0,0,0, NULL, 0);
    svg_texto(NULL, 0,0, NULL, NULL, 0);
    svg_texto((FILE*)1, 0,0, NULL, NULL, 0); 
    svg_fechar(NULL);
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_svg_fluxo_basico);
    RUN_TEST(test_svg_null_safety);
    return UNITY_END();
}
