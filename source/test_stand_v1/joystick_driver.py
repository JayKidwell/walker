#!/usr/bin/env python3
"""
Joystick Driver for ADS1115
Provides a reusable interface for reading joystick inputs via ADS1115 ADC.
Reads X-axis, Y-axis, and button state from a joystick module.
"""

import board
import busio
from adafruit_ads1x15.ads1115 import ADS1115
from adafruit_ads1x15.analog_in import AnalogIn
import RPi.GPIO as GPIO


class JoystickController:
    """
    Controls a joystick via ADS1115 ADC chip.
    Reads analog X and Y positions, plus digital button state.

    The ADS1115 is a 16-bit ADC with 4 channels connected via I2C.

    Example:
        joystick = JoystickController(x_channel=0, y_channel=1, button_channel=2)

        x_value = joystick.read_x()
        y_value = joystick.read_y()
        button_pressed = joystick.is_button_pressed()

        # Get all values at once
        x, y, button = joystick.read_all()
    """

    def __init__(self, x_channel=0, y_channel=1, button_gpio=17,
                 i2c_address=0x48, mode_already_set=False):
        """
        Initialize joystick controller.

        Args:
            x_channel (int): ADS1115 channel for X-axis (0-3, default 0)
            y_channel (int): ADS1115 channel for Y-axis (0-3, default 1)
            button_gpio (int): BCM GPIO pin for button (default 17)
            i2c_address (int): I2C address of ADS1115 (default 0x48)
            mode_already_set (bool): Set to True if GPIO.setmode() was already called (default False)
        """
        self.x_channel = x_channel
        self.y_channel = y_channel
        self.button_gpio = button_gpio
        self._initialized = False
        self._mode_already_set = mode_already_set

        # Calibration values (will be set during calibration)
        self.x_neutral = 16384  # Default center value
        self.y_neutral = 16384
        self.x_min = 0
        self.x_max = 32767
        self.y_min = 0
        self.y_max = 32767
        self._calibrated = False

        # Initialize I2C and ADS1115 for analog inputs
        self.i2c = busio.I2C(board.SCL, board.SDA)
        self.ads = ADS1115(self.i2c, address=i2c_address)

        # Map channel numbers to pin constants
        # Use direct integer values as the library expects channel numbers 0-3
        pin_map = {
            0: 0,
            1: 1,
            2: 2,
            3: 3
        }

        # Create analog input objects for X and Y channels
        self.x_input = AnalogIn(self.ads, pin_map[x_channel])
        self.y_input = AnalogIn(self.ads, pin_map[y_channel])

        # Setup GPIO for button with internal pull-up
        if not self._mode_already_set:
            GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.button_gpio, GPIO.IN, pull_up_down=GPIO.PUD_UP)

        self._initialized = True
        print(f"[DEBUG] Joystick initialized: X=A{x_channel}, Y=A{y_channel}, Button=GPIO{button_gpio}")

    def read_x_raw(self):
        """
        Read raw ADC value from X-axis.

        Returns:
            int: Raw ADC value (0-32767 for ADS1115)
        """
        return self.x_input.value

    def read_y_raw(self):
        """
        Read raw ADC value from Y-axis.

        Returns:
            int: Raw ADC value (0-32767 for ADS1115)
        """
        return self.y_input.value

    def read_button_raw(self):
        """
        Read raw GPIO value from button input.

        Returns:
            int: GPIO state (0 for LOW/pressed, 1 for HIGH/not pressed with pull-up)
        """
        return GPIO.input(self.button_gpio)

    def calibrate(self):
        """
        Calibrate the joystick to determine neutral position and extremes.

        This method prompts the user to:
        1. Leave joystick in neutral position
        2. Move joystick to all extreme positions

        Returns:
            bool: True if calibration successful
        """
        import time

        print("\n" + "="*60)
        print("JOYSTICK CALIBRATION")
        print("="*60)

        # Step 1: Capture neutral position
        print("\nStep 1: NEUTRAL POSITION")
        print("Leave the joystick in the center/neutral position.")
        print("Calibrating in 3 seconds...")
        time.sleep(1)
        print("2...")
        time.sleep(1)
        print("1...")
        time.sleep(1)

        # Sample neutral position multiple times for accuracy
        x_samples = []
        y_samples = []
        print("Sampling neutral position...")
        for i in range(20):
            x_samples.append(self.read_x_raw())
            y_samples.append(self.read_y_raw())
            time.sleep(0.05)

        self.x_neutral = sum(x_samples) // len(x_samples)
        self.y_neutral = sum(y_samples) // len(y_samples)
        print(f"Neutral position captured: X={self.x_neutral}, Y={self.y_neutral}")

        # Step 2: Capture extremes
        print("\nStep 2: EXTREME POSITIONS")
        print("Move the joystick in a FULL CIRCLE around all edges")
        print("and to all corners (up, down, left, right, diagonals).")
        print("Starting in 3 seconds...")
        time.sleep(1)
        print("2...")
        time.sleep(1)
        print("1...")
        time.sleep(1)
        print("GO! Move the joystick to all extreme positions...")
        print("You have 10 seconds...")

        # Track min/max values
        x_min = self.x_neutral
        x_max = self.x_neutral
        y_min = self.y_neutral
        y_max = self.y_neutral

        # Sample for 10 seconds
        start_time = time.time()
        sample_count = 0
        while time.time() - start_time < 10:
            x_raw = self.read_x_raw()
            y_raw = self.read_y_raw()

            x_min = min(x_min, x_raw)
            x_max = max(x_max, x_raw)
            y_min = min(y_min, y_raw)
            y_max = max(y_max, y_raw)

            sample_count += 1
            time.sleep(0.05)

        self.x_min = x_min
        self.x_max = x_max
        self.y_min = y_min
        self.y_max = y_max
        self._calibrated = True

        print(f"\nCalibration complete! ({sample_count} samples)")
        print(f"X: min={self.x_min}, neutral={self.x_neutral}, max={self.x_max}")
        print(f"Y: min={self.y_min}, neutral={self.y_neutral}, max={self.y_max}")
        print("="*60 + "\n")

        return True

    def read_x(self):
        """
        Read X-axis position as normalized value.

        Returns:
            float: X position normalized to -1.0 to 1.0 range (0.0 at neutral)
        """
        raw = self.read_x_raw()

        if not self._calibrated:
            # Use default calibration
            normalized = (raw - 16384) / 16384.0
            return max(-1.0, min(1.0, normalized))

        # Use calibrated values
        if raw < self.x_neutral:
            # Negative range
            if self.x_neutral == self.x_min:
                return 0.0
            normalized = (raw - self.x_neutral) / (self.x_neutral - self.x_min)
        else:
            # Positive range
            if self.x_max == self.x_neutral:
                return 0.0
            normalized = (raw - self.x_neutral) / (self.x_max - self.x_neutral)

        return max(-1.0, min(1.0, normalized))

    def read_y(self):
        """
        Read Y-axis position as normalized value.

        Returns:
            float: Y position normalized to -1.0 to 1.0 range (0.0 at neutral)
        """
        raw = self.read_y_raw()

        if not self._calibrated:
            # Use default calibration
            normalized = (raw - 16384) / 16384.0
            return max(-1.0, min(1.0, normalized))

        # Use calibrated values
        if raw < self.y_neutral:
            # Negative range
            if self.y_neutral == self.y_min:
                return 0.0
            normalized = (raw - self.y_neutral) / (self.y_neutral - self.y_min)
        else:
            # Positive range
            if self.y_max == self.y_neutral:
                return 0.0
            normalized = (raw - self.y_neutral) / (self.y_max - self.y_neutral)

        return max(-1.0, min(1.0, normalized))

    def is_button_pressed(self):
        """
        Check if joystick button is pressed.

        With pull-up resistor, button reads LOW (0) when pressed.

        Returns:
            bool: True if button is pressed, False otherwise
        """
        return self.read_button_raw() == 0

    def read_all(self):
        """
        Read all joystick inputs at once.

        Returns:
            tuple: (x_value, y_value, button_pressed)
                - x_value (float): Normalized X position (-1.0 to 1.0)
                - y_value (float): Normalized Y position (-1.0 to 1.0)
                - button_pressed (bool): True if button is pressed
        """
        return (self.read_x(), self.read_y(), self.is_button_pressed())

    def read_all_raw(self):
        """
        Read all joystick inputs as raw values.

        Returns:
            tuple: (x_raw, y_raw, button_raw)
                - x_raw (int): Raw X ADC value
                - y_raw (int): Raw Y ADC value
                - button_raw (int): Raw button GPIO value (0 or 1)
        """
        return (self.read_x_raw(), self.read_y_raw(), self.read_button_raw())

    def get_voltage_x(self):
        """
        Get X-axis voltage.

        Returns:
            float: Voltage value in volts
        """
        return self.x_input.voltage

    def get_voltage_y(self):
        """
        Get Y-axis voltage.

        Returns:
            float: Voltage value in volts
        """
        return self.y_input.voltage

    def get_button_state(self):
        """
        Get button GPIO state.

        Returns:
            int: GPIO state (0=pressed, 1=not pressed)
        """
        return GPIO.input(self.button_gpio)

    def cleanup(self):
        """
        Clean up resources.
        Note: I2C cleanup is handled automatically by CircuitPython.
        GPIO is cleaned up for the button pin.
        """
        if self._initialized:
            GPIO.cleanup(self.button_gpio)
            print("[DEBUG] Joystick cleanup complete")
            self._initialized = False

    def __enter__(self):
        """Context manager entry."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup resources."""
        self.cleanup()
