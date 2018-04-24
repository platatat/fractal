#include "bignum.h"

#include <math.h>
#include <sstream>
#include <vector>


//TODO remove
#include <iostream>



BigNum::BigNum(int length) : length(length), limbs(new unsigned char[length]) {
    //std::cout << "-------------------> construct " << this << "\n";
}

BigNum::BigNum(double value) {
    //std::cout << "-------------------> construct " << this << "\n";
    bool negative = value < 0;
    if (negative) value *= -1;

    std::vector<unsigned char> limbs_vec;
    do {
        unsigned char limb = (unsigned char) value;
        limbs_vec.push_back(limb);
        value -= limb;
        value *= 256;
    } while (value != 0);

    this->length = limbs_vec.size();
    this->limbs = new unsigned char[this->length];
    int i = 0;
    for (unsigned char limb : limbs_vec) {
        this->limbs[i] = limb;
        i++;
    }

    if (negative) {
        unsigned char carry = 1;
        for (int i = this->length - 1; i >= 0; i--) {
            this->limbs[i] ^= 0xff;
            unsigned char next_carry = this->limbs[i] == 255 && carry == 1 ? 1 : 0;
            this->limbs[i] += carry;
            carry = next_carry;
        }
    }
}

BigNum::~BigNum() {
    //TODO fix memory leak issues
    //std::cout << "<------------------- destruct " << this << "\n";
    //delete[] limbs;
}

std::string BigNum::toString() const {
    std::ostringstream str;
    for (int i = 0; i < this->length; i++) {
        if (i != 0) str << " ";
        if (i == 1) str << ". ";
        str << (int) this->limbs[i];
    }
    return str.str();
}

bool BigNum::isNegative() const {
    return this->limbs[0] > 127;
}

double BigNum::toDouble() const {
    bool negative = this->isNegative();

    unsigned char carry = negative ? 1 : 0;
    double ret = 0;
    for (int i = this->length - 1; i >= 0 ; i--) {
        unsigned char limb = this->limbs[i];
        if (negative) {
            limb ^= 0xff;
            unsigned char next_carry = limb == 255 && carry == 1 ? 1 : 0;
            limb += carry;
            carry = next_carry;
        }
        ret /= 256;
        ret += limb;
    }
    if (negative) ret *= -1;
    return ret;
}

BigNum BigNum::operator+(const BigNum& other) const {
    int max_length = this->length > other.length ? this->length : other.length;
    BigNum ret(max_length);

    unsigned char carry = 0;
    for (int i = max_length - 1; i >= 0; i--) {
        unsigned char this_limb = i < this->length ? this->limbs[i] : 0;
        unsigned char other_limb = i < other.length ? other.limbs[i] : 0;

        int sum = this_limb + other_limb + carry;
        ret.limbs[i] = (unsigned char) sum;
        carry = (unsigned char) (sum >> 8);
    }
    return ret;
}

BigNum BigNum::operator-() const {
    BigNum ret = BigNum((int) this->length);

    unsigned char carry = 1;
    for (int i = this->length - 1; i >= 0; i--) {
        ret.limbs[i] = this->limbs[i] ^ 0xff;
        unsigned char next_carry = ret.limbs[i] == 255 && carry == 1 ? 1 : 0;
        ret.limbs[i] += carry;
        carry = next_carry;
    }
    return ret;
}

BigNum BigNum::operator*(const BigNum& other) const {
    int max_length = this->length > other.length ? this->length : other.length;
    BigNum ret(max_length);

    //Initialize ret to zero
    for (int i = 0; i < ret.length; i++) {
        ret.limbs[i] = 0;
    }

    bool this_neg = this->isNegative();
    bool other_neg = other.isNegative();

    const BigNum a = this_neg ? -*this : *this;
    const BigNum b = other_neg ? -other : other;

    unsigned char last_carry = 0;
    for (int bj = ret.length - 1; bj >= 0; bj--) {
        unsigned char carry = 0;
        unsigned char extra_limb = 0; //TODO is this extra precision necessary?
        unsigned char b_limb = bj < b.length ? b.limbs[bj] : 0;

        for (int ai = ret.length - bj; ai >= 0; ai--) {
            int limb_index = bj + ai;
            unsigned char a_limb = ai < a.length ? a.limbs[ai] : 0;

            int new_limb = b_limb * a_limb; //TODO can new_limb ever overflow 16 bits. I don't think it could before the last_carry change but now I think it can
            new_limb += carry;
            if (ai == 0) new_limb += last_carry;
            if (limb_index < ret.length) {
                new_limb += ret.limbs[limb_index];
                ret.limbs[limb_index] = (unsigned char) new_limb;
            } else {
                new_limb += extra_limb;
                extra_limb = (unsigned char) new_limb;
            }
            carry = (unsigned char) (new_limb >> 8);
            if (ai == 0) last_carry = carry;
        }
/*
        //TODO this is commented out as part of the last carry optimization - remove once it is determined that new_limb can't overflow 16 bits
        for (int i = bj - 1; i >= 0; i--) {
            if (carry == 0) break;
            int sum = ret.limbs[i] + carry;
            ret.limbs[i] = (unsigned char) sum;
            carry = (unsigned char) (sum >> 8);
        }
//*/
    }

    if (this_neg ^ other_neg) {
        return -ret;
    } else {
        return ret;
    }
}

bool BigNum::operator>(const BigNum& other) const {
    return (*this - other).toDouble() > 0;
}

