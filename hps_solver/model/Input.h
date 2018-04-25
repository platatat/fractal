#ifndef __MODEL_INPUT_H__
#define __MODEL_INPUT_H__

namespace model {

struct Input {
    // The shift in the x / y directions, in the range [-1, +1].
    double dx, dy;

    // The amount to zoom in or out, in the range [-1, +1].
    double dz;

    Input() : dx(0), dy(0), dz(0) {};
};

}

#endif
