cmake_minimum_required(VERSION 3.10)
project(TestStandC)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Find pigpio library
find_library(PIGPIO_LIBRARY pigpio REQUIRED)
find_library(PIGPIOD_IF2_LIBRARY pigpiod_if2)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Source files for the drivers
set(DRIVER_SOURCES
    ButtonDriver.cpp
    MotorController.cpp
    JoystickController.cpp
)

# Create library from driver sources
add_library(test_stand_drivers STATIC ${DRIVER_SOURCES})
target_link_libraries(test_stand_drivers ${PIGPIO_LIBRARY})

# Build button_test executable
add_executable(button_test button_test.cpp)
target_link_libraries(button_test test_stand_drivers ${PIGPIO_LIBRARY} pthread)

# Build motor_test executable
add_executable(motor_test motor_test.cpp)
target_link_libraries(motor_test test_stand_drivers ${PIGPIO_LIBRARY} pthread)

# Build joystick_test executable
add_executable(joystick_test joystick_test.cpp)
target_link_libraries(joystick_test test_stand_drivers ${PIGPIO_LIBRARY} pthread)

# Build motor_position executable
add_executable(motor_position motor_position.cpp)
target_link_libraries(motor_position test_stand_drivers ${PIGPIO_LIBRARY} pthread)

# Installation rules
install(TARGETS button_test motor_test joystick_test motor_position
    RUNTIME DESTINATION bin
)

install(FILES
    ButtonDriver.h
    MotorController.h
    JoystickController.h
    DESTINATION include/test_stand
)

install(TARGETS test_stand_drivers
    ARCHIVE DESTINATION lib
)

# Print build information
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
