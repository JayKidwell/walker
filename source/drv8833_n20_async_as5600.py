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
    
    # Define motor configuration
    motor_config = {
        'name': 'Motor1',
        'pin_ain1': 11,  # Board pin 11 (BCM 17)
        'pin_ain2': 13,  # Board pin 13 (BCM 27)
        'i2c_bus': 1,
        'sensor_address': 0x36,  # AS5600 default address
        'pwm_frequency': 1000
    }
    
    # Run the async main function with config
    asyncio.run(async_main(motor_config))


async def async_main(config):
    """
        Async main function that handles motor control and sensor reading
        
        Args:
            config: Dictionary with motor hardware configuration:
                - name: Motor identifier for logging
                - pin_ain1: GPIO pin for AIN1
                - pin_ain2: GPIO pin for AIN2
                - i2c_bus: I2C bus number (usually 1)
                - sensor_address: AS5600 I2C address (default 0x36)
                - pwm_frequency: PWM frequency in Hz
    """
    motor_name = config['name']
    
    # Initialize I2C and AS5600 sensor FIRST (before GPIO setup)
    # This is because board/busio may set GPIO mode to BCM
    try:
        i2c = busio.I2C(board.SCL, board.SDA)
        sensor = adafruit_as5600.AS5600(i2c)
        logging.info(f"{motor_name}: AS5600 sensor initialized successfully")
    except Exception as e:
        logging.error(f"{motor_name}: Failed to initialize AS5600 sensor: {e}")
        return
    
    # Now set up GPIO for motor control
    GPIO.setwarnings(False)
    
    # The board library sets GPIO to BCM mode, so we need to handle this
    current_mode = GPIO.getmode()
    if current_mode == GPIO.BCM:
        # board library already set BCM mode, convert board pins to BCM
        pin_mapping = {11: 17, 13: 27}  # Board to BCM mapping
        pin_ain1 = pin_mapping.get(config['pin_ain1'], config['pin_ain1'])
        pin_ain2 = pin_mapping.get(config['pin_ain2'], config['pin_ain2'])
        logging.debug(f"{motor_name}: Using BCM mode (pins {pin_ain1}, {pin_ain2})")
    else:
        GPIO.setmode(GPIO.BOARD)
        pin_ain1 = config['pin_ain1']
        pin_ain2 = config['pin_ain2']
        logging.debug(f"{motor_name}: Using BOARD mode (pins {pin_ain1}, {pin_ain2})")

    # set up GPIO pins
    GPIO.setup(pin_ain1, GPIO.OUT) # Connected to AIN1
    GPIO.setup(pin_ain2, GPIO.OUT) # Connected to AIN2
    
    # Set up PWM on both pins
    pwm_ain1 = GPIO.PWM(pin_ain1, config['pwm_frequency'])
    pwm_ain2 = GPIO.PWM(pin_ain2, config['pwm_frequency'])
    pwm_ain1.start(0)
    pwm_ain2.start(0)
    
    # Shared state for sensor readings
    sensor_data = {'angle': 0, 'raw_angle': 0}
    
    try:
        # Start the sensor reading task
        sensor_task = asyncio.create_task(read_sensor_continuously(sensor, sensor_data, motor_name))
        
        # Loop for 100 cycles
        for cycle in range(100):
            logging.info(f"{motor_name}: Cycle {cycle + 1}/100 - Running clockwise | Position: {sensor_data['angle']:.1f}° (Raw: {sensor_data['raw_angle']})")
            
            # Ramp up clockwise (0 to 100% over 500ms)
            await ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, motor_name, direction='cw', ramp='up', duration=0.5)
            
            # Run at full speed clockwise for 1 seconds
            pwm_ain1.ChangeDutyCycle(100)
            pwm_ain2.ChangeDutyCycle(0)
            await log_position_periodically(sensor_data, motor_name, duration=1, status="CW Full Speed")
            
            # Ramp down clockwise (100% to 0 over 500ms)
            await ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, motor_name, direction='cw', ramp='down', duration=0.5)
            
            logging.info(f"{motor_name}: Cycle {cycle + 1}/100 - Running counter-clockwise | Position: {sensor_data['angle']:.1f}° (Raw: {sensor_data['raw_angle']})")
            
            # Ramp up counter-clockwise (0 to 100% over 500ms)
            await ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, motor_name, direction='ccw', ramp='up', duration=0.5)
            
            # Run at full speed counter-clockwise for 1 seconds
            pwm_ain1.ChangeDutyCycle(0)
            pwm_ain2.ChangeDutyCycle(100)
            await log_position_periodically(sensor_data, motor_name, duration=1, status="CCW Full Speed")

            # Ramp down counter-clockwise (100% to 0 over 500ms)
            await ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, motor_name, direction='ccw', ramp='down', duration=0.5)
        
        logging.info(f"{motor_name}: Completed 100 cycles")
        
        # Cancel sensor task
        sensor_task.cancel()
        try:
            await sensor_task
        except asyncio.CancelledError:
            pass

    except KeyboardInterrupt:
        logging.warning(f"{motor_name}: Program interrupted by user")

    finally:
        # Stop the motor and clean up GPIO
        pwm_ain1.ChangeDutyCycle(0)
        pwm_ain2.ChangeDutyCycle(0)
        pwm_ain1.stop()
        pwm_ain2.stop()
        logging.debug(f"{motor_name}: Motor stopped and PWM cleaned up")


async def read_sensor_continuously(sensor, sensor_data, motor_name):
    """
    Continuously read the AS5600 sensor and update shared data
    """
    while True:
        try:
            # Raw angle is 0-4095 (12-bit)
            raw = sensor.raw_angle
            # Convert to degrees (0-360)
            angle = (raw / 4096.0) * 360.0
            
            sensor_data['raw_angle'] = raw
            sensor_data['angle'] = angle
        except Exception as e:
            logging.error(f"{motor_name}: Error reading sensor: {e}")
        await asyncio.sleep(0.05)  # Read at 20 Hz


async def log_position_periodically(sensor_data, motor_name, duration, status):
    """
    Log position at regular intervals during a fixed duration
    """
    start_time = asyncio.get_event_loop().time()
    while asyncio.get_event_loop().time() - start_time < duration:
        logging.info(f"{motor_name}: {status} | Position: {sensor_data['angle']:.1f}° (Raw: {sensor_data['raw_angle']})")
        await asyncio.sleep(0.5)  # Log every 500ms


async def ramp_motor_async(pwm_ain1, pwm_ain2, sensor_data, motor_name, direction='cw', ramp='up', duration=0.5):
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
            logging.info(f"{motor_name}: {status} | Position: {sensor_data['angle']:.1f}° (Raw: {sensor_data['raw_angle']})")
        
        await asyncio.sleep(delay)

#
#
#
if __name__ == "__main__":
    main()