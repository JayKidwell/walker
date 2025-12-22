# Test Stand C++ - Motor Control for Raspberry Pi

C++ implementation of the test stand motor control system for Raspberry Pi. This code controls 4 motors based on input from buttons and a joystick, and is designed to be later integrated into ROS nodes.

## Overview

This project provides C++ drivers and test programs for:
- **Button input** - Two GPIO buttons (red and green) on pins 20 and 21
- **Joystick input** - Analog joystick via ADS1115 I2C ADC with digital button
- **Motor control** - Multiple DC motors via DRV8833 driver chips with PWM control

## Hardware Requirements

- Raspberry Pi (tested on Pi 3/4)
- DRV8833 dual motor driver chips
- ADS1115 16-bit I2C ADC
- Analog joystick module
- Push buttons (with pull-up resistors)
- DC motors

## Pin Configuration

### Buttons
- Red Button: GPIO 20
- Green Button: GPIO 21

### Joystick
- X-axis: ADS1115 Channel 0 (A0)
- Y-axis: ADS1115 Channel 1 (A1)
- Button: GPIO 17
- I2C Address: 0x48

### Motors
- Motor Knee X: GPIO 13, 16
- Motor Knee Y: GPIO 6, 12
- Motor Wrist: GPIO 4, 5

## Dependencies

### Install pigpio Library

The pigpio library provides GPIO and PWM control on Raspberry Pi:

```bash
# Install pigpio
sudo apt-get update
sudo apt-get install pigpio libpigpio-dev

# Start pigpio daemon (required for some operations)
sudo systemctl enable pigpiod
sudo systemctl start pigpiod
```

### Install CMake and Build Tools

```bash
sudo apt-get install cmake build-essential
```

## Building the Project

1. Navigate to the project directory:
```bash
cd source/test_stand_c
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Run CMake:
```bash
cmake ..
```

4. Build the project:
```bash
make
```

5. (Optional) Install to system:
```bash
sudo make install
```

## Running the Programs

All programs must be run with root privileges to access GPIO:

### Button Test
Tests the button inputs on GPIO 20 and 21:
```bash
sudo ./button_test
```
Press Ctrl+C to exit.

### Motor Test
Controls a motor with buttons (red=forward, green=reverse):
```bash
sudo ./motor_test
```
Press Ctrl+C to exit.

### Joystick Test
Displays real-time joystick position and button state:
```bash
sudo ./joystick_test
```
The program will guide you through calibration:
1. Leave joystick centered for neutral position calibration
2. Move joystick in full circles to calibrate extremes

Press Ctrl+C to exit.

### Motor Position Test
Full integration test - controls 3 motors with joystick and buttons:
```bash
sudo ./motor_position
```
- Joystick X-axis controls motor_knee_x
- Joystick Y-axis controls motor_knee_y
- Green button controls motor_wrist forward
- Red button controls motor_wrist reverse

Press Ctrl+C to exit.

## Code Structure

### Driver Classes

#### ButtonDriver (ButtonDriver.h/cpp)
- Manages GPIO button inputs on pins 20 and 21
- Supports pull-up/pull-down resistor configuration
- Provides pressed state detection

#### JoystickController (JoystickController.h/cpp)
- Reads analog X/Y axes via ADS1115 I2C ADC
- Reads digital button state
- Calibration support for neutral position and extremes
- Normalized output (-1.0 to 1.0)

#### MotorController (MotorController.h/cpp)
- Controls DC motors via DRV8833 driver chip
- PWM speed control (0-100%)
- Forward, reverse, stop, and brake functions
- Sequence execution for complex motor patterns

### Test Programs

- `button_test.cpp` - Simple button reading test
- `motor_test.cpp` - Button-controlled motor test
- `joystick_test.cpp` - Joystick calibration and reading test
- `motor_position.cpp` - Full system integration test

## Differences from Python Version

The C++ implementation maintains the same functionality as the Python version but with these adaptations:

1. **GPIO Library**: Uses `pigpio` instead of `RPi.GPIO`
   - More precise PWM control
   - Better real-time performance
   - Hardware-timed PWM

2. **I2C Communication**: Direct I2C register access instead of Adafruit CircuitPython libraries
   - Lower-level control of ADS1115
   - Manual configuration register management

3. **Memory Management**: Manual resource cleanup via destructors
   - RAII pattern for automatic cleanup
   - Explicit cleanup methods available

4. **Error Handling**: C++ exceptions instead of Python exceptions
   - Return code checking for GPIO operations
   - Signal handlers for graceful shutdown

## ROS Integration (Future)

This codebase is designed to be easily converted to ROS2 nodes:

- Each driver class can become a ROS2 node
- Test programs demonstrate the usage patterns
- Clean separation of hardware interface and logic
- Ready for topic-based communication

Recommended ROS2 node structure:
- `button_node` - Publishes button states
- `joystick_node` - Publishes joystick position
- `motor_controller_node` - Subscribes to motor commands
- `test_stand_controller` - High-level control logic

## Troubleshooting

### Permission Denied Errors
Make sure to run programs with `sudo` for GPIO access.

### I2C Not Found
Enable I2C interface:
```bash
sudo raspi-config
# Interface Options -> I2C -> Enable
```

### pigpio Initialization Failed
Start the pigpio daemon:
```bash
sudo systemctl start pigpiod
```

### Motor Not Running
- Check wiring connections
- Verify GPIO pin numbers match hardware
- Check power supply to motors
- Ensure DRV8833 is properly powered

### Joystick Reading Zeros
- Verify I2C address (default 0x48)
- Check I2C connections (SDA, SCL)
- Confirm ADS1115 power supply
- Run `i2cdetect -y 1` to see if device is detected

## License

This project is part of the walker robot control system.

## Authors

Converted from Python to C++ for ROS integration and improved real-time performance.
