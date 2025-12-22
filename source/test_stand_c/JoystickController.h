/**
 * @file JoystickController.h
 * @brief Joystick Driver for ADS1115
 *
 * Provides a reusable interface for reading joystick inputs via ADS1115 ADC.
 * Reads X-axis, Y-axis, and button state from a joystick module.
 * Uses ADS1115 library for I2C ADC communication on Raspberry Pi.
 */

#ifndef JOYSTICK_CONTROLLER_H
#define JOYSTICK_CONTROLLER_H

#include <pigpio.h>

class JoystickController {
public:
    /**
     * @brief Initialize joystick controller.
     *
     * @param x_channel ADS1115 channel for X-axis (0-3, default 0)
     * @param y_channel ADS1115 channel for Y-axis (0-3, default 1)
     * @param button_gpio BCM GPIO pin for button (default 17)
     * @param i2c_address I2C address of ADS1115 (default 0x48)
     * @param mode_already_set Set to true if pigpio was already initialized (default false)
     */
    explicit JoystickController(int x_channel = 0,
                                int y_channel = 1,
                                int button_gpio = 17,
                                int i2c_address = 0x48,
                                bool mode_already_set = false);

    /**
     * @brief Destructor - cleanup resources.
     */
    ~JoystickController();

    /**
     * @brief Read raw ADC value from X-axis.
     *
     * @return int Raw ADC value (signed 16-bit for ADS1115)
     */
    int read_x_raw();

    /**
     * @brief Read raw ADC value from Y-axis.
     *
     * @return int Raw ADC value (signed 16-bit for ADS1115)
     */
    int read_y_raw();

    /**
     * @brief Read raw GPIO value from button input.
     *
     * @return int GPIO state (0 for LOW/pressed, 1 for HIGH/not pressed with pull-up)
     */
    int read_button_raw();

    /**
     * @brief Calibrate the joystick to determine neutral position and extremes.
     *
     * @return bool True if calibration successful
     */
    bool calibrate();

    /**
     * @brief Read X-axis position as normalized value.
     *
     * @return double X position normalized to -1.0 to 1.0 range (0.0 at neutral)
     */
    double read_x();

    /**
     * @brief Read Y-axis position as normalized value.
     *
     * @return double Y position normalized to -1.0 to 1.0 range (0.0 at neutral)
     */
    double read_y();

    /**
     * @brief Check if joystick button is pressed.
     *
     * @return bool True if button is pressed, False otherwise
     */
    bool is_button_pressed();

    /**
     * @brief Read all joystick inputs at once.
     *
     * @param x_value Reference to store normalized X position (-1.0 to 1.0)
     * @param y_value Reference to store normalized Y position (-1.0 to 1.0)
     * @param button_pressed Reference to store button state
     */
    void read_all(double& x_value, double& y_value, bool& button_pressed);

    /**
     * @brief Read all joystick inputs as raw values.
     *
     * @param x_raw Reference to store raw X ADC value
     * @param y_raw Reference to store raw Y ADC value
     * @param button_raw Reference to store raw button GPIO value
     */
    void read_all_raw(int& x_raw, int& y_raw, int& button_raw);

    /**
     * @brief Clean up resources.
     */
    void cleanup();

private:
    int x_channel;
    int y_channel;
    int button_gpio;
    int i2c_handle;
    bool _initialized;
    bool _mode_already_set;

    // Calibration values
    int x_neutral;
    int y_neutral;
    int x_min;
    int x_max;
    int y_min;
    int y_max;
    bool _calibrated;

    // Helper methods
    int read_adc_channel(int channel);
    double clamp(double value, double min_val, double max_val);
};

#endif // JOYSTICK_CONTROLLER_H
