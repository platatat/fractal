#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include "bignum.h"


struct complex {
    BigNum real;
    BigNum imag;
};


#endif
