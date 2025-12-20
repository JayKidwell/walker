#!/usr/bin/env python3
"""
Test Motor Control with Joystick
Uses joystick to control three DC motors for test arm joints via DRV8833.
"""

import time
import logging
import os
from joystick_driver import JoystickController
from motor_driver import MotorController
from button_driver import Buttons


def main():
    """
    Main function to control motors with joystick.
    Joystick X-axis controls motor_knee_x
    Joystick Y-axis controls motor_knee_y
    """
    # Setup logging
    log_level = os.getenv("LOG_LEVEL", "INFO").upper()
    logging.basicConfig(
        level=getattr(logging, log_level, logging.INFO),
        format='%(asctime)s - %(levelname)s - %(message)s'
    )

    logging.info("Starting joystick-controlled motor test")

    # Initialize joystick
    joystick = JoystickController(x_channel=0, y_channel=1, button_gpio=17)

    # Calibrate the joystick
    logging.info("Calibrating joystick...")
    joystick.calibrate()

    # Initialize buttons
    # mode_already_set=True because JoystickController already called GPIO.setmode()
    buttons = Buttons(pull_up=True)

    # Initialize three motors
    # mode_already_set=True because JoystickController already called GPIO.setmode()
    motor_knee_x = MotorController(ain1_gpio=13, ain2_gpio=16, mode_already_set=True)
    motor_knee_y = MotorController(ain1_gpio=6, ain2_gpio=12, mode_already_set=True)
    motor_wrist = MotorController(ain1_gpio=4, ain2_gpio=5, mode_already_set=True)

    logging.info("Motors initialized:")
    logging.info("  motor_knee_x:  GPIO 4/5")
    logging.info("  motor_knee_y:  GPIO 6/12")
    logging.info("  motor_wrist:   GPIO 13/16")

    try:
        logging.info("Joystick X controls motor_knee_x")
        logging.info("Joystick Y controls motor_knee_y")
        logging.info("Green button controls motor_wrist forward at 20%")
        logging.info("Red button controls motor_wrist reverse at 20%")
        logging.info("Press Ctrl+C to exit")

        last_x_speed = 0
        last_y_speed = 0
        last_green_pressed = False
        last_red_pressed = False

        while True:
            # Read joystick position
            x_pos = joystick.read_x()  # -1.0 to 1.0
            y_pos = joystick.read_y()  # -1.0 to 1.0

            # Map joystick position to motor speed (-30% to +30%)
            # At neutral (0.0), motor stops
            # At full deflection (±1.0), motor runs at ±30% speed
            x_speed = int(x_pos * 20)  # Maps -1.0 to 1.0 => -20 to 20
            y_speed = int(y_pos * 40)  # Maps -1.0 to 1.0 => -40 to 40

            # Control motor_knee_x based on joystick X (reversed direction)
            if x_speed != last_x_speed:
                if x_speed > 0:
                    motor_knee_x.reverse(speed=abs(x_speed))
                    logging.info(f"motor_knee_x: REVERSE at {abs(x_speed)}% (X={x_pos:.2f})")
                elif x_speed < 0:
                    motor_knee_x.forward(speed=abs(x_speed))
                    logging.info(f"motor_knee_x: FORWARD at {abs(x_speed)}% (X={x_pos:.2f})")
                else:
                    motor_knee_x.stop()
                    logging.info("motor_knee_x: STOP (X=0.00)")
                last_x_speed = x_speed

            # Control motor_knee_y based on joystick Y
            if y_speed != last_y_speed:
                if y_speed > 0:
                    motor_knee_y.forward(speed=abs(y_speed))
                    logging.info(f"motor_knee_y: FORWARD at {abs(y_speed)}% (Y={y_pos:.2f})")
                elif y_speed < 0:
                    motor_knee_y.reverse(speed=abs(y_speed))
                    logging.info(f"motor_knee_y: REVERSE at {abs(y_speed)}% (Y={y_pos:.2f})")
                else:
                    motor_knee_y.stop()
                    logging.info("motor_knee_y: STOP (Y=0.00)")
                last_y_speed = y_speed

            # Read button states
            green_pressed = buttons.is_green_button_pressed()
            red_pressed = buttons.is_red_button_pressed()

            # Control motor_wrist based on buttons
            if green_pressed != last_green_pressed:
                if green_pressed:
                    motor_wrist.forward(speed=20)
                    logging.info("motor_wrist: FORWARD at 20% (Green button pressed)")
                else:
                    motor_wrist.stop()
                    logging.info("motor_wrist: STOP (Green button released)")
                last_green_pressed = green_pressed

            if red_pressed != last_red_pressed:
                if red_pressed:
                    motor_wrist.reverse(speed=20)
                    logging.info("motor_wrist: REVERSE at 20% (Red button pressed)")
                else:
                    motor_wrist.stop()
                    logging.info("motor_wrist: STOP (Red button released)")
                last_red_pressed = red_pressed

            # Small delay to prevent excessive CPU usage
            time.sleep(0.05)

    except KeyboardInterrupt:
        logging.info("Program interrupted by user")

    finally:
        # Clean up - stop all motors
        logging.info("Stopping all motors...")
        motor_knee_x.stop()
        motor_knee_y.stop()
        motor_wrist.stop()

        # Cleanup resources
        motor_knee_x.cleanup()
        motor_knee_y.cleanup()
        motor_wrist.cleanup()
        joystick.cleanup()
        buttons.cleanup()

        logging.info("Cleanup complete")


if __name__ == "__main__":
    main()
