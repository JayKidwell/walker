import RPi.GPIO as GPIO
import time
import logging
import os
import math

def main():
    """
        Main function to run the servo test...
    """
    # -- setup logging
    log_level = os.getenv("LOG_LEVEL", "DEBUG").upper()
    logging.basicConfig(
        level=getattr(logging, log_level, logging.DEBUG),
        format='%(asctime)s - %(levelname)s - %(message)s' 
    )
    logging.debug("Starting motor control program") 

    # set numbering as board number on P1 pin header
    GPIO.setmode(GPIO.BOARD)

    # set up GPIO pins
    GPIO.setup(11, GPIO.OUT) # Connected to AIN1
    GPIO.setup(13, GPIO.OUT) # Connected to AIN2
    
    # Set up PWM on both pins at 1000 Hz
    pwm_ain1 = GPIO.PWM(11, 1000)
    pwm_ain2 = GPIO.PWM(13, 1000)
    pwm_ain1.start(0)
    pwm_ain2.start(0)

    try:
        # Loop for 100 cycles
        for cycle in range(100):
            logging.info(f"Cycle {cycle + 1}/100 - Running clockwise")
            
            # Ramp up clockwise (0 to 100% over 500ms)
            ramp_motor(pwm_ain1, pwm_ain2, direction='cw', ramp='up', duration=0.25)
            
            # Run at full speed clockwise for 4 seconds
            pwm_ain1.ChangeDutyCycle(100)
            pwm_ain2.ChangeDutyCycle(0)
            time.sleep(4)
            
            # Ramp down clockwise (100% to 0 over 500ms)
            ramp_motor(pwm_ain1, pwm_ain2, direction='cw', ramp='down', duration=0.25)
            
            logging.info(f"Cycle {cycle + 1}/100 - Running counter-clockwise")
            
            # Ramp up counter-clockwise (0 to 100% over 500ms)
            ramp_motor(pwm_ain1, pwm_ain2, direction='ccw', ramp='up', duration=0.25)
            
            # Run at full speed counter-clockwise for 4 seconds
            pwm_ain1.ChangeDutyCycle(0)
            pwm_ain2.ChangeDutyCycle(100)
            time.sleep(4)
            
            # Ramp down counter-clockwise (100% to 0 over 500ms)
            ramp_motor(pwm_ain1, pwm_ain2, direction='ccw', ramp='down', duration=0.25)
        
        logging.info("Completed 100 cycles")

    except KeyboardInterrupt:
        logging.warning("Program interrupted by user")

    finally:
        # Stop the motor and clean up GPIO
        pwm_ain1.ChangeDutyCycle(0)
        pwm_ain2.ChangeDutyCycle(0)
        pwm_ain1.stop()
        pwm_ain2.stop()
        GPIO.cleanup()
        logging.debug("GPIO cleaned up")


def ramp_motor(pwm_ain1, pwm_ain2, direction='cw', ramp='up', duration=0.5):
    """
    Ramp motor speed up or down using a sine curve for smooth acceleration/deceleration.
    
    Args:
        pwm_ain1: PWM object for AIN1
        pwm_ain2: PWM object for AIN2
        direction: 'cw' for clockwise, 'ccw' for counter-clockwise
        ramp: 'up' to accelerate, 'down' to decelerate
        duration: time in seconds for the ramp (default 0.5)
    """
    steps = 50  # Number of steps in the ramp
    delay = duration / steps
    
    for i in range(steps + 1):
        if ramp == 'up':
            # Sine curve from 0 to 1 (accelerating)
            progress = i / steps
            speed = 100 * math.sin(progress * math.pi / 2)
        else:  # ramp == 'down'
            # Sine curve from 1 to 0 (decelerating)
            progress = i / steps
            speed = 100 * math.cos(progress * math.pi / 2)
        
        if direction == 'cw':
            pwm_ain1.ChangeDutyCycle(speed)
            pwm_ain2.ChangeDutyCycle(0)
        else:  # direction == 'ccw'
            pwm_ain1.ChangeDutyCycle(0)
            pwm_ain2.ChangeDutyCycle(speed)
        
        time.sleep(delay)

#
#
#
if __name__ == "__main__":
    main()