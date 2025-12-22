/**
 * @file button_test.cpp
 * @brief GPIO Button Test Program
 *
 * Reads GPIO pins 20 and 21 on Raspberry Pi and displays their states.
 * Uses the Buttons driver for reusable GPIO functionality.
 */

#include "ButtonDriver.h"
#include <iostream>
#include <unistd.h>
#include <csignal>

volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signum) {
    stop_flag = 1;
}

int main() {
    // Setup signal handler for Ctrl+C
    signal(SIGINT, signal_handler);

    try {
        Buttons buttons(true);  // Use pull-up resistors

        std::cout << "Reading Red Button (GPIO 20) and Green Button (GPIO 21). Press Ctrl+C to exit." << std::endl;
        std::cout << std::string(60, '-') << std::endl;

        while (!stop_flag) {
            // Read button states
            int red_button_state, green_button_state;
            buttons.read_both(red_button_state, green_button_state);

            // Display the values
            std::cout << "Red Button: " << red_button_state
                      << "  |  Green Button: " << green_button_state << std::endl;

            // Small delay to avoid overwhelming the output
            usleep(100000);  // 100ms
        }

        std::cout << "\n\nExiting program..." << std::endl;
        std::cout << "GPIO cleanup complete." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Cleanup pigpio
    gpioTerminate();

    return 0;
}
