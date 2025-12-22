/**
 * @file MotorController.h
 * @brief Motor Driver for DRV8833
 *
 * Provides a reusable interface for controlling DC motors via DRV8833 driver chip.
 * Supports multiple motor instances with configurable GPIO pins.
 * Uses pigpio library for PWM control on Raspberry Pi.
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <pigpio.h>
#include <vector>
#include <string>

struct MotorStep {
    std::string action;  // "forward", "reverse", "stop"
    int duration_ms;     // Duration in milliseconds
    int speed;           // Speed 0-100 (optional, default 50)

    MotorStep(const std::string& act, int dur, int spd = 50)
        : action(act), duration_ms(dur), speed(spd) {}
};

class MotorController {
public:
    /**
     * @brief Initialize motor controller for a single motor.
     *
     * @param ain1_gpio BCM GPIO pin number for AIN1 (or BIN1 for motor B)
     * @param ain2_gpio BCM GPIO pin number for AIN2 (or BIN2 for motor B)
     * @param mode_already_set Set to true if pigpio was already initialized
     * @param pwm_frequency PWM frequency in Hz (default 1000 Hz)
     */
    explicit MotorController(int ain1_gpio,
                             int ain2_gpio,
                             bool mode_already_set = false,
                             int pwm_frequency = 1000);

    /**
     * @brief Destructor - cleanup GPIO.
     */
    ~MotorController();

    /**
     * @brief Configure GPIO pins for motor control with PWM.
     */
    void setup();

    /**
     * @brief Run motor in forward direction (clockwise).
     *
     * @param speed Motor speed from 0-100 (default 50)
     */
    void forward(int speed = 50);

    /**
     * @brief Run motor in reverse direction (counter-clockwise).
     *
     * @param speed Motor speed from 0-100 (default 50)
     */
    void reverse(int speed = 50);

    /**
     * @brief Stop the motor (coast to a stop).
     */
    void stop();

    /**
     * @brief Brake the motor (active braking).
     */
    void brake();

    /**
     * @brief Execute a sequence of motor commands.
     *
     * @param steps Vector of MotorStep objects to execute
     */
    void execute_sequence(const std::vector<MotorStep>& steps);

    /**
     * @brief Clean up this motor's GPIO pins.
     */
    void cleanup();

private:
    int ain1_gpio;
    int ain2_gpio;
    bool _initialized;
    bool _mode_already_set;
    int pwm_frequency;

    int clamp_speed(int speed);
};

#endif // MOTOR_CONTROLLER_H
