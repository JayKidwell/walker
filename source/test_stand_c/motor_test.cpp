/**
 * @file motor_test.cpp
 * @brief Test Motor Control with Buttons
 *
 * Uses red and green buttons to control DC motor direction via DRV8833.
 */

#include "ButtonDriver.h"
#include "MotorController.h"
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <cstdlib>

volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signum) {
    stop_flag = 1;
}

int main() {
    // Setup signal handler for Ctrl+C
    signal(SIGINT, signal_handler);

    std::cout << "Starting button-controlled motor test" << std::endl;

    try {
        // Initialize components
        Buttons buttons(true);  // pull_up=true
        // mode_already_set=true because Buttons class already initialized pigpio
        MotorController motor(13, 16, true);  // GPIO 13/16

        std::cout << "Press RED button for forward, GREEN button for reverse" << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;

        std::string last_state = "stopped";

        while (!stop_flag) {
            bool red_pressed = buttons.is_red_button_pressed();
            bool green_pressed = buttons.is_green_button_pressed();

            if (red_pressed && !green_pressed) {
                if (last_state != "forward") {
                    motor.forward(15);
                    std::cout << "Motor running FORWARD at 15% speed (red button pressed)" << std::endl;
                    last_state = "forward";
                }
            }
            else if (green_pressed && !red_pressed) {
                if (last_state != "reverse") {
                    motor.reverse(15);
                    std::cout << "Motor running REVERSE at 15% speed (green button pressed)" << std::endl;
                    last_state = "reverse";
                }
            }
            else if (red_pressed && green_pressed) {
                // Both buttons pressed - stop for safety
                if (last_state != "stopped") {
                    motor.stop();
                    std::cout << "WARNING: Both buttons pressed - motor STOPPED" << std::endl;
                    last_state = "stopped";
                }
            }
            else {
                // No buttons pressed
                if (last_state != "stopped") {
                    motor.stop();
                    std::cout << "No buttons pressed - motor STOPPED" << std::endl;
                    last_state = "stopped";
                }
            }

            // Small delay to prevent excessive CPU usage
            usleep(50000);  // 50ms
        }

        std::cout << "\nProgram interrupted by user" << std::endl;

        // Clean up
        motor.stop();
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
