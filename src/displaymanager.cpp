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
    m_tft.setRotation(1); // 1 = Landscape (320x240)
    m_tft.fillScreen(COLOR_BACKGROUND);

    // Sprite shrunk down to conserve RAM and fit the narrower 240px canvas height
    m_speedSprite.createSprite(180, 130);
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
    // Bounding Box centered horizontally in landscape
    // Width 184, Height 134 centered at X=68, Y=78
    m_tft.drawRect(68, 78, 184, 134, COLOR_TEXT_PRIMARY);
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
    int16_t x = 240;
    int16_t y = 15;
    int16_t w = 50;
    int16_t h = 24;
    int16_t tipW = 4;
    int16_t tipH = 10;

    m_tft.drawRect(x, y, w, h, COLOR_TEXT_PRIMARY);
    m_tft.fillRect(x + w, y + (h - tipH) / 2, tipW, tipH, COLOR_TEXT_PRIMARY);
    m_tft.fillRect(x + 2, y + 2, w - 4, h - 4, COLOR_BACKGROUND);

    uint8_t activeSegments = (percentage + 12) / 25;
    int16_t segmentW = (w - 11) / 4;

    for (int i = 0; i < activeSegments; ++i)
    {
        m_tft.fillRect(x + 3 + (i * (segmentW + 2)), y + 3, segmentW, h - 6, COLOR_TEXT_PRIMARY);
    }
}

void DisplayManager::drawSpeedometerCard(double speed, uint32_t odometer)
{
    m_speedSprite.fillSprite(COLOR_BACKGROUND);

    // Centered coordinates within the smaller 180x130 sprite structure
    m_speedSprite.setTextColor(COLOR_TEXT_PRIMARY, COLOR_BACKGROUND);
    m_speedSprite.setTextDatum(MC_DATUM);

    char speedBuffer[8];
    dtostrf(speed, 4, 1, speedBuffer);

    // Numerical readout using font 7
    m_speedSprite.drawString(speedBuffer, 90, 35, 7);

    // Unit string
    m_speedSprite.drawString("Km/h", 90, 80, 4);

    // Small odometer readout
    String odoStr = "ODO: " + String(odometer) + " km";
    m_speedSprite.drawString(odoStr, 90, 110, 2);

    // Push local canvas buffer to its center landscape box matching coordinates
    m_speedSprite.pushSprite(70, 80);
}

void DisplayManager::drawLowBeamIcon(bool active, bool forceRedraw)
{
    uint16_t color = active ? COLOR_LOW_BEAM : COLOR_DIMMED;
    // Lower left flank area
    int16_t x = 12;
    int16_t y = 130;

    m_tft.fillRect(x, y - 5, 45, 30, COLOR_BACKGROUND);

    m_tft.fillRoundRect(x + 20, y, 20, 20, 10, color);
    m_tft.fillRect(x + 20, y, 10, 20, color);

    for (int i = 0; i < 3; i++)
    {
        m_tft.drawLine(x, y + (i * 8), x + 12, y + 5 + (i * 8), color);
    }
}

void DisplayManager::drawHighBeamIcon(bool active, bool forceRedraw)
{
    uint16_t color = active ? COLOR_HIGH_BEAM : COLOR_DIMMED;
    // Lower right flank area
    int16_t x = 265;
    int16_t y = 130;

    m_tft.fillRect(x - 5, y - 5, 45, 30, COLOR_BACKGROUND);

    m_tft.fillRoundRect(x, y, 20, 20, 10, color);
    m_tft.fillRect(x + 10, y, 10, 20, color);

    for (int i = 0; i < 3; i++)
    {
        m_tft.drawLine(x + 25, y + 3 + (i * 7), x + 40, y + 3 + (i * 7), color);
    }
}
