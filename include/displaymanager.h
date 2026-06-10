#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "telemetrystate.h"

class DisplayManager
{
public:
    DisplayManager();
    ~DisplayManager();

    void init();
    void update(const TelemetryState &newState);

private:
    TFT_eSPI m_tft;
    TFT_eSprite m_speedSprite;
    TelemetryState m_currentState;

    // Core Theme Palette
    const uint16_t COLOR_BACKGROUND = TFT_BLACK;
    const uint16_t COLOR_TEXT_PRIMARY = TFT_WHITE;
    const uint16_t COLOR_LOW_BEAM = 0x07E0;  // Neon Green
    const uint16_t COLOR_HIGH_BEAM = 0x001F; // Electric Blue
    const uint16_t COLOR_DIMMED = 0x4208;    // Tactical Dark Gray

    // New Scaled Layout Coordinates (320 x 240 Landscape Grid)
    const int16_t SCREEN_WIDTH = 320;
    const int16_t SCREEN_HEIGHT = 240;

    // Render Sub-routines adapted for 320x240
    void drawStaticLayout();
    void drawBatteryIndicator(uint8_t percentage, bool forceRedraw = false);
    void drawSpeedometerCard(double speed, uint32_t odometer);
    void drawLowBeamIcon(bool active, bool forceRedraw = false);
    void drawHighBeamIcon(bool active, bool forceRedraw = false);
};

#endif // DISPLAY_MANAGER_H
