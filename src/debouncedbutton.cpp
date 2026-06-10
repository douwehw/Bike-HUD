#include "debouncedbutton.h"

DebouncedButton::DebouncedButton(uint8_t pin, uint32_t debounceDelayMs)
    : m_pin(pin), m_debounceDelayMs(debounceDelayMs),
      m_stableState(LOW), m_lastRawState(LOW), m_lastTransitionTime(0),
      m_rose(false), m_fell(false) {}

void DebouncedButton::init()
{
    // Note: Your breakout board uses INPUT_PULLDOWN.
    // If it was a raw button to GND, you would use INPUT_PULLUP.
    pinMode(m_pin, INPUT_PULLUP);

    // Sample initial state
    m_stableState = digitalRead(m_pin);
    m_lastRawState = m_stableState;
}

void DebouncedButton::update()
{
    // Reset edge triggers at the start of every frame processing cycle
    m_rose = false;
    m_fell = false;

    // 1. Read the raw, noisy physical voltage on the pin
    bool currentRawState = digitalRead(m_pin);

    // 2. If the physical voltage changed (due to pressing or noise), reset the timer
    if (currentRawState != m_lastRawState)
    {
        m_lastTransitionTime = millis();
    }

    // 3. If the voltage has stayed perfectly still for longer than our window (e.g., 50ms)
    // we can officially trust that the mechanical bouncing has finished.
    if ((millis() - m_lastTransitionTime) > m_debounceDelayMs)
    {

        // If the trusted state is different from our last stable state, we have a real transition!
        if (currentRawState != m_stableState)
        {

            if (currentRawState == HIGH && m_stableState == LOW)
            {
                m_rose = true; // Clean Rising Edge Detected!
            }
            else if (currentRawState == LOW && m_stableState == HIGH)
            {
                m_fell = true; // Clean Falling Edge Detected!
            }

            m_stableState = currentRawState; // Update our official tracked state
        }
    }

    m_lastRawState = currentRawState; // Save raw state for next frame evaluation
}

bool DebouncedButton::rose() const { return m_rose; }
bool DebouncedButton::fell() const { return m_fell; }
bool DebouncedButton::isPressed() const { return m_stableState == HIGH; }
