#ifndef TELEMETRY_STATE_H
#define TELEMETRY_STATE_H

#include <Arduino.h>

/**
 * @brief Thread-safe plain old data structure (POD) representing
 * the current runtime telemetry state of the bike.
 */
struct TelemetryState
{
    double speedKmh = 0.0;
    float odometerKm = 0;
    uint8_t batteryPercentage = 100;
    bool lowBeamOn = false;
    bool highBeamOn = false;

    // Evaluates equality to optimize away redundant display draw operations
    bool operator==(const TelemetryState &other) const
    {
        return (speedKmh == other.speedKmh) &&
               (odometerKm == other.odometerKm) &&
               (batteryPercentage == other.batteryPercentage) &&
               (lowBeamOn == other.lowBeamOn) &&
               (highBeamOn == other.highBeamOn);
    }

    bool operator!=(const TelemetryState &other) const
    {
        return !(*this == other);
    }
};

#endif // TELEMETRY_STATE_H
