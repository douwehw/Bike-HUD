#ifndef PERIPHERAL_CONTROLLER_H
#define PERIPHERAL_CONTROLLER_H

#include <Arduino.h>
#include <Preferences.h>
#include "telemetrystate.h"
#include "debouncedbutton.h"

class PeripheralController
{
public:
    PeripheralController();
    void init();
    // Pass state by reference so we can initialize the loaded odometer value into it
    void loadPreferences(TelemetryState &state);
    void readInputs(TelemetryState &state);

private:
    const uint8_t PIN_BATTERY_ADC = 36;

    // Preferences object for flash memory
    Preferences m_preferences;
    // Track the last value we successfully saved to avoid spamming the flash memory
    float m_lastSavedOdometerKm = 0.0f;
};

#endif // PERIPHERAL_CONTROLLER_H
