import RPi.GPIO as GPIO
import time
import logging
import os
import math
import asyncio
import board
import busio
import adafruit_as5600

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
    
    # Run the async main function
    asyncio.run(async_main())


async def async_main():
    """
        Async main function that handles motor control and sensor reading
    """
    # Initialize I2C and AS5600 sensor FIRST (before GPIO setup)
    # This is because board/busio may set GPIO mode to BCM
    try:
        i2c = busio.I2C(board.SCL, board.SDA)
        sensor = adafruit_as5600.AS5600(i2c)
        logging.info("AS5600 sensor initialized successfully")
    except Exception as e:
        logging.error(f"Failed to initialize AS5600 sensor: {e}")
        return
    
    # Now set up GPIO for motor control
    GPIO.setwarnings(False)
    
    # The board library sets GPIO to BCM mode, so we need to handle this
    current_mode = GPIO.getmode()
    if current_mode == GPIO.BCM:
        # board library already set BCM mode, we'll use BCM pins instead
        # BCM 17 = Board 11, BCM 27 = Board 13
        pin_ain1 = 17
        pin_ain2 = 27
        logging.debug("Using BCM mode (set by board library)")
    else:
        GPIO.setmode(GPIO.BOARD)
        pin_ain1 = 11
        pin_ain2 = 13
        logging.debug("Using BOARD mode")

    # set up GPIO pins
    GPIO.setup(pin_ain1, GPIO.OUT) # Connected to AIN1
    GPIO.setup(pin_ain2, GPIO.OUT) # Connected to AIN2
    
    # Set up PWM on both pins at 1000 Hz
    pwm_ain1 = GPIO.PWM(pin_ain1, 1000)
    pwm_ain2 = GPIO.PWM(pin_ain2, 1000)
    pwm_ain1.start(0)
    pwm_ain2.start(0)
    
    # Shared state for sensor readings
    sensor_data = {'angle': 0, 'raw_angle': 0}
    
    try:
        # Start the sensor reading task
        sensor_task = asyncio.create_task(read_sensor_continuously(sensor, sensor_data))
        
        # Loop for 100 cycles
        for cycle in range(100):
            logging.info(f"Cycle {cycle + 1}/100 - Running clockwise | Position: {sensor_data['angle']:.2f}° (Raw: {sensor_data['raw_angle']})")
            
            # Ramp up clockwise (0 to 100% over 500ms)
            await ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, direction='cw', ramp='up', duration=0.5)
            
            # Run at full speed clockwise for 4 seconds
            pwm_ain1.ChangeDutyCycle(100)
            pwm_ain2.ChangeDutyCycle(0)
            await log_position_periodically(sensor_data, duration=4, status="CW Full Speed")
            
            # Ramp down clockwise (100% to 0 over 500ms)
            await ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, direction='cw', ramp='down', duration=0.5)
            
            logging.info(f"Cycle {cycle + 1}/100 - Running counter-clockwise | Position: {sensor_data['angle']:.2f}° (Raw: {sensor_data['raw_angle']})")
            
            # Ramp up counter-clockwise (0 to 100% over 500ms)
            await ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, direction='ccw', ramp='up', duration=0.5)
            
            # Run at full speed counter-clockwise for 4 seconds
            pwm_ain1.ChangeDutyCycle(0)
            pwm_ain2.ChangeDutyCycle(100)
            await log_position_periodically(sensor_data, duration=4, status="CCW Full Speed")
            
            # Ramp down counter-clockwise (100% to 0 over 500ms)
            await ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, direction='ccw', ramp='down', duration=0.5)
        
        logging.info("Completed 100 cycles")
        
        # Cancel sensor task
        sensor_task.cancel()
        try:
            await sensor_task
        except asyncio.CancelledError:
            pass

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


async def read_sensor_continuously(sensor, sensor_data):
    """
    Continuously read the AS5600 sensor and update shared data
    """
    while True:
        try:
            sensor_data['raw_angle'] = sensor.raw_angle
            sensor_data['angle'] = sensor.angle
        except Exception as e:
            logging.error(f"Error reading sensor: {e}")
        await asyncio.sleep(0.05)  # Read at 20 Hz


async def log_position_periodically(sensor_data, duration, status):
    """
    Log position at regular intervals during a fixed duration
    """
    start_time = asyncio.get_event_loop().time()
    while asyncio.get_event_loop().time() - start_time < duration:
        logging.info(f"{status} | Position: {sensor_data['angle']:.2f}° (Raw: {sensor_data['raw_angle']})")
        await asyncio.sleep(0.5)  # Log every 500ms


async def ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, direction='cw', ramp='up', duration=0.5):
    """
    Async version of ramp motor with position logging
    """
    steps = 50
    delay = duration / steps
    
    for i in range(steps + 1):
        if ramp == 'up':
            progress = i / steps
            speed = 100 * math.sin(progress * math.pi / 2)
            status = f"Ramping {direction.upper()} UP ({speed:.0f}%)"
        else:
            progress = i / steps
            speed = 100 * math.cos(progress * math.pi / 2)
            status = f"Ramping {direction.upper()} DOWN ({speed:.0f}%)"
        
        if direction == 'cw':
            pwm_ain1.ChangeDutyCycle(speed)
            pwm_ain2.ChangeDutyCycle(0)
        else:
            pwm_ain1.ChangeDutyCycle(0)
            pwm_ain2.ChangeDutyCycle(speed)
        
        # Log position every 10 steps
        if i % 10 == 0:
            logging.info(f"{status} | Position: {sensor_data['angle']:.2f}° (Raw: {sensor_data['raw_angle']})")
        
        await asyncio.sleep(delay)

#
#
#
if __name__ == "__main__":
    main()