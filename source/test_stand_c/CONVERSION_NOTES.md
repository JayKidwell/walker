# Python to C++ Conversion Notes

This document explains the key differences and design decisions in converting the test stand Python code to C++ for Raspberry Pi.

## File Mapping

| Python File | C++ Files | Notes |
|------------|-----------|-------|
| `button_driver.py` | `ButtonDriver.h`, `ButtonDriver.cpp` | Class-based driver with header/implementation split |
| `joystick_driver.py` | `JoystickController.h`, `JoystickController.cpp` | Direct I2C register access instead of Adafruit library |
| `motor_driver.py` | `MotorController.h`, `MotorController.cpp` | Hardware PWM using pigpio |
| `button_test.py` | `button_test.cpp` | Signal handler for Ctrl+C |
| `motor_test.py` | `motor_test.cpp` | Same functionality, cleaner state management |
| `joystick_test.py` | `joystick_test.cpp` | Same calibration and display logic |
| `motor_position.py` | `motor_position.cpp` | Full integration with all components |
| N/A | `CMakeLists.txt` | Build system for compilation |

## Key Technical Differences

### 1. GPIO Library: RPi.GPIO → pigpio

**Python (RPi.GPIO):**
```python
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(20, GPIO.IN, pull_up_down=GPIO.PUD_UP)
state = GPIO.input(20)
```

**C++ (pigpio):**
```cpp
#include <pigpio.h>
gpioInitialise();
gpioSetMode(20, PI_INPUT);
gpioSetPullUpDown(20, PI_PUD_UP);
int state = gpioRead(20);
```

**Why pigpio?**
- Hardware-timed PWM (more precise than RPi.GPIO software PWM)
- Better real-time performance
- C library with excellent C++ compatibility
- Preferred for robotics applications

### 2. I2C/ADC: Adafruit CircuitPython → Direct I2C

**Python (Adafruit):**
```python
import board
import busio
from adafruit_ads1x15.ads1115 import ADS1115
from adafruit_ads1x15.analog_in import AnalogIn

i2c = busio.I2C(board.SCL, board.SDA)
ads = ADS1115(i2c, address=0x48)
channel = AnalogIn(ads, 0)
value = channel.value
```

**C++ (pigpio I2C):**
```cpp
int i2c_handle = i2cOpen(1, 0x48, 0);
// Configure ADS1115 registers
uint16_t config = ADS1115_CONFIG_OS_SINGLE | ...;
i2cWriteI2CBlockData(i2c_handle, ADS1115_REG_CONFIG, data, 2);
// Read conversion result
i2cReadI2CBlockData(i2c_handle, ADS1115_REG_CONVERSION, result, 2);
```

**Why direct I2C?**
- No dependency on Python-specific libraries
- Full control over timing and configuration
- Better performance for real-time applications
- Easier to integrate with ROS

### 3. PWM Control

**Python:**
```python
pwm = GPIO.PWM(gpio_pin, 1000)  # 1000 Hz
pwm.start(0)
pwm.ChangeDutyCycle(50)  # 50%
```

**C++ (pigpio):**
```cpp
gpioSetPWMfrequency(gpio_pin, 1000);  // 1000 Hz
gpioSetPWMrange(gpio_pin, 255);       // 0-255 range
gpioPWM(gpio_pin, 128);                // 50% (128/255)
```

### 4. Context Managers → RAII

**Python:**
```python
with Buttons(pull_up=True) as buttons:
    # Use buttons
    pass
# Automatic cleanup via __exit__
```

**C++ (RAII - Resource Acquisition Is Initialization):**
```cpp
{
    Buttons buttons(true);
    // Use buttons
} // Automatic cleanup via destructor
```

### 5. Data Types

| Python | C++ | Notes |
|--------|-----|-------|
| `bool` | `bool` | Same |
| `int` | `int`, `int16_t`, `uint16_t` | Explicit sizes for hardware |
| `float` | `double` | Better precision for joystick |
| `str` | `std::string` | C++ string class |
| `list` | `std::vector` | Dynamic arrays |
| `tuple` | Pass by reference | Use function parameters |

### 6. Error Handling

**Python:**
```python
try:
    # Do something
except Exception as e:
    print(f"Error: {e}")
```

