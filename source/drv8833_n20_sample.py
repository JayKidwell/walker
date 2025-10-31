import RPi.GPIO as GPIO
import time
import logging
import os

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

    try:
        # Loop for 100 cycles
        for cycle in range(100):
            logging.info(f"Cycle {cycle + 1}/100 - Running clockwise")
            
            # Drive the motor clockwise
            GPIO.output(11, GPIO.HIGH) # Set AIN1
            GPIO.output(13, GPIO.LOW)  # Set AIN2
            
            # Wait 5 seconds
            time.sleep(5)
            
            logging.info(f"Cycle {cycle + 1}/100 - Running counter-clockwise")
            
            # Drive the motor counter-clockwise
            GPIO.output(11, GPIO.LOW)  # Set AIN1
            GPIO.output(13, GPIO.HIGH) # Set AIN2
            
            # Wait 5 seconds
            time.sleep(5)
        
        logging.info("Completed 100 cycles")

    except KeyboardInterrupt:
        logging.warning("Program interrupted by user")

    finally:
        # Stop the motor and clean up GPIO
        GPIO.output(11, GPIO.LOW)
        GPIO.output(13, GPIO.LOW)
        GPIO.cleanup()
        logging.debug("GPIO cleaned up")

#
#
#
if __name__ == "__main__":
    main()