#include "ControllerState.h"

char *getControllerStateString(ControllerState state)
{
    switch (state)
    {
        case ControllerState::FAN_LOW:
            return (char *)"low";
        case ControllerState::FAN_MEDIUM:
            return (char *)"medium";
        default:
            return (char *)"high";
    }
}
