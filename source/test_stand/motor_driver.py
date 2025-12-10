#!/usr/bin/env python3
"""
Motor Driver for DRV8833
Provides a reusable interface for controlling DC motors via DRV8833 driver chip.
Supports multiple motor instances with configurable GPIO pins.
"""

import RPi.GPIO as GPIO


class MotorController:
    """
    Controls a DC motor via DRV8833 driver chip.
    Each instance can control one motor with configurable GPIO pins.

    Example:
        motor1 = MotorController(ain1_pin=17, ain2_pin=27)
        motor2 = MotorController(ain1_pin=22, ain2_pin=23)

        motor1.forward()
        motor2.reverse()
        motor1.stop()
    """

    def __init__(self, ain1_pin, ain2_pin, mode_already_set=False):
        """
        Initialize motor controller for a single motor.

        Args:
            ain1_pin (int): BCM GPIO pin number for AIN1 (or BIN1 for motor B)
            ain2_pin (int): BCM GPIO pin number for AIN2 (or BIN2 for motor B)
            mode_already_set (bool): Set to True if GPIO.setmode() was already called
                                    elsewhere in your program (e.g., by Buttons class)
        """
        self.ain1_pin = ain1_pin
        self.ain2_pin = ain2_pin
        self._initialized = False
        self._mode_already_set = mode_already_set
        self.setup()

    def setup(self):
        """Configure GPIO pins for motor control."""
        if not self._initialized:
            # Only set mode if it hasn't been set by another component
            if not self._mode_already_set:
                GPIO.setmode(GPIO.BCM)

            GPIO.setup(self.ain1_pin, GPIO.OUT)
            GPIO.setup(self.ain2_pin, GPIO.OUT)
            self.stop()  # Start with motor stopped
            self._initialized = True

    def forward(self):
        """
        Run motor in forward direction (clockwise).
        AIN1=HIGH, AIN2=LOW
        """
        GPIO.output(self.ain1_pin, GPIO.HIGH)
        GPIO.output(self.ain2_pin, GPIO.LOW)

    def reverse(self):
        """
        Run motor in reverse direction (counter-clockwise).
        AIN1=LOW, AIN2=HIGH
        """
        GPIO.output(self.ain1_pin, GPIO.LOW)
        GPIO.output(self.ain2_pin, GPIO.HIGH)

    def stop(self):
        """
        Stop the motor (coast to a stop).
        AIN1=LOW, AIN2=LOW
        """
        GPIO.output(self.ain1_pin, GPIO.LOW)
        GPIO.output(self.ain2_pin, GPIO.LOW)

    def brake(self):
        """
        Brake the motor (active braking).
        AIN1=HIGH, AIN2=HIGH
        """
        GPIO.output(self.ain1_pin, GPIO.HIGH)
        GPIO.output(self.ain2_pin, GPIO.HIGH)

    def cleanup(self):
        """
        Clean up this motor's GPIO pins.
        Note: This only cleans up the specific pins for this motor.
        Call GPIO.cleanup() to clean up all pins.
        """
        if self._initialized:
            self.stop()
            GPIO.cleanup([self.ain1_pin, self.ain2_pin])
            self._initialized = False

    def __enter__(self):
        """Context manager entry."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup GPIO."""
        self.cleanup()
