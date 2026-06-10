#ifndef PERIPHERAL_CONTROLLER_H
#define PERIPHERAL_CONTROLLER_H

#include <Arduino.h>
#include "telemetrystate.h"
#include "debouncedbutton.h"

class PeripheralController
{
public:
    PeripheralController();
    void init();
    void readInputs(TelemetryState &state);

private:
    const uint8_t PIN_BATTERY_ADC = 36;
};

#endif // PERIPHERAL_CONTROLLER_H
