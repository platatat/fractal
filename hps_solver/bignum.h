#ifndef __BIG_NUM_H__
#define __BIG_NUM_H__

#include <string>


class MaxBigNum {
private:
    unsigned int length;
    unsigned char* limbs;

    MaxBigNum(int length);

public:
    MaxBigNum(double value);
    ~MaxBigNum();

    double toDouble() const;

    std::string toString() const;

    bool isNegative() const;

    MaxBigNum operator+(const MaxBigNum& other) const;

    MaxBigNum operator+(double other) const { return *this + MaxBigNum(other); }

    MaxBigNum operator-() const;

    MaxBigNum operator-(const MaxBigNum& other) const { return *this + -other; }

    MaxBigNum operator-(double other) const { return *this + MaxBigNum(-other); }

    MaxBigNum operator*(const MaxBigNum& other) const;

    MaxBigNum operator*(double other) const { return *this * MaxBigNum(other); }

    bool operator>(const MaxBigNum& other) const;

    bool operator>(double other) const { return *this > MaxBigNum(other); }
};


class DoubleBigNum {
private:
    double _value;

public:
    DoubleBigNum(double value) { _value = value; }
    ~DoubleBigNum() {}

    double toDouble() const { return _value; }

    bool isNegative() const { return _value < 0; }

    DoubleBigNum operator+(const DoubleBigNum& other) const { return _value + other._value; }

    DoubleBigNum operator+(double other) const { return *this + DoubleBigNum(other); }

    DoubleBigNum operator-() const { return -_value; }

    DoubleBigNum operator-(const DoubleBigNum& other) const { return *this + -other; }

    DoubleBigNum operator-(double other) const { return *this + DoubleBigNum(-other); }

    DoubleBigNum operator*(const DoubleBigNum& other) const { return _value * other._value; }

    DoubleBigNum operator*(double other) const { return *this * DoubleBigNum(other); }

    bool operator>(const DoubleBigNum& other) const { return _value > other._value; }

    bool operator>(double other) const { return *this > DoubleBigNum(other); }
};


/*
typedef MaxBigNum BigNum;
//*/ typedef DoubleBigNum BigNum;


#endif
