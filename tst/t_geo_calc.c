#include "unity.h"
#include "geo_calc.h"
#include <stddef.h>

void setUp(void) {}
void tearDown(void) {}

void test_geo_calc_sul(void) {
    double rx, ry;
    
    geo_calc_endereco(10.0, 10.0, 100.0, 50.0, 'S', 20, &rx, &ry);
    
    TEST_ASSERT_EQUAL_DOUBLE(30.0, rx);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, ry);
}

void test_geo_calc_leste(void) {
    double rx, ry;
    geo_calc_endereco(10.0, 10.0, 100.0, 50.0, 'L', 15, &rx, &ry);
    
    TEST_ASSERT_EQUAL_DOUBLE(10.0, rx);
    TEST_ASSERT_EQUAL_DOUBLE(25.0, ry);
}

void test_geo_calc_norte(void) {
    double rx, ry;
    geo_calc_endereco(10.0, 10.0, 100.0, 50.0, 'N', 30, &rx, &ry);
    
    TEST_ASSERT_EQUAL_DOUBLE(40.0, rx);
    TEST_ASSERT_EQUAL_DOUBLE(60.0, ry);
}

void test_geo_calc_oeste(void) {
    double rx, ry;
    geo_calc_endereco(10.0, 10.0, 100.0, 50.0, 'O', 25, &rx, &ry);
    
    TEST_ASSERT_EQUAL_DOUBLE(110.0, rx);
    TEST_ASSERT_EQUAL_DOUBLE(35.0, ry);
}

void test_geo_calc_null(void) {
    double rx;
    geo_calc_endereco(10.0, 10.0, 100.0, 50.0, 'S', 20, NULL, &rx);
    geo_calc_endereco(10.0, 10.0, 100.0, 50.0, 'S', 20, &rx, NULL);
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_geo_calc_sul);
    RUN_TEST(test_geo_calc_leste);
    RUN_TEST(test_geo_calc_norte);
    RUN_TEST(test_geo_calc_oeste);
    RUN_TEST(test_geo_calc_null);
    return UNITY_END();
}
