#!/usr/bin/env python3
"""
Test Motor Control with Buttons
Uses red and green buttons to control DC motor direction via DRV8833.
"""

import time
import logging
import os
from button_driver import Buttons
from motor_driver import MotorController


def main():
    """
    Main function to control motor with buttons.
    Red button: motor runs forward
    Green button: motor runs reverse
    No button: motor stops
    """
    # Setup logging
    log_level = os.getenv("LOG_LEVEL", "INFO").upper()
    logging.basicConfig(
        level=getattr(logging, log_level, logging.INFO),
        format='%(asctime)s - %(levelname)s - %(message)s'
    )

    logging.info("Starting button-controlled motor test")

    # Initialize components
    buttons = Buttons(pull_up=True)
    # mode_already_set=True because Buttons class already called GPIO.setmode()
    # motor = MotorController(ain1_gpio=4, ain2_gpio=5, mode_already_set=True)
    # motor = MotorController(ain1_gpio=6, ain2_gpio=12, mode_already_set=True)
    motor = MotorController(ain1_gpio=13, ain2_gpio=16, mode_already_set=True)

    try:
        logging.info("Press RED button for forward, GREEN button for reverse")
        logging.info("Press Ctrl+C to exit")
        last_state = "stopped"
        while True:
            red_pressed = buttons.is_red_button_pressed()
            green_pressed = buttons.is_green_button_pressed()

            if red_pressed and not green_pressed:
                if last_state != "forward":
                    motor.forward(speed=15)
                    logging.info("Motor running FORWARD at 15% speed (red button pressed)")
                    last_state = "forward"

            elif green_pressed and not red_pressed:
                if last_state != "reverse":
                    motor.reverse(speed=15)
                    logging.info("Motor running REVERSE at 15% speed (green button pressed)")
                    last_state = "reverse"

            elif red_pressed and green_pressed:
                # Both buttons pressed - stop for safety
                if last_state != "stopped":
                    motor.stop()
                    logging.warning("Both buttons pressed - motor STOPPED")
                    last_state = "stopped"

            else:
                # No buttons pressed
                if last_state != "stopped":
                    motor.stop()
                    logging.info("No buttons pressed - motor STOPPED")
                    last_state = "stopped"

            # Small delay to prevent excessive CPU usage
            time.sleep(0.05)

    except KeyboardInterrupt:
        logging.info("Program interrupted by user")

    finally:
        # Clean up
        motor.stop()
        buttons.cleanup()
        logging.info("Cleanup complete")


if __name__ == "__main__":
    main()
