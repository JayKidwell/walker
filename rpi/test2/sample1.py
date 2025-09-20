from adafruit_servokit import ServoKit
from time import sleep
#
#
#
def main():
    """
        Main function to run the servo test..
    """
    kit = ServoKit(channels=16)
    # Sweep servo on channel 0
    # set servos 0 to 3 to 0, 180, and 90 degrees
    servoMax = range(6)
    for i in servoMax:
        setServoAngle(kit, i, 80)
    sleep(.5)
    for i in servoMax:
        setServoAngle(kit, i, 90)
    sleep(.5)
    for i in servoMax:
        setServoAngle(kit, i, 100)
#
#
#
def setServoAngle(kit, channel, angle):
    """
        Set the angle of a servo.
    """
    kit.servo[channel].angle = angle
#
#
#
if __name__ == "__main__":
    main()



