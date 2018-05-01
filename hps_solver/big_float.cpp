#include "big_float.h"
#include "constants.h"


// CONSTRUCTORS

BFloat::BFloat() {
    mpf_init2(_f, Constants::MPF_PRECISION);
}


BFloat::BFloat(double d) {
    mpf_init2(_f, Constants::MPF_PRECISION);
    mpf_set_d(_f, d);
}


BFloat::BFloat(BInt bi) {
    mpf_init2(_f, Constants::MPF_PRECISION);
    mpf_set_z(_f, bi._z);
}


BFloat::~BFloat() {
    // mpf_clear(_f);
}


// CONVERSION

double BFloat::toDouble() const {
    return mpf_get_d(_f);
}


BInt BFloat::floor() const {
    BInt result = BInt();
    mpz_set_f(result._z, _f);
    return result;
}


BFloat BFloat::abs() const {
    BFloat result = BFloat();
    mpf_abs(result._f, _f);
    return result;
}


// COMPARISON

bool BFloat::operator>(const BFloat bf) const {
    return mpf_cmp(_f, bf._f) > 0;
}


bool BFloat::operator>(const double d) const {
    return *this > BFloat(d);
}


// ADDITION

BFloat BFloat::operator+(const BFloat bf) const {
    BFloat result = BFloat();
    mpf_add(result._f, _f, bf._f);
    return result;
}


BFloat BFloat::operator+(const double d) const {
    return *this + BFloat(d);
}


// SUBTRACTION

BFloat BFloat::operator-(const BFloat bf) const {
    BFloat result = BFloat();
    mpf_sub(result._f, _f, bf._f);
    return result;
}


BFloat BFloat::operator-(const double d) const {
    return *this - BFloat(d);
}


// MULTIPLICATION

BFloat BFloat::operator*(const BFloat bf) const {
    BFloat result = BFloat();
    mpf_mul(result._f, _f, bf._f);
    return result;
}


BFloat BFloat::operator*(const double d) const {
    return *this * BFloat(d);
}


// DIVISION

BFloat BFloat::operator/(const BFloat bf) const {
    BFloat result = BFloat();
    mpf_div(result._f, _f, bf._f);
    return result;
}


BFloat BFloat::operator/(const double d) const {
    return *this / BFloat(d);
}


// MISC

BFloat BFloat::operator-() const {
    BFloat result = BFloat();
    mpf_neg(result._f, _f);
    return result;
}