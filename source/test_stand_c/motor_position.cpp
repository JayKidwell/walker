/**
 * @file motor_position.cpp
 * @brief Test Motor Control with Joystick
 *
 * Uses joystick to control three DC motors for test arm joints via DRV8833.
 */

#include "JoystickController.h"
#include "MotorController.h"
#include "ButtonDriver.h"
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <cmath>

volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signum) {
    stop_flag = 1;
}

int main() {
    // Setup signal handler for Ctrl+C
    signal(SIGINT, signal_handler);

    std::cout << "Starting joystick-controlled motor test" << std::endl;

    try {
        // Initialize joystick
        JoystickController joystick(0, 1, 17);

        // Calibrate the joystick
        std::cout << "Calibrating joystick..." << std::endl;
        joystick.calibrate();

        // Initialize buttons
        // mode_already_set=true because JoystickController already initialized pigpio
        Buttons buttons(true);

        // Initialize three motors
        // mode_already_set=true because JoystickController already initialized pigpio
        MotorController motor_knee_x(13, 16, true);
        MotorController motor_knee_y(6, 12, true);
        MotorController motor_wrist(4, 5, true);

        std::cout << "Motors initialized:" << std::endl;
        std::cout << "  motor_knee_x:  GPIO 13/16" << std::endl;
        std::cout << "  motor_knee_y:  GPIO 6/12" << std::endl;
        std::cout << "  motor_wrist:   GPIO 4/5" << std::endl;

        std::cout << "Joystick X controls motor_knee_x" << std::endl;
        std::cout << "Joystick Y controls motor_knee_y" << std::endl;
        std::cout << "Green button controls motor_wrist forward at 20%" << std::endl;
        std::cout << "Red button controls motor_wrist reverse at 20%" << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;

        int last_x_speed = 0;
        int last_y_speed = 0;
        bool last_green_pressed = false;
        bool last_red_pressed = false;

        while (!stop_flag) {
            // Read joystick position
            double x_pos = joystick.read_x();  // -1.0 to 1.0
            double y_pos = joystick.read_y();  // -1.0 to 1.0

            // Map joystick position to motor speed
            // At neutral (0.0), motor stops
            // At full deflection (±1.0), motor runs at specified speed
            int x_speed = static_cast<int>(x_pos * 20);  // Maps -1.0 to 1.0 => -20 to 20
            int y_speed = static_cast<int>(y_pos * 40);  // Maps -1.0 to 1.0 => -40 to 40

            // Control motor_knee_x based on joystick X (reversed direction)
            if (x_speed != last_x_speed) {
                if (x_speed > 0) {
                    motor_knee_x.reverse(std::abs(x_speed));
                    std::cout << "motor_knee_x: REVERSE at " << std::abs(x_speed)
                              << "% (X=" << std::fixed << std::setprecision(2) << x_pos << ")" << std::endl;
                } else if (x_speed < 0) {
                    motor_knee_x.forward(std::abs(x_speed));
                    std::cout << "motor_knee_x: FORWARD at " << std::abs(x_speed)
                              << "% (X=" << std::fixed << std::setprecision(2) << x_pos << ")" << std::endl;
                } else {
                    motor_knee_x.stop();
                    std::cout << "motor_knee_x: STOP (X=0.00)" << std::endl;
                }
                last_x_speed = x_speed;
            }

            // Control motor_knee_y based on joystick Y
            if (y_speed != last_y_speed) {
                if (y_speed > 0) {
                    motor_knee_y.forward(std::abs(y_speed));
                    std::cout << "motor_knee_y: FORWARD at " << std::abs(y_speed)
                              << "% (Y=" << std::fixed << std::setprecision(2) << y_pos << ")" << std::endl;
                } else if (y_speed < 0) {
                    motor_knee_y.reverse(std::abs(y_speed));
                    std::cout << "motor_knee_y: REVERSE at " << std::abs(y_speed)
                              << "% (Y=" << std::fixed << std::setprecision(2) << y_pos << ")" << std::endl;
                } else {
                    motor_knee_y.stop();
                    std::cout << "motor_knee_y: STOP (Y=0.00)" << std::endl;
                }
                last_y_speed = y_speed;
            }

            // Read button states
            bool green_pressed = buttons.is_green_button_pressed();
            bool red_pressed = buttons.is_red_button_pressed();

            // Control motor_wrist based on buttons
            if (green_pressed != last_green_pressed) {
                if (green_pressed) {
                    motor_wrist.forward(20);
                    std::cout << "motor_wrist: FORWARD at 20% (Green button pressed)" << std::endl;
                } else {
                    motor_wrist.stop();
                    std::cout << "motor_wrist: STOP (Green button released)" << std::endl;
                }
                last_green_pressed = green_pressed;
            }

            if (red_pressed != last_red_pressed) {
                if (red_pressed) {
                    motor_wrist.reverse(20);
                    std::cout << "motor_wrist: REVERSE at 20% (Red button pressed)" << std::endl;
                } else {
                    motor_wrist.stop();
                    std::cout << "motor_wrist: STOP (Red button released)" << std::endl;
                }
                last_red_pressed = red_pressed;
            }

            // Small delay to prevent excessive CPU usage
            usleep(50000);  // 50ms
        }

        std::cout << "\nProgram interrupted by user" << std::endl;

        // Clean up - stop all motors
        std::cout << "Stopping all motors..." << std::endl;
        motor_knee_x.stop();
        motor_knee_y.stop();
        motor_wrist.stop();

        std::cout << "Cleanup complete" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        gpioTerminate();
        return 1;
    }

    // Cleanup pigpio
    gpioTerminate();

    return 0;
}
