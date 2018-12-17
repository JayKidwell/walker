/*
  CONSTANTS
  /
  Limbs do one thing at at time. 
  You send an instruction and they execute instructions on the joints to satisfy the limb instuction.
  Limb is controlled in x,y,z coordinate space, but controls joints in a joint-angle space
  Coordinate system:
    x in mm, front-to-back, front is positive x, 0 is at the shoulder joint
    y in mm, left-to-right, left is position, 0 is at the shoulder joint
    z in mm, top-to-bottom, top is positive, 0 is at the shoulder joint
  instuctions:
    moveHandLinear( x, y, z, duration ) 
      - move the tip of the hand to this position in mm wrt the shoulder joint
    (more later)
  
*/
#ifndef constants_h
#define constants_h
//
// -- adaFruit pcs9685 pulse width modulation library (to drive servos)
// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you have!
//
// this is the 'minimum' pulse length count (out of 4096)
#define SERVOMIN  150 
//
// this is the 'maximum' pulse length count (out of 4096)
#define SERVOMAX  600 


//
// -- Seial Monitor Baud rate
const int SerialBaudRate = 9600;
//
// -- wrist min value sent to servo
const int WRIST_ANGLE_MIN = 0;
//
// -- wrist max value sent to servo
const int WRIST_ANGLE_MAX = 160;
//
// -- parking position
const int WRIST_ANGLE_PARK = 20;
//
// -- elbow min value sent to servo
const int ELBOW_ANGLE_MIN = 0;
//
// -- elbow max value sent to servo
const int ELBOW_ANGLE_MAX = 90;
//
// -- parking position
const int ELBOW_ANGLE_PARK = 45;
//
// -- shoulder min value sent to servo
const int SHOULDER_ANGLE_MIN = 0;
//
// -- shoulder max value sent to servo
const int SHOULDER_ANGLE_MAX = 160;
//
// -- shoulder max value sent to servo
const int SHOULDER_ANGLE_PARK = 120;
//
// -- arduino pin numbers for each servo
const int servoChannelFrontLeftWrist      = 0;
const int servoChannelFrontLeftElbow      = 1;
const int servoChannelFrontLeftShoulder   = 2;
const int servoChannelFrontRightWrist     = 3;
const int servoChannelFrontRightElbow     = 4;
const int servoChannelFrontRightShoulder  = 5;
const int servoChannelBackLeftWrist       = 6;
const int servoChannelBackLeftElbow       = 7;
const int servoChannelBackLeftShoulder    = 8;
const int servoChannelBackRightWrist      = 9;
const int servoChannelBackRightElbow      = 10;
const int servoChannelBackRightShoulder   = 11;


#endif // constants_h define
