/**
 * rover
 */

#include "ArmController.h"


// constructor
ArmController::ArmController( ServoController* servoController ) : mServo(servoController)
{
	//mServo->SetPosition(PAN_CHANNEL, PAN_CENTER);
	//mServo->SetPosition(TILT_CHANNEL, TILT_CENTER);
}


ArmController::~ArmController()
{
	// ServoController is deleted by UsbManager
}


void ArmController::Update( evdevController* user )
{
	if( !user )
		return;

	printf("ARM controller active!\n");
				
	/**
	 * pan
	 */
	const float x = float(user->GetState(evdevController::AXIS_LX)) / 128.0f;

	float pan = PAN_CENTER;

	if( x < 0.0f )
		pan = ((PAN_CENTER - PAN_LEFT) * x) + PAN_CENTER; 
	if( x > 0.0f )
		pan = PAN_CENTER - ((PAN_CENTER - PAN_RIGHT) * x);

	mServo->SetPosition(PAN_CHANNEL, pan);

	arm1(user);
	arm2(user);
	arm3(user);
}

void ArmController::arm1( evdevController* user )
{
	const float y = float(user->GetState(evdevController::AXIS_LY)) / 128.0f;

	float tilt = TILT_CENTER_1;

	if( y < 0.0f )
		tilt = ((TILT_CENTER_1 - TILT_DOWN_1) * y) + TILT_CENTER_1; 
	if( y > 0.0f )
		tilt = TILT_CENTER_1 - ((TILT_CENTER_1 - TILT_UP_1) * y);

	mServo->SetPosition(TILT_CHANNEL_1, tilt);
}

void ArmController::arm2( evdevController* user )
{
	const float y = float(user->GetState(evdevController::AXIS_RY)) / 128.0f;

	float tilt = TILT_CENTER_2;

	if( y < 0.0f )
		tilt = ((TILT_CENTER_2 - TILT_DOWN_2) * y) + TILT_CENTER_2; 
	if( y > 0.0f )
		tilt = TILT_CENTER_2 - ((TILT_CENTER_2 - TILT_UP_2) * y);

	mServo->SetPosition(TILT_CHANNEL_2, tilt);
}

void ArmController::arm3( evdevController* user )
{
	const float y = float(user->GetState(evdevController::AXIS_RX)) / 128.0f;

	float tilt = TILT_CENTER_3;

	if( y < 0.0f )
		tilt = ((TILT_CENTER_3 - TILT_DOWN_3) * y) + TILT_CENTER_3; 
	if( y > 0.0f )
		tilt = TILT_CENTER_3 - ((TILT_CENTER_3 - TILT_UP_3) * y);

	mServo->SetPosition(TILT_CHANNEL_3, tilt);
}
	
