#include <Arduino.h>

#include "telemetrystate.h"
#include "displaymanager.h"
#include "serialcommandhandler.h"
#include "peripheralcontroller.h"

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

    testState.speedKmh += 0.5;

    Serial.printf("Current speed: %f\n", testState.speedKmh);
}

void decreaseSpeed()
{
    if (testState.speedKmh <= 0)
    {
        return;
    }

    testState.speedKmh -= 0.5;

    Serial.printf("Current speed: %f\n", testState.speedKmh);
}

void cycleBatteryLife()
{
    if (testState.batteryPercentage <= 0)
    {
        testState.batteryPercentage = 100;
    }
    else
    {
        testState.batteryPercentage -= 20;
    }

    Serial.printf("Battery percentage: %d\n", testState.batteryPercentage);
}

void toggleLightState()
{
    if (!testState.lowBeamOn && !testState.highBeamOn)
    {
        testState.lowBeamOn = true;
        Serial.println("Low beam ON");
    }
    else if (testState.lowBeamOn && !testState.highBeamOn)
    {
        testState.lowBeamOn = false;
        testState.highBeamOn = true;
        Serial.println("Highbeam ON");
    }
    else
    {
        testState.highBeamOn = false;
        Serial.println("Lights OFF");
    }
}

void printOdometer()
{
    Serial.printf("Odometer state: %d \n", testState.odometerKm);
}

// 1. Define the FreeRTOS Task Function
void displayUpdateTask(void *pvParameters)
{
    // A FreeRTOS task MUST contain an infinite loop.
    // It runs completely independently of the standard loop().
    for (;;)
    {
        // Update the display with the latest state
        display.update(testState);

        // Pause THIS specific task for 50 milliseconds
        // This frees up the core to handle other background operations
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\n--- HUD System Started ---");
    Serial.println("Send 1, 2, 3, or 4 to test commands.");

    // Baseline mock data variables
    testState.speedKmh = 0.0;
    testState.odometerKm = 0;
    testState.batteryPercentage = 100;
    testState.lowBeamOn = false;
    testState.highBeamOn = false;

    // Bind commands to command handler
    cmdHandler.createCommand('1', increaseSpeed);
    cmdHandler.createCommand('2', decreaseSpeed);
    cmdHandler.createCommand('3', cycleBatteryLife);
    cmdHandler.createCommand('4', toggleLightState);
    cmdHandler.createCommand('9', printOdometer);

    // Start display manager
    display.init();

    // Start peripheral controller
    pController.init();

    // 2. Spawn the FreeRTOS Task
    xTaskCreatePinnedToCore(
        displayUpdateTask, // 1. Function that contains the task code
        "DisplayUpdate",   // 2. Name of the task (for debugging)
        4096,              // 3. Stack size allocated for this task (in words)
        NULL,              // 4. Parameter passed into the task
        1,                 // 5. Priority of the task (0 is lowest)
        NULL,              // 6. Task handle (useful if we want to delete/suspend it later)
        0                  // 7. Core where the task should run (Core 0)
    );
}

void loop()
{
    // Have the command handler read for inputs on the serial monitor
    cmdHandler.process();

    // Pass info to peripheral controller for odometer
    pController.readInputs(testState);

    // 3. Yield to the scheduler
    // Give a tiny 1-tick yield to the Watchdog Timer to prevent crashes
    vTaskDelay(1);
}
