/**
 * rover
 */

#ifndef __ROVER_ARM_CONTROLLER_H
#define __ROVER_ARM_CONTROLLER_H


#include "ServoController.h"
#include "evdevController.h"


/**
 * ArmController
 */
class ArmController
{
public:
	/**
	 * constructor
	 */
	ArmController( ServoController* servoController );

	/**
	 * destructor
	 */
	~ArmController();

	/**
	 * Update
	 */
	void Update( evdevController* userController );
	
	static const uint8_t PAN_CHANNEL    = 7;
	static const uint8_t TILT_CHANNEL_1 = 8;
	static const uint8_t TILT_CHANNEL_2 = 10;
	static const uint8_t TILT_CHANNEL_3 = 9;

	static const uint16_t PAN_CENTER  = 1500;		// arm L/R base
	static const uint16_t PAN_LEFT    = 992;
	static const uint16_t PAN_RIGHT   = 2000;

	static const uint16_t TILT_CENTER_1 = 1200;		// 1st arm segment
	static const uint16_t TILT_DOWN_1   = 1392;
	static const uint16_t TILT_UP_1     = 992;
	
	static const uint16_t TILT_CENTER_2 = 1500;		// 2nd arm segment
	static const uint16_t TILT_DOWN_2   = 2000;
	static const uint16_t TILT_UP_2     = 992;
	
	static const uint16_t TILT_CENTER_3 = 1500;		// final DoF
	static const uint16_t TILT_DOWN_3   = 2000;
	static const uint16_t TILT_UP_3     = 992;

protected:
	void arm1( evdevController* user );
	void arm2( evdevController* user );
	void arm3( evdevController* user );
	
	ServoController* mServo;
};


#endif

