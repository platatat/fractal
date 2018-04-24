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
        if (i == 1) str << " . ";
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
    //std::cout << "mult " << this << " * " << &other << "\n";

    int max_length = this->length > other.length ? this->length : other.length;
    BigNum ret(max_length);
    //std::cout << "ret " << &ret << "\n";

    //Initialize ret to zero
    for (int i = 0; i < ret.length; i++) {
        ret.limbs[i] = 0;
    }

    bool this_neg = this->isNegative();
    bool other_neg = other.isNegative();

    const BigNum a = this_neg ? -*this : *this;
    //std::cout << "a " << &a << "\n";
    const BigNum b = other_neg ? -other : other;
    //std::cout << "b " << &b << "\n";

    for (int bj = ret.length - 1; bj >= 0; bj--) {
        //std::cout << "bj " << bj << "\n";
        unsigned char extra_limb = 0; //TODO is this extra precision necessary?
        unsigned char carry = 0;
        unsigned char this_limb = bj < b.length ? b.limbs[bj] : 0;
        //std::cout << "this_limb " << (int) this_limb << "\n";
        for (int ai = ret.length - bj; ai >= 0; ai--) {
            //std::cout << "ai " << ai << "\n";
            int limb_index = bj + ai;
            unsigned char other_limb = ai < a.length ? a.limbs[ai] : 0;
            //std::cout << "other_limb " << (int) other_limb << "\n";
            //std::cout << "limb_index " << limb_index << "\n";
            int new_limb = this_limb * other_limb;
            //std::cout << "this * other = " << new_limb << "\n";
            new_limb += carry;
            //std::cout << "carry " << (int) carry << ", total = " << new_limb << "\n";
            if (limb_index < ret.length) {
                new_limb += ret.limbs[limb_index];
                //std::cout << "old_limb " << (int) ret.limbs[limb_index] << ", total " << new_limb << "\n";
                ret.limbs[limb_index] = (unsigned char) new_limb;
                //std::cout << "new_limb " << (int) ret.limbs[limb_index] << "\n";
            } else {
                new_limb += extra_limb;
                //std::cout << "old_extra_limb " << (int) extra_limb << ", total " << new_limb << "\n";
                extra_limb = (unsigned char) new_limb;
                //std::cout << "new_extra_limb " << (int) extra_limb << "\n";
            }
            carry = (unsigned char) (new_limb >> 8);
            //std::cout << "new carry " << (int) carry << "\n";
        }
        for (int i = bj - 1; i >= 0; i--) {
            //std::cout << "carrying " << (int) carry << " to index " << i << "\n";
            if (carry == 0) break;
            int sum = ret.limbs[i] + carry;
            ret.limbs[i] = (unsigned char) sum;
            //std::cout << "ret.limbs[ " << i << "] now = " << (int) ret.limbs[i] << "\n";
            carry = (unsigned char) (sum >> 8);
        }
    }

    if (this_neg ^ other_neg) {
        //std::cout << "negate ret\n";
        std::cout << ret.toString() << " = " << ret.toDouble() << "\n";
        return -ret;
    } else {
        //std::cout << "positive ret\n";
        std::cout << ret.toString() << " = " << ret.toDouble() << "\n";
        return ret;
    }
}

