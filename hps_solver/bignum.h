#ifndef __BIGNUM_H__
#define __BIGNUM_H__


class BigNum {
private:
    double _data;

public:
    BigNum(double value);

    double toDouble() { return _data; }

    BigNum operator + (BigNum other) { return _data + other._data; }

    BigNum operator + (double other) { return _data + other; }

    BigNum operator - (BigNum other) { return _data - other._data; }

    BigNum operator - (double other) { return _data - other; }

    BigNum operator * (BigNum other) { return _data * other._data; }

    BigNum operator * (double other) { return _data * other; }

    BigNum operator / (BigNum other) { return _data / other._data; }

    BigNum operator / (double other) { return _data / other; }

    bool operator > (BigNum other) { return _data > other._data; }

    bool operator > (double other) { return _data > other; }
};


#endif