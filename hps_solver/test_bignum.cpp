#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "bignum.h"

#define EPSILON 0.00000000001

double get_rand() {
    int range = 22;
    return (((double) rand()) / RAND_MAX) * range - (range / 2);
}

void assertEquals(double a, BigNum num) {
    double num_val = num.toDouble();
    if (std::abs(a - num_val) > EPSILON) {
        std::cout << a << " != " << num.toString() << " (" << num_val << ")" << "\n";
        std::cout << "Failed\n";
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    int num_tests = 100000;

    std::cout << "Testing double conversion\n";
    for (int i = 0; i < num_tests; i++) {
        double val = get_rand();
        BigNum num(val);
        assertEquals(val, num);
        std::cout << "\r" << (i + 1) << "     " << std::flush;
    }
    std::cout << "Passed\n\n";

    std::cout << "Testing addition\n";
    for (int i = 0; i < num_tests; i++) {
        double a = get_rand();
        double b = get_rand();
        BigNum big_a(a);
        BigNum big_b(b);
        BigNum big_sum = big_a + big_b;
        assertEquals(a + b, big_sum);
        std::cout << "\r" << (i + 1) << "     " << std::flush;
    }
    std::cout << "Passed\n\n";

    std::cout << "Testing negation\n";
    for (int i = 0; i < num_tests; i++) {
        double val = get_rand();
        BigNum num(val);
        assertEquals(-val, -num);
        std::cout << "\r" << (i + 1) << "     " << std::flush;
    }
    std::cout << "Passed\n\n";

    std::cout << "Testing subtraction\n";
    for (int i = 0; i < num_tests; i++) {
        double a = get_rand();
        double b = get_rand();
        BigNum big_a(a);
        BigNum big_b(b);
        BigNum big_diff = big_a - big_b;
        assertEquals(a - b, big_diff);
        std::cout << "\r" << (i + 1) << "     " << std::flush;
    }
    std::cout << "Passed\n\n";

    std::cout << "Testing multiplication\n";
    for (int i = 0; i < num_tests; i++) {
        double a = get_rand();
        double b = get_rand();
        BigNum big_a(a);
        BigNum big_b(b);
        BigNum big_product = big_a * big_b;
        assertEquals(a * b, big_product);
        std::cout << "\r" << (i + 1) << "     " << std::flush;
    }
    std::cout << "Passed\n\n";

/*
    BigNum foo(1.5);
    BigNum bar(0.5);
    std::cout << "foo: " << foo.toString() << "\n";
    std::cout << "bar: " << bar.toString() << "\n";
    BigNum bla(foo * bar);
    std::cout << "end\n";
    std::cout << get_rand() << "\n";*/
    return 0;
}
