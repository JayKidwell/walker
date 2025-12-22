/**
 * @file JoystickController.cpp
 * @brief Implementation of Joystick Driver for ADS1115
 */

#include "JoystickController.h"
#include <iostream>
#include <unistd.h>
#include <cmath>
#include <algorithm>

// ADS1115 Register addresses
#define ADS1115_REG_CONVERSION 0x00
#define ADS1115_REG_CONFIG     0x01

// ADS1115 Configuration bits
#define ADS1115_CONFIG_OS_SINGLE    0x8000  // Start single conversion
#define ADS1115_CONFIG_MUX_OFFSET   12      // Multiplexer offset
#define ADS1115_CONFIG_PGA_4_096V   0x0200  // +/- 4.096V range
#define ADS1115_CONFIG_MODE_SINGLE  0x0100  // Single-shot mode
#define ADS1115_CONFIG_DR_128SPS    0x0080  // 128 samples per second
#define ADS1115_CONFIG_COMP_DIS     0x0003  // Disable comparator

JoystickController::JoystickController(int x_channel, int y_channel,
                                       int button_gpio, int i2c_address,
                                       bool mode_already_set)
    : x_channel(x_channel), y_channel(y_channel), button_gpio(button_gpio),
      i2c_handle(-1), _initialized(false), _mode_already_set(mode_already_set),
      x_neutral(16384), y_neutral(16384),
      x_min(0), x_max(32767), y_min(0), y_max(32767),
      _calibrated(false) {

    // Initialize pigpio library if not already initialized
    if (!mode_already_set) {
        if (gpioInitialise() < 0) {
            std::cerr << "pigpio initialization failed" << std::endl;
            return;
        }
    }

    // Open I2C connection to ADS1115
    i2c_handle = i2cOpen(1, i2c_address, 0);  // I2C bus 1, default address 0x48
    if (i2c_handle < 0) {
        std::cerr << "Failed to open I2C connection to ADS1115" << std::endl;
        return;
    }

    // Setup GPIO for button with internal pull-up
    gpioSetMode(button_gpio, PI_INPUT);
    gpioSetPullUpDown(button_gpio, PI_PUD_UP);

    _initialized = true;
    std::cout << "[DEBUG] Joystick initialized: X=A" << x_channel
              << ", Y=A" << y_channel
              << ", Button=GPIO" << button_gpio << std::endl;
}

JoystickController::~JoystickController() {
    cleanup();
}

int JoystickController::read_adc_channel(int channel) {
    if (i2c_handle < 0) {
        return 0;
    }

    // Build configuration word
    uint16_t config = ADS1115_CONFIG_OS_SINGLE |                    // Start conversion
                      ((4 + channel) << ADS1115_CONFIG_MUX_OFFSET) | // Single-ended input
                      ADS1115_CONFIG_PGA_4_096V |                    // +/- 4.096V range
                      ADS1115_CONFIG_MODE_SINGLE |                   // Single-shot mode
                      ADS1115_CONFIG_DR_128SPS |                     // 128 SPS
                      ADS1115_CONFIG_COMP_DIS;                       // Disable comparator

    // Write configuration to config register
    uint8_t config_bytes[2];
    config_bytes[0] = (config >> 8) & 0xFF;
    config_bytes[1] = config & 0xFF;

    i2cWriteI2CBlockData(i2c_handle, ADS1115_REG_CONFIG, (char*)config_bytes, 2);

    // Wait for conversion to complete (128 SPS = ~8ms per sample)
    usleep(10000);  // 10ms

    // Read conversion result
    uint8_t result_bytes[2];
    i2cReadI2CBlockData(i2c_handle, ADS1115_REG_CONVERSION, (char*)result_bytes, 2);

    // Combine bytes into 16-bit signed value
    int16_t result = (result_bytes[0] << 8) | result_bytes[1];

    // Convert to unsigned 0-32767 range (matching Python behavior)
    return (result < 0) ? 0 : result;
}

int JoystickController::read_x_raw() {
    return read_adc_channel(x_channel);
}

int JoystickController::read_y_raw() {
    return read_adc_channel(y_channel);
}

int JoystickController::read_button_raw() {
    return gpioRead(button_gpio);
}

