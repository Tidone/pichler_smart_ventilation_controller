#include "StateHandler.h"
#include <Arduino.h>

void StateHandler::begin()
{
    controllerStateHandler->begin();

    oldState = controllerStateHandler->getState();
}

void StateHandler::loop()
{
    controllerStateHandler->loop();
    bool controllerIsBusy = controllerStateHandler->isBusy();

    if (controllerIsBusy)
    {
        return;
    }

    // We don't need to check the state every iteration. Do it every 100ms instead.
    uint32_t now = millis();
    if ((now - lastCheckStateTimestamp) < 100)
    {
        return;
    }

    lastCheckStateTimestamp = now;
    ControllerState currentState = controllerStateHandler->getState();
    if (currentState != oldState)
    {
        oldState = currentState;
        if (onStateChangedCallback)
        {
            onStateChangedCallback(currentState);
        }
    }
}

void StateHandler::setState(ControllerState newState)
{
    controllerStateHandler->setState(newState);
}
