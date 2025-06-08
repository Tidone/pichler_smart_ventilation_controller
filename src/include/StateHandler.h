#ifndef __STATEHANDLER_H
#define __STATEHANDLER_H

#include "ControllerStateHandler.h"

#define ONSTATECHANGED_CALLBACK(name) void (*name)(ControllerState newState)

class StateHandler
{
private:
    ControllerStateHandler *controllerStateHandler;
    ControllerState oldState;
    uint32_t lastCheckStateTimestamp = 0;

    ONSTATECHANGED_CALLBACK(onStateChangedCallback) = nullptr;

public:
    StateHandler(ControllerStateHandler &controllerStateHandler) : controllerStateHandler(&controllerStateHandler) {}

    void begin();
    void loop();
    void setState(ControllerState newState);

    void onStateChanged(ONSTATECHANGED_CALLBACK(callback)) { onStateChangedCallback = callback; }
};

#endif //__STATEHANDLER_H
