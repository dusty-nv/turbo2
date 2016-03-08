/**
 * rover
 */

#include "panTilt.h"


// constructor
panTilt::panTilt( ServoController* servoController ) : mServo(servoController)
{
	mServo->SetPosition(PAN_CHANNEL, PAN_CENTER);
	mServo->SetPosition(TILT_CHANNEL, TILT_CENTER);
}


panTilt::~panTilt()
{
	// ServoController is deleted by UsbManager
}


void panTilt::Update( evdevController* user )
{
	if( !user )
		return;

	/**
	 * pan
	 */
	const float x = float(user->GetState(evdevController::AXIS_RX)) / 128.0f;

	float pan = PAN_CENTER;

	if( x < 0.0f )
		pan = ((PAN_CENTER - PAN_LEFT) * x) + PAN_CENTER; 
	if( x > 0.0f )
		pan = PAN_CENTER - ((PAN_CENTER - PAN_RIGHT) * x);

	mServo->SetPosition(PAN_CHANNEL, pan);

	/**
	 * tilt
	 */
	const float y = float(user->GetState(evdevController::AXIS_RY)) / 128.0f;

	float tilt = TILT_CENTER;

	if( y < 0.0f )
		tilt = ((TILT_CENTER - TILT_DOWN) * y) + TILT_CENTER; 
	if( y > 0.0f )
		tilt = TILT_CENTER - ((TILT_CENTER - TILT_UP) * y);

	mServo->SetPosition(TILT_CHANNEL, tilt);
}
	
