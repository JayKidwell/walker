/**
 * @file MotorController.cpp
 * @brief Implementation of Motor Driver for DRV8833
 */

#include "MotorController.h"
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <stdexcept>

MotorController::MotorController(int ain1_gpio, int ain2_gpio,
                                 bool mode_already_set, int pwm_frequency)
    : ain1_gpio(ain1_gpio), ain2_gpio(ain2_gpio),
      _initialized(false), _mode_already_set(mode_already_set),
      pwm_frequency(pwm_frequency) {
    setup();
}

MotorController::~MotorController() {
    cleanup();
}

void MotorController::setup() {
    if (!_initialized) {
        // Initialize pigpio library if not already initialized
        if (!_mode_already_set) {
            if (gpioInitialise() < 0) {
                std::cerr << "pigpio initialization failed" << std::endl;
                return;
            }
        }

        // Set GPIO pins as outputs
        gpioSetMode(ain1_gpio, PI_OUTPUT);
        gpioSetMode(ain2_gpio, PI_OUTPUT);

        // Set PWM frequency
        gpioSetPWMfrequency(ain1_gpio, pwm_frequency);
        gpioSetPWMfrequency(ain2_gpio, pwm_frequency);

        // Set PWM range (0-255 for compatibility with duty cycle percentage)
        gpioSetPWMrange(ain1_gpio, 255);
        gpioSetPWMrange(ain2_gpio, 255);

        // Start with motor stopped
        gpioPWM(ain1_gpio, 0);
        gpioPWM(ain2_gpio, 0);

        _initialized = true;
    }
}

int MotorController::clamp_speed(int speed) {
    return std::max(0, std::min(100, speed));
}

void MotorController::forward(int speed) {
    speed = clamp_speed(speed);
    int duty_cycle = (speed * 255) / 100;  // Convert 0-100 to 0-255

    gpioPWM(ain1_gpio, duty_cycle);
    gpioPWM(ain2_gpio, 0);

    std::cout << "[DEBUG] Forward: GPIO" << ain1_gpio << "=" << speed
              << "% PWM, GPIO" << ain2_gpio << "=0%" << std::endl;
}

void MotorController::reverse(int speed) {
    speed = clamp_speed(speed);
    int duty_cycle = (speed * 255) / 100;  // Convert 0-100 to 0-255

    gpioPWM(ain1_gpio, 0);
    gpioPWM(ain2_gpio, duty_cycle);

    std::cout << "[DEBUG] Reverse: GPIO" << ain1_gpio << "=0%, GPIO"
              << ain2_gpio << "=" << speed << "% PWM" << std::endl;
}

void MotorController::stop() {
    gpioPWM(ain1_gpio, 0);
    gpioPWM(ain2_gpio, 0);

    std::cout << "[DEBUG] Stop: GPIO" << ain1_gpio << "=0%, GPIO"
              << ain2_gpio << "=0%" << std::endl;
}

void MotorController::brake() {
    gpioPWM(ain1_gpio, 255);
    gpioPWM(ain2_gpio, 255);

    std::cout << "[DEBUG] Brake: GPIO" << ain1_gpio << "=100%, GPIO"
              << ain2_gpio << "=100%" << std::endl;
}

void MotorController::execute_sequence(const std::vector<MotorStep>& steps) {
    std::cout << "[DEBUG] Starting sequence with " << steps.size() << " steps" << std::endl;

    for (size_t i = 0; i < steps.size(); i++) {
        const MotorStep& step = steps[i];
        const std::string& action = step.action;
        int duration_ms = step.duration_ms;
        int speed = step.speed;

        // Build debug message
        if (action == "forward" || action == "reverse") {
            std::cout << "[DEBUG] Step " << (i + 1) << "/" << steps.size()
                      << ": " << action << " at " << speed << "% for "
                      << duration_ms << "ms" << std::endl;
        } else {
            std::cout << "[DEBUG] Step " << (i + 1) << "/" << steps.size()
                      << ": " << action << " for " << duration_ms << "ms" << std::endl;
        }

        // Execute the action
        if (action == "forward") {
            forward(speed);
        } else if (action == "reverse") {
            reverse(speed);
        } else if (action == "stop") {
            stop();
        } else {
            throw std::invalid_argument("Unknown action '" + action +
                                       "'. Valid actions: forward, reverse, stop");
        }

        // Wait for the specified duration
        if (duration_ms > 0) {
            usleep(duration_ms * 1000);  // Convert ms to microseconds
        }
    }

    std::cout << "[DEBUG] Sequence complete" << std::endl;
}

void MotorController::cleanup() {
    if (_initialized) {
        // Stop motor
        stop();

        // Set pins back to input mode
        gpioSetMode(ain1_gpio, PI_INPUT);
        gpioSetMode(ain2_gpio, PI_INPUT);

        _initialized = false;
    }
}
