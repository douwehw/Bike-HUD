#include "displaymanager.h"

DisplayManager::DisplayManager()
    : m_tft(TFT_eSPI()), m_speedSprite(&m_tft) {}

DisplayManager::~DisplayManager()
{
    m_speedSprite.deleteSprite();
}

void DisplayManager::init()
{
    m_tft.init();
    m_tft.setRotation(0); // 0 = Portrait (240x320)
    m_tft.fillScreen(COLOR_BACKGROUND);

    // Sprite sized for the portrait central box
    m_speedSprite.createSprite(200, 160);
    m_speedSprite.setColorDepth(16);

    drawStaticLayout();

    // Initial draw pass forcing visibility
    drawBatteryIndicator(m_currentState.batteryPercentage, true);
    drawSpeedometerCard(m_currentState.speedKmh, m_currentState.odometerKm);
    drawLowBeamIcon(m_currentState.lowBeamOn, true);
    drawHighBeamIcon(m_currentState.highBeamOn, true);
}

void DisplayManager::drawStaticLayout()
{
    // Bounding Box centered horizontally in portrait
    // Width 204, Height 164 centered at X=18, Y=80
    m_tft.drawRect(18, 80, 204, 164, COLOR_TEXT_PRIMARY);
}

void DisplayManager::update(const TelemetryState &newState)
{
    if (newState == m_currentState)
        return;

    if (newState.batteryPercentage != m_currentState.batteryPercentage)
    {
        drawBatteryIndicator(newState.batteryPercentage);
    }

    if (newState.speedKmh != m_currentState.speedKmh || newState.odometerKm != m_currentState.odometerKm)
    {
        drawSpeedometerCard(newState.speedKmh, newState.odometerKm);
    }

    if (newState.lowBeamOn != m_currentState.lowBeamOn)
    {
        drawLowBeamIcon(newState.lowBeamOn);
    }

    if (newState.highBeamOn != m_currentState.highBeamOn)
    {
        drawHighBeamIcon(newState.highBeamOn);
    }

    m_currentState = newState;
}

void DisplayManager::drawBatteryIndicator(uint8_t percentage, bool forceRedraw)
{
    // Positioned in top right corner out of the way of telemetry
    int16_t w = 53;
    int16_t h = 24;
    int16_t x = 240 - w - 10; // Right aligned with 10px padding
    int16_t y = 15;
    int16_t tipW = 4;
    int16_t tipH = 10;

    m_tft.drawRect(x, y, w, h, COLOR_TEXT_PRIMARY);
    m_tft.fillRect(x + w, y + (h - tipH) / 2, tipW, tipH, COLOR_TEXT_PRIMARY);
    // Erase the inside of the battery completely before drawing new segments
    m_tft.fillRect(x + 2, y + 2, w - 4, h - 4, COLOR_BACKGROUND);

    // Explicitly clamp segments to exact thresholds to avoid integer rounding bugs
    uint8_t activeSegments = 0;
    if (percentage > 80) activeSegments = 5;
    else if (percentage > 60) activeSegments = 4;
    else if (percentage > 40) activeSegments = 3;
    else if (percentage > 20) activeSegments = 2;
    else if (percentage > 0) activeSegments = 1;
    else activeSegments = 0;

    int16_t segmentW = 8; // 5 bars * 8px + 4 gaps * 2px = 48px width inside

    for (int i = 0; i < activeSegments; ++i)
    {
        m_tft.fillRect(x + 3 + (i * (segmentW + 2)), y + 3, segmentW, h - 6, COLOR_TEXT_PRIMARY);
    }
}

void DisplayManager::drawSpeedometerCard(double speed, float odometer)
{
    m_speedSprite.fillSprite(COLOR_BACKGROUND);

    // Centered coordinates within the new 200x160 sprite structure
    m_speedSprite.setTextColor(COLOR_TEXT_PRIMARY, COLOR_BACKGROUND);
    m_speedSprite.setTextDatum(MC_DATUM);

    char speedBuffer[8];
    dtostrf(speed, 4, 1, speedBuffer);

    // Numerical readout using font 7
    m_speedSprite.drawString(speedBuffer, 100, 45, 7);

    // Unit string
    m_speedSprite.drawString("Km/h", 100, 95, 4);

    // Formatted custom odometer readout (e.g. "000 000,0")
    // Multiply by 10 and round to prevent floating point rendering inaccuracies
    long totalTenths = (long)(odometer * 10.0f + 0.5f);
    int upper = (totalTenths / 10) / 1000;
    int lower = (totalTenths / 10) % 1000;
    int decimal = totalTenths % 10;

    char odoBuffer[32];
    snprintf(odoBuffer, sizeof(odoBuffer), "%03d %03d,%d", upper % 1000, lower, decimal);
    m_speedSprite.drawString(odoBuffer, 100, 140, 2);

    // Push local canvas buffer to its portrait box matching coordinates
    m_speedSprite.pushSprite(20, 82);
}

void DisplayManager::drawLowBeamIcon(bool active, bool forceRedraw)
{
    uint16_t color = active ? COLOR_LOW_BEAM : COLOR_DIMMED;
    // Lower left flank area, below the speedometer box
    int16_t x = 15;
    int16_t y = 265;

    m_tft.fillRect(x, y - 5, 45, 30, COLOR_BACKGROUND);

    m_tft.fillRoundRect(x + 20, y, 20, 20, 10, color);
    m_tft.fillRect(x + 20, y, 10, 20, color);

    for (int i = 0; i < 3; i++)
    {
        m_tft.drawLine(x, y + (i * 8), x + 12, y + 5 + (i * 8), color);
        m_tft.drawLine(x+1, y + (i * 8), x + 13, y + 5 + (i * 8), color);
    }
}

void DisplayManager::drawHighBeamIcon(bool active, bool forceRedraw)
{
    uint16_t color = active ? COLOR_HIGH_BEAM : COLOR_DIMMED;
    // Lower right flank area, below the speedometer box
    int16_t x = 185; // Right aligned
    int16_t y = 265;

    m_tft.fillRect(x - 5, y - 5, 45, 30, COLOR_BACKGROUND);

    m_tft.fillRoundRect(x, y, 20, 20, 10, color);
    m_tft.fillRect(x + 10, y, 10, 20, color);

    for (int i = 0; i < 3; i++)
    {
        m_tft.drawLine(x + 25, y + 3 + (i * 7), x + 40, y + 3 + (i * 7), color);
        m_tft.drawLine(x + 25, y + 4 + (i * 7), x + 40, y + 4 + (i * 7), color);
    }
}
