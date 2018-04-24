#ifndef __BIGNUM_H__
#define __BIGNUM_H__

#include <string>


//TODO remove
#include <iostream>



class BigNum {
private:
    unsigned int length;
    unsigned char* limbs;

    BigNum(int length);

public:
    BigNum(double value);
    ~BigNum();

    double toDouble() const;

    std::string toString() const;

    bool isNegative() const;

    BigNum operator+(const BigNum& other) const;

    BigNum operator+(double other) const { return *this + BigNum(other); }

    BigNum operator-() const;

    BigNum operator-(const BigNum& other) const { return *this + -other; }

    BigNum operator-(double other) const { return *this + BigNum(-other); }

    BigNum operator*(const BigNum& other) const;

    BigNum operator*(double other) const { return *this * BigNum(other); }

    bool operator>(const BigNum& other) const;

    bool operator>(double other) const { return *this > BigNum(other); }
};


#endif
