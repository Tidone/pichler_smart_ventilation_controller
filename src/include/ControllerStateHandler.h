#ifndef __CONTROLLERSTATEHANDLER_H
#define __CONTROLLERSTATEHANDLER_H

#include "ControllerState.h"
#include <cstdint>

class ControllerStateHandler
{
private:
    uint32_t yellowLedPin;
    uint32_t greenLedPin;
    uint32_t buttonPin;

    bool busyFlag = false;

    ControllerState expectedState = ControllerState::FAN_LOW;
    uint32_t lastButtonPressTimestamp = 0;
    uint32_t lastButtonReleaseTimestamp = 0;
    uint8_t buttonPressCount = 0;
    bool newStateRequested = false;

public:
    ControllerStateHandler(uint32_t yellowLed, uint32_t greenLed, uint32_t buttonPin) : yellowLedPin(yellowLed), greenLedPin(greenLed), buttonPin(buttonPin) {}

    void begin();
    void loop();
    ControllerState getState();

    /* automatically simulates a button press until the new state is reached */
    void setState(ControllerState newState);

    bool isBusy() { return busyFlag; }
};

#endif //__CONTROLLERSTATEHANDLER_H
