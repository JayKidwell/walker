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
        motor1 = MotorController(ain1_gpio=17, ain2_gpio=27)
        motor2 = MotorController(ain1_gpio=22, ain2_gpio=23)

        motor1.forward()
        motor2.reverse()
        motor1.stop()
    """

    def __init__(self, ain1_gpio, ain2_gpio, mode_already_set=False, pwm_frequency=1000):
        """
        Initialize motor controller for a single motor.

        Args:
            ain1_gpio (int): BCM GPIO pin number for AIN1 (or BIN1 for motor B)
            ain2_gpio (int): BCM GPIO pin number for AIN2 (or BIN2 for motor B)
            mode_already_set (bool): Set to True if GPIO.setmode() was already called
                                    elsewhere in your program (e.g., by Buttons class)
            pwm_frequency (int): PWM frequency in Hz (default 1000 Hz)
        """
        self.ain1_gpio = ain1_gpio
        self.ain2_gpio = ain2_gpio
        self._initialized = False
        self._mode_already_set = mode_already_set
        self.pwm_frequency = pwm_frequency
        self.pwm1 = None
        self.pwm2 = None
        self.setup()

    def setup(self):
        """Configure GPIO pins for motor control with PWM."""
        if not self._initialized:
            # Only set mode if it hasn't been set by another component
            if not self._mode_already_set:
                GPIO.setmode(GPIO.BCM)

            GPIO.setup(self.ain1_gpio, GPIO.OUT)
            GPIO.setup(self.ain2_gpio, GPIO.OUT)

            # Create PWM objects
            self.pwm1 = GPIO.PWM(self.ain1_gpio, self.pwm_frequency)
            self.pwm2 = GPIO.PWM(self.ain2_gpio, self.pwm_frequency)

            # Start PWM at 0% duty cycle (stopped)
            self.pwm1.start(0)
            self.pwm2.start(0)

            self._initialized = True

    def forward(self):
        """
        Run motor in forward direction (clockwise) at 50% speed.
        AIN1=50% PWM, AIN2=0%
        """
        self.pwm1.ChangeDutyCycle(50)
        self.pwm2.ChangeDutyCycle(0)
        print(f"[DEBUG] Forward: GPIO{self.ain1_gpio}=50% PWM, GPIO{self.ain2_gpio}=0%")

    def reverse(self):
        """
        Run motor in reverse direction (counter-clockwise) at 50% speed.
        AIN1=0%, AIN2=50% PWM
        """
        self.pwm1.ChangeDutyCycle(0)
        self.pwm2.ChangeDutyCycle(50)
        print(f"[DEBUG] Reverse: GPIO{self.ain1_gpio}=0%, GPIO{self.ain2_gpio}=50% PWM")

    def stop(self):
        """
        Stop the motor (coast to a stop).
        AIN1=0%, AIN2=0%
        """
        self.pwm1.ChangeDutyCycle(0)
        self.pwm2.ChangeDutyCycle(0)
        print(f"[DEBUG] Stop: GPIO{self.ain1_gpio}=0%, GPIO{self.ain2_gpio}=0%")

    def brake(self):
        """
        Brake the motor (active braking).
        AIN1=100%, AIN2=100%
        """
        self.pwm1.ChangeDutyCycle(100)
        self.pwm2.ChangeDutyCycle(100)
        print(f"[DEBUG] Brake: GPIO{self.ain1_gpio}=100%, GPIO{self.ain2_gpio}=100%")

    def cleanup(self):
        """
        Clean up this motor's GPIO pins.
        Note: This only cleans up the specific pins for this motor.
        Call GPIO.cleanup() to clean up all pins.
        """
        if self._initialized:
            # Stop PWM
            if self.pwm1:
                self.pwm1.stop()
            if self.pwm2:
                self.pwm2.stop()

            # Clean up GPIO pins
            GPIO.cleanup([self.ain1_gpio, self.ain2_gpio])
            self._initialized = False

    def __enter__(self):
        """Context manager entry."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup GPIO."""
        self.cleanup()
