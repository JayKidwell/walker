#!/usr/bin/env python3
"""
Test Joystick Driver
Displays real-time joystick position and button state.
Shows X position, Y position, and button value in scrolling columns.
"""

import time
import logging
import os
from joystick_driver import JoystickController


def main():
    """
    Main function to test joystick readings.
    Displays X, Y, and button state in three columns.
    """
    # Setup logging
    log_level = os.getenv("LOG_LEVEL", "INFO").upper()
    logging.basicConfig(
        level=getattr(logging, log_level, logging.INFO),
        format='%(asctime)s - %(levelname)s - %(message)s'
    )

    logging.info("Starting joystick test")

    try:
        # Initialize joystick
        # X on ADC channel 0, Y on ADC channel 1, button on GPIO 17
        joystick = JoystickController(x_channel=0, y_channel=1, button_gpio=17)

        logging.info("Joystick initialized successfully")

        # Calibrate the joystick
        joystick.calibrate()

        logging.info("Press Ctrl+C to exit")
        print("\n" + "="*60)
        print(f"{'X Position':<20} {'Y Position':<20} {'Button':<20}")
        print("="*60)

        while True:
            # Read joystick values
            x_pos = joystick.read_x()
            y_pos = joystick.read_y()
            button_pressed = joystick.is_button_pressed()

            # Format button display
            button_display = "PRESSED" if button_pressed else "Released"

            # Display as scrolling columns
            print(f"{x_pos:>8.3f}            {y_pos:>8.3f}            {button_display:<20}")

            # Small delay to prevent excessive CPU usage and make output readable
            time.sleep(0.1)

    except KeyboardInterrupt:
        logging.info("\nProgram interrupted by user")

    except Exception as e:
        logging.error(f"Error occurred: {e}")
        import traceback
        traceback.print_exc()

    finally:
        # Clean up
        if 'joystick' in locals():
            joystick.cleanup()
        logging.info("Cleanup complete")


if __name__ == "__main__":
    main()
