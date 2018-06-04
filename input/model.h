#ifndef __INPUT_MODEL_H__
#define __INPUT_MODEL_H__

namespace input {

struct Model {
    double dx;
    double dy;
    double dz;
    double di;

    Model() : dx(0), dy(0), dz(0), di(0) {};
};

}

#endif
