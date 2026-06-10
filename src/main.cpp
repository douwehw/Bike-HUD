#include <Arduino.h>
#include "telemetrystate.h"
#include "displaymanager.h"
#include "serialcommandhandler.h"
#include "peripheralcontroller.h"

// Instantiate local test assets
TelemetryState testState;
DisplayManager display;
SerialCommandHandler cmdHandler;
PeripheralController pController;

void increaseSpeed()
{
    if (testState.speedKmh >= 99.9)
    {
        return;
    }

    testState.speedKmh += 0.2;
}

void decreaseSpeed()
{
    if (testState.speedKmh <= 0)
    {
        return;
    }

    testState.speedKmh -= 0.2;
}

void cycleBatteryLife()
{
    if (testState.batteryPercentage >= 100)
    {
        testState.batteryPercentage = 80;
    }
    else if (testState.batteryPercentage > 0)
    {
        testState.batteryPercentage -= 20;
    }
    else
    {
        testState.batteryPercentage = 20;
    }
}

void toggleLightState()
{
    if (!testState.lowBeamOn && !testState.highBeamOn)
    {
        testState.lowBeamOn = true;
    }
    else if (testState.lowBeamOn && !testState.highBeamOn)
    {
        testState.lowBeamOn = false;
        testState.highBeamOn = true;
    }
    else
    {
        testState.highBeamOn = false;
    }
}

void handleOdometer()
{

}

void setup()
{
    Serial.begin(9600);

    // Baseline mock data variables
    testState.speedKmh = 0.0;
    testState.odometerKm = 100;
    testState.batteryPercentage = 100;
    testState.lowBeamOn = false;
    testState.highBeamOn = false;

    // Bind commands to command handler
    cmdHandler.createCommand('1', increaseSpeed);
    cmdHandler.createCommand('2', decreaseSpeed);
    cmdHandler.createCommand('3', cycleBatteryLife);
    cmdHandler.createCommand('4', toggleLightState);

    // Start display manager
    display.init();

    // Start peripheral controller
    pController.init();
}

void loop()
{
    // Have the command handler read for inputs on the serial monitor
    cmdHandler.process();

    // Pass info to peripheral controller for odometer
    pController.readInputs(testState);

    // Pass the state information straight to the screen layout framework
    display.update(testState);

    delay(10);
}