**C++ (exceptions + return codes):**
```cpp
try {
    // Do something
    if (gpioInitialise() < 0) {
        std::cerr << "Error initializing GPIO" << std::endl;
        return 1;
    }
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

### 7. Signal Handling (Ctrl+C)

**Python:**
```python
try:
    while True:
        # Main loop
except KeyboardInterrupt:
    print("Interrupted")
```

**C++ (signal handler):**
```cpp
volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signum) {
    stop_flag = 1;
}

int main() {
    signal(SIGINT, signal_handler);
    while (!stop_flag) {
        // Main loop
    }
}
```

## Performance Improvements

1. **Faster execution** - Compiled code vs interpreted Python
2. **Lower latency** - Direct hardware access, no Python overhead
3. **Deterministic timing** - Better for real-time control
4. **Lower memory usage** - No Python interpreter overhead

## ROS Integration Readiness

The C++ implementation is structured for easy ROS2 integration:

### Potential ROS2 Node Structure

```cpp
class ButtonNode : public rclcpp::Node {
    Buttons buttons_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr red_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr green_pub_;

    void timer_callback() {
        auto red_msg = std_msgs::msg::Bool();
        red_msg.data = buttons_.is_red_button_pressed();
        red_pub_->publish(red_msg);
        // ...
    }
};
```

### Suggested ROS2 Topics

- `/buttons/red` - Bool for red button state
- `/buttons/green` - Bool for green button state
- `/joystick/position` - Vector3 for X, Y, button
- `/motors/knee_x/command` - Float64 for speed (-1.0 to 1.0)
- `/motors/knee_y/command` - Float64 for speed
- `/motors/wrist/command` - Float64 for speed

## Build System: CMake

The CMakeLists.txt provides:
- Library build (`libtest_stand_drivers.a`)
- Four executable targets
- Installation rules
- Dependency management

To integrate with ROS2, replace with:
```cmake
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

ament_auto_add_executable(button_node button_node.cpp)
# ...
```

## Testing Workflow

1. **Test buttons**: `sudo ./button_test`
2. **Test motors**: `sudo ./motor_test`
3. **Calibrate joystick**: `sudo ./joystick_test`
4. **Full system test**: `sudo ./motor_position`

## Calibration Data

Unlike Python version, C++ calibration is runtime-only. For persistent calibration:

**Option 1**: Save to file
```cpp
void save_calibration(const std::string& filename) {
    std::ofstream file(filename);
    file << x_neutral << " " << y_neutral << "\n";
    file << x_min << " " << x_max << "\n";
    file << y_min << " " << y_max << "\n";
}
```

**Option 2**: ROS parameter server
```cpp
this->declare_parameter("joystick.x_neutral", 16384);
x_neutral = this->get_parameter("joystick.x_neutral").as_int();
```

## Safety Features

All programs include:
1. Signal handler for graceful shutdown (Ctrl+C)
2. Motor stop on exit
3. GPIO cleanup on exit
4. Both-button-pressed safety stop (motor_test)

## Next Steps for ROS Integration

1. Create ROS2 package structure
2. Add ROS2 dependencies to CMakeLists.txt
3. Convert each driver to a ROS2 node
4. Define message types for motor commands
5. Create launch files for system startup
6. Add parameter files for configuration
7. Implement safety monitors and watchdogs

## Compatibility

- **Raspberry Pi**: 3B, 3B+, 4, 5 (tested on 4)
- **OS**: Raspberry Pi OS (Debian-based)
- **C++ Standard**: C++11 or higher
- **CMake**: 3.10 or higher
- **pigpio**: Version 79 or higher

## Known Limitations

1. **Root required**: GPIO access needs sudo (can be fixed with udev rules)
2. **No hot-plug**: I2C device must be present at startup
3. **Single instance**: Only one process can use pigpio daemon at a time
4. **Fixed pin mapping**: Pins are hardcoded (should be configurable for ROS)

## Future Enhancements

1. Add configuration file support (YAML/JSON)
2. Add logging framework (spdlog)
3. Add unit tests (Google Test)
4. Add velocity ramping for smoother motor control
5. Add encoder support for closed-loop control
6. Add emergency stop functionality
7. Add motor current monitoring
8. Add PID controllers for position control
