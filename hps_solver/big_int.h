#ifndef __BIG_INT_H__
#define __BIG_INT_H__

#include "big_float.h"
#include <gmpxx.h>


class BFloat;


class BInt {
private:
    mpz_t _z;

    friend class BFloat;

public:
    BInt();
    BInt(long n);
    ~BInt();

    long toLong() const;

    bool operator==(const BInt& bi) const;
    bool operator==(const long n) const;

    bool operator<(const BInt& bi) const;
    bool operator<(const long n) const;

    bool operator<=(const BInt& bi) const;
    bool operator<=(const long n) const;

    BFloat operator*(const BFloat& bf) const;
    BFloat operator*(const double d) const;

    BInt operator+(const BInt& bi) const;
    BInt operator+(const long n) const;

    BInt operator-(const BInt& bi) const;
    BInt operator-(const long n) const;

    BInt operator-() const;
};


#endif
