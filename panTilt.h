/**
 * rover
 */

#ifndef __ROVER_PAN_TILT_H
#define __ROVER_PAN_TILT_H


#include "ServoController.h"
#include "evdevController.h"


/**
 * panTilt
 */
class panTilt
{
public:
	/**
	 * constructor
	 */
	panTilt( ServoController* servoController );

	/**
	 * destructor
	 */
	~panTilt();

	/**
	 * Update
	 */
	void Update( evdevController* userController );
	
	static const uint8_t PAN_CHANNEL  = 0;
	static const uint8_t TILT_CHANNEL = 1;

	static const uint16_t PAN_CENTER  = 1650;
	static const uint16_t PAN_LEFT    = 2000;
	static const uint16_t PAN_RIGHT   = 1280;

	static const uint16_t TILT_CENTER = 1440;
	static const uint16_t TILT_DOWN   = 1950;
	static const uint16_t TILT_UP     = 1000;

protected:

	ServoController* mServo;
};


#endif

