/*
* constants.h
*
* Created: 4/18/2016
*  Author: ollul666
*/

/************************************************************************/
/*					     	COMMANDTYPES                                */
/************************************************************************/
#define controlCommand 255
#define sensorCommand 253
#define settingCommand 252

/************************************************************************/
/*					     	   SENSORS                                  */
/************************************************************************/
#define sensorFrontRight 0x01
#define sensorFrontLeft 0x02
#define sensorRearRight 0x03
#define sensorRearLeft 0x04
#define sensorForward 0x05
#define sensorRotation 0x06
#define sensorTarget 0x07

/************************************************************************/
/*					     	   COMMANDS                                 */
/************************************************************************/
#define commandStop 0x00
#define commandForward 0x01
#define commandReverse 0x02
#define commandRight 0x03
#define commandLeft 0x04
#define commandForwardRight 0x05
#define commandForwardLeft 0x06
#define commandReverseRight 0x07
#define commandReverseLeft 0x08
#define commandClaw 0x09
#define commandScan 0x0A
#define stop 0x00
#define forward 0x01
#define rotation 0x02

/************************************************************************/
/*					     	   TWI                                      */
/************************************************************************/
#define mySlaveAdress 0xCC

/************************************************************************/
/*					     	   INDICES                                  */
/************************************************************************/
#define frontRightIndex  0
#define frontLeftIndex 1
#define rearRightIndex 2
#define rearLeftIndex 3
#define noWallsIndex 10

/************************************************************************/
/*					     	   LIMITS                                   */
/************************************************************************/
#define maxDistance 245
#define maxSpeed 100
#define zeroAngVel 124
#define moduleDepth 35
#define minDistanceForward 24