bool JoystickController::calibrate() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "JOYSTICK CALIBRATION" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    // Step 1: Capture neutral position
    std::cout << "\nStep 1: NEUTRAL POSITION" << std::endl;
    std::cout << "Leave the joystick in the center/neutral position." << std::endl;
    std::cout << "Calibrating in 3 seconds..." << std::endl;
    sleep(1);
    std::cout << "2..." << std::endl;
    sleep(1);
    std::cout << "1..." << std::endl;
    sleep(1);

    // Sample neutral position multiple times for accuracy
    std::cout << "Sampling neutral position..." << std::endl;
    int x_sum = 0, y_sum = 0;
    const int samples = 20;
    for (int i = 0; i < samples; i++) {
        x_sum += read_x_raw();
        y_sum += read_y_raw();
        usleep(50000);  // 50ms
    }

    x_neutral = x_sum / samples;
    y_neutral = y_sum / samples;
    std::cout << "Neutral position captured: X=" << x_neutral
              << ", Y=" << y_neutral << std::endl;

    // Step 2: Capture extremes
    std::cout << "\nStep 2: EXTREME POSITIONS" << std::endl;
    std::cout << "Move the joystick in a FULL CIRCLE around all edges" << std::endl;
    std::cout << "and to all corners (up, down, left, right, diagonals)." << std::endl;
    std::cout << "Starting in 3 seconds..." << std::endl;
    sleep(1);
    std::cout << "2..." << std::endl;
    sleep(1);
    std::cout << "1..." << std::endl;
    sleep(1);
    std::cout << "GO! Move the joystick to all extreme positions..." << std::endl;
    std::cout << "You have 10 seconds..." << std::endl;

    // Track min/max values
    int x_min_temp = x_neutral;
    int x_max_temp = x_neutral;
    int y_min_temp = y_neutral;
    int y_max_temp = y_neutral;

    // Sample for 10 seconds
    const int duration = 10;  // seconds
    const int sample_delay = 50000;  // 50ms in microseconds
    const int total_samples = (duration * 1000000) / sample_delay;

    for (int i = 0; i < total_samples; i++) {
        int x_raw = read_x_raw();
        int y_raw = read_y_raw();

        x_min_temp = std::min(x_min_temp, x_raw);
        x_max_temp = std::max(x_max_temp, x_raw);
        y_min_temp = std::min(y_min_temp, y_raw);
        y_max_temp = std::max(y_max_temp, y_raw);

        usleep(sample_delay);
    }

    x_min = x_min_temp;
    x_max = x_max_temp;
    y_min = y_min_temp;
    y_max = y_max_temp;
    _calibrated = true;

    std::cout << "\nCalibration complete! (" << total_samples << " samples)" << std::endl;
    std::cout << "X: min=" << x_min << ", neutral=" << x_neutral << ", max=" << x_max << std::endl;
    std::cout << "Y: min=" << y_min << ", neutral=" << y_neutral << ", max=" << y_max << std::endl;
    std::cout << std::string(60, '=') << "\n" << std::endl;

    return true;
}

double JoystickController::clamp(double value, double min_val, double max_val) {
    return std::max(min_val, std::min(max_val, value));
}

double JoystickController::read_x() {
    int raw = read_x_raw();

    if (!_calibrated) {
        // Use default calibration
        double normalized = (raw - 16384) / 16384.0;
        return clamp(normalized, -1.0, 1.0);
    }

    // Use calibrated values
    double normalized;
    if (raw < x_neutral) {
        // Negative range
        if (x_neutral == x_min) {
            return 0.0;
        }
        normalized = (double)(raw - x_neutral) / (double)(x_neutral - x_min);
    } else {
        // Positive range
        if (x_max == x_neutral) {
            return 0.0;
        }
        normalized = (double)(raw - x_neutral) / (double)(x_max - x_neutral);
    }

    return clamp(normalized, -1.0, 1.0);
}

double JoystickController::read_y() {
    int raw = read_y_raw();

    if (!_calibrated) {
        // Use default calibration
        double normalized = (raw - 16384) / 16384.0;
        return clamp(normalized, -1.0, 1.0);
    }

    // Use calibrated values
    double normalized;
    if (raw < y_neutral) {
        // Negative range
        if (y_neutral == y_min) {
            return 0.0;
        }
        normalized = (double)(raw - y_neutral) / (double)(y_neutral - y_min);
    } else {
        // Positive range
        if (y_max == y_neutral) {
            return 0.0;
        }
        normalized = (double)(raw - y_neutral) / (double)(y_max - y_neutral);
    }

    return clamp(normalized, -1.0, 1.0);
}

bool JoystickController::is_button_pressed() {
    return read_button_raw() == 0;
}

void JoystickController::read_all(double& x_value, double& y_value, bool& button_pressed) {
    x_value = read_x();
    y_value = read_y();
    button_pressed = is_button_pressed();
}

void JoystickController::read_all_raw(int& x_raw, int& y_raw, int& button_raw) {
    x_raw = read_x_raw();
    y_raw = read_y_raw();
    button_raw = read_button_raw();
}

void JoystickController::cleanup() {
    if (_initialized) {
        if (i2c_handle >= 0) {
            i2cClose(i2c_handle);
            i2c_handle = -1;
        }
        std::cout << "[DEBUG] Joystick cleanup complete" << std::endl;
        _initialized = false;
    }
}
