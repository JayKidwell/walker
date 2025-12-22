/**
 * @file joystick_test.cpp
 * @brief Test Joystick Driver
 *
 * Displays real-time joystick position and button state.
 * Shows X position, Y position, and button value in scrolling columns.
 */

#include "JoystickController.h"
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <csignal>

volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signum) {
    stop_flag = 1;
}

int main() {
    // Setup signal handler for Ctrl+C
    signal(SIGINT, signal_handler);

    std::cout << "Starting joystick test" << std::endl;

    try {
        // Initialize joystick
        // X on ADC channel 0, Y on ADC channel 1, button on GPIO 17
        JoystickController joystick(0, 1, 17);

        std::cout << "Joystick initialized successfully" << std::endl;

        // Calibrate the joystick
        joystick.calibrate();

        std::cout << "Press Ctrl+C to exit" << std::endl;
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << std::left << std::setw(20) << "X Position"
                  << std::setw(20) << "Y Position"
                  << std::setw(20) << "Button" << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        while (!stop_flag) {
            // Read joystick values
            double x_pos = joystick.read_x();
            double y_pos = joystick.read_y();
            bool button_pressed = joystick.is_button_pressed();

            // Format button display
            std::string button_display = button_pressed ? "PRESSED" : "Released";

            // Display as scrolling columns
            std::cout << std::fixed << std::setprecision(3)
                      << std::setw(20) << x_pos
                      << std::setw(20) << y_pos
                      << std::setw(20) << button_display << std::endl;

            // Small delay to prevent excessive CPU usage and make output readable
            usleep(100000);  // 100ms
        }

        std::cout << "\nProgram interrupted by user" << std::endl;
        std::cout << "Cleanup complete" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error occurred: " << e.what() << std::endl;
        gpioTerminate();
        return 1;
    }

    // Cleanup pigpio
    gpioTerminate();

    return 0;
}
