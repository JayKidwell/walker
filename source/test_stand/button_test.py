#!/usr/bin/env python3
"""
GPIO Button Test Program
Reads GPIO pins 20 and 21 on Raspberry Pi and displays their states.
Uses the Buttons driver for reusable GPIO functionality.
"""

import time
from button_driver import Buttons


def main():
    """Main loop to read and display GPIO values."""
    try:
        # Use context manager for automatic cleanup
        with Buttons(pull_up=True) as buttons:
            print("Reading Red Button (GPIO 20) and Green Button (GPIO 21). Press Ctrl+C to exit.")
            print("-" * 60)

            while True:
                # Read button states
                red_button_state, green_button_state = buttons.read_both()

                # Display the values
                print(f"Red Button: {red_button_state}  |  Green Button: {green_button_state}")

                # Small delay to avoid overwhelming the output
                time.sleep(0.1)

    except KeyboardInterrupt:
        print("\n\nExiting program...")
        print("GPIO cleanup complete.")


if __name__ == "__main__":
    main()
