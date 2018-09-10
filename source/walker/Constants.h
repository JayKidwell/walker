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
// -- wrist min value sent to servo
const int WRIST_RANGE_MIN = 20;
//
// -- wrist max value sent to servo
const int WRIST_RANGE_MAX = 145;
//
// -- elbow min value sent to servo
const int ELBOW_RANGE_MIN = 20;
//
// -- elbow max value sent to servo
const int ELBOW_RANGE_MAX = 145;
//
// -- shoulder min value sent to servo
const int SHOULDER_RANGE_MIN = 20;
//
// -- shoulder max value sent to servo
const int SHOULDER_RANGE_MAX = 145;
//
// -- arduino pin numbers for each servo
const int servoPinFrontLeftWrist      = 1;
const int servoPinFrontLeftElbow      = 2;
const int servoPinFrontLeftShoulder   = 3;
const int servoPinFrontRightWrist     = 4;
const int servoPinFrontRightElbow     = 5;
const int servoPinFrontRightShoulder  = 6;
const int servoPinBackLeftWrist       = 7;
const int servoPinBackLeftElbow       = 8;
const int servoPinBackLeftShoulder    = 9;
const int servoPinBackRightWrist      = 10;
const int servoPinBackRightElbow      = 11;
const int servoPinBackRightShoulder   = 12;


#endif // constants_h define
