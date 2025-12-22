/**
 * @file ButtonDriver.h
 * @brief Button Driver for Test Stand
 *
 * Provides a reusable interface for reading GPIO buttons on pins 20 and 21.
 * Uses pigpio library for GPIO control on Raspberry Pi.
 */

#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <pigpio.h>

class Buttons {
public:
    // GPIO pin configuration
    static const int RED_BUTTON = 20;      // GPIO pin 20
    static const int GREEN_BUTTON = 21;    // GPIO pin 21

    // Aliases for backward compatibility
    static const int BUTTON_PIN_20 = RED_BUTTON;
    static const int BUTTON_PIN_21 = GREEN_BUTTON;

    /**
     * @brief Initialize the button GPIO pins.
     *
     * @param pull_up If true, enables internal pull-up resistors.
     *                If false, enables pull-down resistors.
     */
    explicit Buttons(bool pull_up = true);

    /**
     * @brief Destructor - cleanup GPIO resources.
     */
    ~Buttons();

    /**
     * @brief Configure GPIO pins for reading buttons.
     */
    void setup();

    /**
     * @brief Read the current state of red button (GPIO 20).
     *
     * @return int 1 for HIGH, 0 for LOW
     */
    int read_red_button();

    /**
     * @brief Read the current state of green button (GPIO 21).
     *
     * @return int 1 for HIGH, 0 for LOW
     */
    int read_green_button();

    /**
     * @brief Read both button states at once.
     *
     * @param red_state Reference to store red button state
     * @param green_state Reference to store green button state
     */
    void read_both(int& red_state, int& green_state);

    /**
     * @brief Check if red button is pressed.
     *
     * @return bool True if pressed (LOW with pull-up), False if not pressed (HIGH)
     */
    bool is_red_button_pressed();

    /**
     * @brief Check if green button is pressed.
     *
     * @return bool True if pressed (LOW with pull-up), False if not pressed (HIGH)
     */
    bool is_green_button_pressed();

    // Backward compatibility aliases
    int read_button_20() { return read_red_button(); }
    int read_button_21() { return read_green_button(); }
    bool is_button_20_pressed() { return is_red_button_pressed(); }
    bool is_button_21_pressed() { return is_green_button_pressed(); }

    /**
     * @brief Clean up GPIO resources.
     */
    void cleanup();

private:
    bool _initialized;
    bool _pull_up;
};

#endif // BUTTON_DRIVER_H
