#!/usr/bin/env python3
"""
Motor Driver for DRV8833
Provides a reusable interface for controlling DC motors via DRV8833 driver chip.
Supports multiple motor instances with configurable GPIO pins.
"""

import time
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

    def forward(self, speed=50):
        """
        Run motor in forward direction (clockwise).

        Args:
            speed (int): Motor speed from 0-100 (default 50)

        PWM: AIN1=speed%, AIN2=0%
        """
        speed = max(0, min(100, speed))  # Clamp speed to 0-100 range
        self.pwm1.ChangeDutyCycle(speed)
        self.pwm2.ChangeDutyCycle(0)
        print(f"[DEBUG] Forward: GPIO{self.ain1_gpio}={speed}% PWM, GPIO{self.ain2_gpio}=0%")

    def reverse(self, speed=50):
        """
        Run motor in reverse direction (counter-clockwise).

        Args:
            speed (int): Motor speed from 0-100 (default 50)

        PWM: AIN1=0%, AIN2=speed%
        """
        speed = max(0, min(100, speed))  # Clamp speed to 0-100 range
        self.pwm1.ChangeDutyCycle(0)
        self.pwm2.ChangeDutyCycle(speed)
        print(f"[DEBUG] Reverse: GPIO{self.ain1_gpio}=0%, GPIO{self.ain2_gpio}={speed}% PWM")

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

    def execute_sequence(self, steps):
        """
        Execute a sequence of motor commands.

        Each step in the sequence is executed for a specified duration.

        Args:
            steps (list): List of step dictionaries, each containing:
                - 'action' (str): The action to perform ('forward', 'reverse', 'stop')
                - 'duration_ms' (int): Duration in milliseconds to execute the action
                - 'speed' (int, optional): Speed 0-100 for 'forward'/'reverse' actions (default 50)

        Example:
            sequence = [
                {'action': 'forward', 'duration_ms': 1000, 'speed': 75},
                {'action': 'stop', 'duration_ms': 500},
                {'action': 'reverse', 'duration_ms': 1000, 'speed': 60},
                {'action': 'stop', 'duration_ms': 500}
            ]
            motor.execute_sequence(sequence)

        Raises:
            ValueError: If an unknown action is specified
        """
        # Map action names to methods
        action_map = {
            'forward': self.forward,
            'reverse': self.reverse,
            'stop': self.stop
        }

        print(f"[DEBUG] Starting sequence with {len(steps)} steps")

        for i, step in enumerate(steps):
            action = step.get('action')
            duration_ms = step.get('duration_ms', 0)
            speed = step.get('speed', 50)

            if action not in action_map:
                raise ValueError(f"Unknown action '{action}'. Valid actions: {list(action_map.keys())}")

            # Build debug message
            if action in ['forward', 'reverse']:
                print(f"[DEBUG] Step {i+1}/{len(steps)}: {action} at {speed}% for {duration_ms}ms")
            else:
                print(f"[DEBUG] Step {i+1}/{len(steps)}: {action} for {duration_ms}ms")

            # Execute the action with speed parameter for forward/reverse
            if action in ['forward', 'reverse']:
                action_map[action](speed=speed)
            else:
                action_map[action]()

            # Wait for the specified duration
            if duration_ms > 0:
                time.sleep(duration_ms / 1000.0)

        print("[DEBUG] Sequence complete")

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
