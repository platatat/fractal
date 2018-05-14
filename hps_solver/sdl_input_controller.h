#ifndef __SDL_INPUT_CONTROLLER_H__
#define __SDL_INPUT_CONTROLLER_H__

#include "model/Input.h"

class SdlInputController {
private:
    bool last_iter_inc_state;
    bool last_iter_dec_state;

public:
    SdlInputController();
    model::Input getInput();
};

#endif
