#ifndef DEBOUNCED_BUTTON_H
#define DEBOUNCED_BUTTON_H

#include <Arduino.h>

class DebouncedButton
{
public:
    // Constructor takes physical pin and the debounce window in milliseconds
    DebouncedButton(uint8_t pin, uint32_t debounceDelayMs = 50);

    void init();
    void update(); // Must be called continuously every loop iteration

    bool rose() const;      // Returns true ONLY on the exact frame the button is pressed (Rising Edge)
    bool fell() const;      // Returns true ONLY on the exact frame the button is released (Falling Edge)
    bool isPressed() const; // Returns the stable, filtered current state of the button

private:
    uint8_t m_pin;
    uint32_t m_debounceDelayMs;

    bool m_stableState;                 // The current filtered state of the button
    bool m_lastRawState;                // The raw un-debounced state from the previous frame
    unsigned long m_lastTransitionTime; // Timestamp of when the raw pin last changed

    bool m_rose; // Edge trigger flags
    bool m_fell;
};

#endif // DEBOUNCED_BUTTON_H
