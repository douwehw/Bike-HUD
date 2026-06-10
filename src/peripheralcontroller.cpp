#include "peripheralcontroller.h"

PeripheralController::PeripheralController(){}

void PeripheralController::init()
{
    // ADC configuration for battery voltage divider circuit analytics
    analogSetAttenuation(ADC_11db);
}

void PeripheralController::readInputs(TelemetryState &state)
{
    static unsigned long lastTime = 0;
    unsigned long currentTime = millis();

    if (lastTime != 0)
    {
        // Calculate the time delta since the last frame ran (in milliseconds)
        unsigned long deltaTimeMs = currentTime - lastTime;

        // Convert time delta from milliseconds to hours (3,600,000 ms = 1 hour)
        float deltaTimeHours = deltaTimeMs / 3600000.0f;

        // Distance = Speed * Time
        // Read the live speed updated by ESP-NOW and add the fractional distance to the odometer
        state.odometerKm += (state.speedKmh * deltaTimeHours);
    }

    // Track current time for the next loop execution
    lastTime = currentTime;
}
