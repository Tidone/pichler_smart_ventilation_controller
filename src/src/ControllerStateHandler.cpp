#include "ControllerStateHandler.h"
#include <Arduino.h>

#define BUTTON_PRESS_TIME_MS 200
#define MAX_BUTTON_PRESS_COUNT 6

void ControllerStateHandler::begin()
{
    pinMode(yellowLedPin, INPUT_PULLUP);
    pinMode(greenLedPin, INPUT_PULLUP);
    pinMode(buttonPin, OUTPUT);
    digitalWrite(buttonPin, LOW);
}

ControllerState ControllerStateHandler::getState()
{
    if (digitalRead(yellowLedPin))
    {
        return ControllerState::FAN_LOW;
    }
    else if (digitalRead(greenLedPin))
    {
        return ControllerState::FAN_MEDIUM;
    }
    return ControllerState::FAN_HIGH;
}

void ControllerStateHandler::setState(ControllerState newState)
{
    Serial1.printf("Changing state to %s\n", getControllerStateString(newState));

    // we only set the new state if we are not busy
    if (!busyFlag)
    {
        expectedState = newState;
        buttonPressCount = 0;
        newStateRequested = true;
    }
}

void ControllerStateHandler::loop()
{
    if (lastButtonPressTimestamp != 0)
    {
        uint32_t now = millis();
        if ((now - lastButtonPressTimestamp) >= BUTTON_PRESS_TIME_MS)
        {
            // the button has been pressed long enough -> release it
            lastButtonPressTimestamp = 0;
            lastButtonReleaseTimestamp = now;
            digitalWrite(buttonPin, LOW);
        }
        else
        {
            // we are still waiting until we can release the button
            busyFlag = true;
            return;
        }
    }

    if (lastButtonReleaseTimestamp != 0)
    {
        uint32_t now = millis();
        if ((now - lastButtonReleaseTimestamp) >= BUTTON_PRESS_TIME_MS)
        {
            // the button has been released long enough -> we are not busy any more
            lastButtonReleaseTimestamp = 0;
            busyFlag = false;
        }
        else
        {
            // we are still waiting for the external controller to settle the new state
            busyFlag = true;
            return;
        }
    }

    ControllerState currentState = getState();

    // TODO maybe also check the busy flag here? (should not be necessary)
    // only set the new state if setState() has been called
    // otherwise this function would also run if the user has changed the state manually by pressing the button,
    // and it would reset the state back to the old value
    if (newStateRequested && currentState != expectedState)
    {
        if (buttonPressCount >= MAX_BUTTON_PRESS_COUNT)
        {
            Serial1.println("Max number of button presses reached -> Aborting");
            expectedState = currentState;
            newStateRequested = false;
            return;
        }
        Serial1.printf("Controller state (%s) is not the expected state (%s) -> pressing button\n", getControllerStateString(currentState), getControllerStateString(expectedState));

        // we have to change the state -> start pressing the button
        busyFlag = true;
        lastButtonPressTimestamp = millis();
        digitalWrite(buttonPin, HIGH);
        buttonPressCount++;
    }
    else
    {
        newStateRequested = false;
    }
}
