#include "peripheralcontroller.h"

PeripheralController::PeripheralController(){}

void PeripheralController::init()
{
    // ADC configuration for battery voltage divider circuit analytics
    analogSetAttenuation(ADC_11db);
}

void PeripheralController::loadPreferences(TelemetryState &state)
{
    // Open the "bike_data" namespace in Read/Write mode (false)
    m_preferences.begin("bike_data", false);

    // Grab the saved float. If it doesn't exist yet, return 0.0f
    state.odometerKm = m_preferences.getFloat("odo", 0.0f);

    // Sync our tracker so we don't immediately overwrite on boot
    m_lastSavedOdometerKm = state.odometerKm;

    Serial.printf("Loaded Odometer from Memory: %.3f km\n", state.odometerKm);
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

        // --- SAFE FLASH SAVING LOGIC ---
        // Only write to memory if the current odemeter is 0.1km (100 meters)
        // higher than the last time we saved it into memory.
        if ((state.odometerKm - m_lastSavedOdometerKm) >= 0.1f)
        {
            m_preferences.putFloat("odo", state.odometerKm);
            m_lastSavedOdometerKm = state.odometerKm; // Update our tracker
            Serial.printf("SAVED TO FLASH: %.3f km\n", state.odometerKm);
        }
    }

    // Track current time for the next loop execution
    lastTime = currentTime;
}
