#include "peripheralcontroller.h"

PeripheralController::PeripheralController() {}

void PeripheralController::init()
{
    // ADC configuration for battery voltage divider circuit analytics
    analogSetAttenuation(ADC_11db);
    pinMode(26, INPUT);
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
    static unsigned long lastTriggerTime = 0;
    static int lastPinState = HIGH;

    unsigned long currentTime = millis();

    // Read the hall effect sensor
    int currentPinState = digitalRead(m_speedometerPin);

    // Detect falling edge (hall effect sensor triggered)
    if (currentPinState == LOW && lastPinState == HIGH)
    {
        // Software debounce
        if ((currentTime - lastTriggerTime) > m_debounce)
        {
            unsigned long rotationTimeMs = currentTime - lastTriggerTime;

            // Calculate speed

            // Speed (m/ms) = circumference / rotationTimeMs
            // Convert to km/h: (m/ms) * 3600
            if (rotationTimeMs > 0)
            {
                state.speedKmh = (m_circumferenceMeters / rotationTimeMs) * 3600.0f;
            }

            lastTriggerTime = currentTime;
        }
    }
    lastPinState = currentPinState;

    // Reset speed to 0 if the wheel hasn't completed a rotation in 2 seconds (e.g. stopped)
    if (currentTime - lastTriggerTime > 2000)
    {
        state.speedKmh = 0.0f;
    }

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
