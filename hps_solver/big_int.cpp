#include "big_int.h"


// CONSTRUCTORS

BInt::BInt() {
    mpz_init(_z);
}


BInt::BInt(long n) {
    mpz_init_set_si(_z, n);
}


BInt::~BInt() {
    // mpz_clear(_z);
}


// CONVERSION

long BInt::toLong() const {
    return mpz_get_si(_z);
}


// COMPARISON

bool BInt::operator==(const BInt& bi) const {
    return mpz_cmp(_z, bi._z) == 0;
}


bool BInt::operator==(const long n) const {
    return *this == BInt(n);
}


bool BInt::operator<(const BInt& bi) const {
    return mpz_cmp(_z, bi._z) < 0;
}


bool BInt::operator<(const long n) const {
    return *this < BInt(n);
}

bool BInt::operator<=(const BInt& bi) const {
    return mpz_cmp(_z, bi._z) <= 0;
}


bool BInt::operator<=(const long n) const {
    return *this < BInt(n);
}


// MULTIPLICATION

BFloat BInt::operator*(const BFloat& bf) const {
    return BFloat(*this) * bf;
}


BFloat BInt::operator*(const double d) const {
    return BFloat(*this) * d;
}


// ADDITION

BInt BInt::operator+(const BInt& bi) const {
    BInt result = BInt();
    mpz_add(result._z, _z, bi._z);
    return result;
}


BInt BInt::operator+(const long n) const {
    return *this + BInt(n);
}


// SUBTRACTION

BInt BInt::operator-(const BInt& bi) const {
    BInt result = BInt();
    mpz_sub(result._z, _z, bi._z);
    return result;
}


BInt BInt::operator-(const long n) const {
    return *this - BInt(n);
}


// MISC

BInt BInt::operator-() const {
    BInt result = BInt();
    mpz_neg(result._z, _z);
    return result;
}
