#ifndef __BIG_FLOAT_H__
#define __BIG_FLOAT_H__

#include "big_int.h"
#include <gmpxx.h>


class BInt;


class BFloat {
private:
    mpf_t _f;

    friend class BInt;

public:
    BFloat();
    BFloat(double d);
    BFloat(BInt bi);
    ~BFloat();

    double toDouble() const;
    BInt floor() const;
    BFloat abs() const;

    bool operator>(const BFloat bf) const;
    bool operator>(const double d) const;

    BFloat operator+(const BFloat bf) const;
    BFloat operator+(const double d) const;

    BFloat operator-(const BFloat bf) const;
    BFloat operator-(const double d) const;

    BFloat operator*(const BFloat bf) const;
    BFloat operator*(const double d) const;

    BFloat operator/(const BFloat bf) const;
    BFloat operator/(const double d) const;

    BFloat operator-() const;
};


#endif