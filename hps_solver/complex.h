#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include <gmpxx.h>


struct complex {
    mpf_class real;
    mpf_class imag;

    bool operator==(const complex& other) const {
        return real == other.real && imag == other.imag;
    }
};


#endif
