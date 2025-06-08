#ifndef __CONTROLLERSTATE_H
#define __CONTROLLERSTATE_H

enum ControllerState
{
    FAN_LOW,
    FAN_MEDIUM,
    FAN_HIGH
};

char *getControllerStateString(ControllerState state);

#endif //__CONTROLLERSTATE_H
