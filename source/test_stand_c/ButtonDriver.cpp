/**
 * @file ButtonDriver.cpp
 * @brief Implementation of Button Driver for Test Stand
 */

#include "ButtonDriver.h"
#include <iostream>

Buttons::Buttons(bool pull_up)
    : _initialized(false), _pull_up(pull_up) {
    setup();
}

Buttons::~Buttons() {
    cleanup();
}

void Buttons::setup() {
    if (!_initialized) {
        // Initialize pigpio library if not already initialized
        if (gpioInitialise() < 0) {
            std::cerr << "pigpio initialization failed" << std::endl;
            return;
        }

        // Set pull mode
        int pull_mode = _pull_up ? PI_PUD_UP : PI_PUD_DOWN;

        // Configure GPIO pins as inputs with pull resistors
        gpioSetMode(RED_BUTTON, PI_INPUT);
        gpioSetPullUpDown(RED_BUTTON, pull_mode);

        gpioSetMode(GREEN_BUTTON, PI_INPUT);
        gpioSetPullUpDown(GREEN_BUTTON, pull_mode);

        _initialized = true;
    }
}

int Buttons::read_red_button() {
    return gpioRead(RED_BUTTON);
}

int Buttons::read_green_button() {
    return gpioRead(GREEN_BUTTON);
}

void Buttons::read_both(int& red_state, int& green_state) {
    red_state = read_red_button();
    green_state = read_green_button();
}

bool Buttons::is_red_button_pressed() {
    int state = read_red_button();
    return _pull_up ? (state == 0) : (state == 1);
}

bool Buttons::is_green_button_pressed() {
    int state = read_green_button();
    return _pull_up ? (state == 0) : (state == 1);
}

void Buttons::cleanup() {
    if (_initialized) {
        // pigpio cleanup is handled globally
        // Individual pin cleanup not needed
        _initialized = false;
    }
}
