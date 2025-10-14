from adafruit_servokit import ServoKit
from time import sleep
import logging

#
#
#
POS_Flat =     [90, 90, 90,  # front-right leg
                90, 90, 90,  # back-right leg
                90, 90, 90,  # back-left leg
                90, 90, 90]  # front-left leg   
#
#
#
POS_STAND_A =  [180, 0, 90,  # front-right leg
                180, 0, 90,  # back-right leg
                180, 0, 90,  # back-left leg
                180, 0, 90]  # front-left leg   
                
POS_STAND_B =  [180, 90, 90,  # front-right leg
                180, 90, 90,  # back-right leg
                180, 90, 90,  # back-left leg
                180, 90, 90]  # front-left leg    
                
POS_STAND_C =  [135, 135, 90,  # front-right leg
                135, 135, 90,  # back-right leg
                135, 135, 90,  # back-left leg
                135, 135, 90]  # front-left leg   
def main():
    """
        Main function to run the servo test...
    """
    #
    # -- setup logging
    logging.basicConfig(
        # level=logging.INFO,
        level=logging.DEBUG,
        # level=logging.WARNING,
        # level=logging.ERROR,
        # level=logging.CRITICAL,
        format='%(asctime)s - %(levelname)s - %(message)s'
    )
    logging.debug("Debugging details") 
    #
    # -- setup servokit   
    kit = ServoKit(channels=16)
    #
    # wiggle(kit, 0)
    # wiggle(kit, 1)
    # wiggle(kit, 2)
    # wiggle(kit, 3)
    #
    layFlat(kit)
    # stand(kit)
    return
#
#
#
def wiggle(kit, leg ):
    """
        Wiggle a joint
    """
    logging.debug("wiggle") 
    for joint in [ 0, 1, 2 ]:
        for angle in [80,100,80,100,80,100,80,100,90]:
            setJointAngle(kit, leg, joint, angle)
            sleep(.5)
#
#
#
def layFlat(kit):
    setLegs(kit, POS_Flat)
    sleep(2)
#
#
def stand(kit):
    setLegs(kit, POS_STAND_A)
    sleep(2)
    setLegs(kit, POS_STAND_B)
    sleep(2)
    setLegs(kit, POS_STAND_C)
#
#
#
def setLegs(kit, legAngles):
    """
        Set the angles of all servos in all legs.
        legAngles: list of angles for all servos in all legs, 
            0=finger, 
            1=elbow, 
            2=shoulder, 
            3=empty
    """
    logging.debug("setLegs") 
    setLegAngles(kit, 0, legAngles[0:3] )
    setLegAngles(kit, 1, legAngles[3:6] )
    setLegAngles(kit, 2, legAngles[6:9] )
    setLegAngles(kit, 3, legAngles[9:12] )

    #
#
#
#
def setLegAngles(kit, leg, angles):
    """
        Set the angles of all servos in a leg.
        leg: 0 to 3
            0 = front-right
            1 = back-right
            2 = back-left
            3 = front-left
        angles: list of angles for the servos in the leg, 
            0=finger, 
            1=elbow, 
            2=shoulder, 
            3=empty
    """
    logging.debug("setLegAngles") 
    for joint in range(3):
        setServoAngle(kit, joint+(4*leg), angles[joint])
    #
#
#
#
def setJointAngle(kit, leg, joint, angle):
    """
        Set the angle of a servo in a leg.
        leg: 0 to 3
            0 = front-right
            1 = back-right
            2 = back-left
            3 = front-left
    """
    logging.debug("setJointAngle") 
    setServoAngle(kit, joint+(4*leg), angle)
#
#
#
def setServoAngle(kit, channel, angle):
    """
        Set the angle of a servo, 0 to 180 degrees.
    """
    logging.debug("setServoAngle") 
    kit.servo[channel].angle = angle
#
#
#
if __name__ == "__main__":
    main()



