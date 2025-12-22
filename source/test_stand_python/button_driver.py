#!/usr/bin/env python3
"""
Button Driver for Test Stand
Provides a reusable interface for reading GPIO buttons on pins 20 and 21.
"""

import RPi.GPIO as GPIO


class Buttons:
    """
    Manages GPIO buttons on the test stand.
    Handles initialization and reading of button states on GPIO pins 20 and 21.
    """

    # GPIO pin configuration
    RED_BUTTON = 20      # GPIO pin 20
    GREEN_BUTTON = 21    # GPIO pin 21

    # Aliases for backward compatibility
    BUTTON_PIN_20 = RED_BUTTON
    BUTTON_PIN_21 = GREEN_BUTTON

    def __init__(self, pull_up=True):
        """
        Initialize the button GPIO pins.

        Args:
            pull_up (bool): If True, enables internal pull-up resistors.
                           If False, enables pull-down resistors.
        """
        self._initialized = False
        self._pull_up = pull_up
        self.setup()

    def setup(self):
        """Configure GPIO pins for reading buttons."""
        if not self._initialized:
            GPIO.setmode(GPIO.BCM)  # Use BCM pin numbering

            pull_mode = GPIO.PUD_UP if self._pull_up else GPIO.PUD_DOWN
            GPIO.setup(self.RED_BUTTON, GPIO.IN, pull_up_down=pull_mode)
            GPIO.setup(self.GREEN_BUTTON, GPIO.IN, pull_up_down=pull_mode)

            self._initialized = True

    def read_red_button(self):
        """
        Read the current state of red button (GPIO 20).

        Returns:
            int: 1 for HIGH, 0 for LOW
        """
        return GPIO.input(self.RED_BUTTON)

    def read_green_button(self):
        """
        Read the current state of green button (GPIO 21).

        Returns:
            int: 1 for HIGH, 0 for LOW
        """
        return GPIO.input(self.GREEN_BUTTON)

    def read_both(self):
        """
        Read both button states at once.

        Returns:
            tuple: (red_button_state, green_button_state)
        """
        return (self.read_red_button(), self.read_green_button())

    def is_red_button_pressed(self):
        """
        Check if red button is pressed.

        Returns:
            bool: True if pressed (LOW with pull-up), False if not pressed (HIGH)
        """
        return self.read_red_button() == 0 if self._pull_up else self.read_red_button() == 1

    def is_green_button_pressed(self):
        """
        Check if green button is pressed.

        Returns:
            bool: True if pressed (LOW with pull-up), False if not pressed (HIGH)
        """
        return self.read_green_button() == 0 if self._pull_up else self.read_green_button() == 1

    # Backward compatibility aliases
    def read_button_20(self):
        """Read GPIO 20 (red button). Deprecated: use read_red_button() instead."""
        return self.read_red_button()

    def read_button_21(self):
        """Read GPIO 21 (green button). Deprecated: use read_green_button() instead."""
        return self.read_green_button()

    def is_button_20_pressed(self):
        """Check if GPIO 20 is pressed. Deprecated: use is_red_button_pressed() instead."""
        return self.is_red_button_pressed()

    def is_button_21_pressed(self):
        """Check if GPIO 21 is pressed. Deprecated: use is_green_button_pressed() instead."""
        return self.is_green_button_pressed()

    def cleanup(self):
        """Clean up GPIO resources."""
        if self._initialized:
            GPIO.cleanup()
            self._initialized = False

    def __enter__(self):
        """Context manager entry."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup GPIO."""
        self.cleanup()
