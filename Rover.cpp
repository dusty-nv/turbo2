/**
 * rover
 */

#include "Rover.h"


#define CAMERA_PATH "/dev/video0"

#define MOTOR_SERIAL_1	"55FF-7306-7084-5457-2709-0267"
#define MOTOR_SERIAL_2	"55FF-7B06-7084-5457-2608-0267"

#define MAX_SPEED 1600.0f



// constructor
Rover::Rover()
{
	for( uint32_t n=0; n < NumMotorCon; n++ )
		mMotorCon[n] = NULL;
	
	mUsbManager   = NULL;
	mCamera       = NULL;
	mBtController = NULL;
	mLidar        = NULL;
	mRoverNet	    = NULL;
}


// destructor
Rover::~Rover()
{
	if( mRoverNet != NULL )
	{
		delete mRoverNet;
		mRoverNet = NULL;
	}

	if( mLidar != NULL )
	{
		delete mLidar;
		mLidar = NULL;
	}

	if( mBtController != NULL )
	{
		delete mBtController;
		mBtController = NULL;
	}

	for( uint32_t n=0; n < NumMotorCon; n++ )
	{
		if( mMotorCon[n] != NULL )
		{
			delete mMotorCon[n];
			mMotorCon[n] = NULL;
		}
	}

	if( mUsbManager != NULL )
	{
		delete mUsbManager;
		mUsbManager = NULL;
	}
}


// Create
Rover* Rover::Create()
{
	Rover* r = new Rover();

	if( !r->init() )
	{
		printf("[rover]  failed to initialize Rover.\n");
		delete r;
		return NULL;
	}

	return r;
}


// init
bool Rover::init()
{
	// init motor controllers
	if( !initMotors() )
	{
		for( uint32_t n=0; n < NumMotorCon; n++ )
		{
			if( mMotorCon[n] != NULL )
			{
				delete mMotorCon[n];
				mMotorCon[n] = NULL;
			}
		}
	
		if( mUsbManager != NULL )
		{
			delete mUsbManager;
			mUsbManager = NULL;
		}
	}

	// init bluetooth controller
	if( !initBtController() )
	{
		if( mBtController != NULL )
		{
			delete mBtController;
			mBtController = NULL;
		}
	}

	// create V4L2 camera
	mCamera = v4l2Camera::Create(CAMERA_PATH);

	if( !mCamera )
		printf("[rover]  failed to initialize V4L2 camera %s\n", CAMERA_PATH);


	// create roverNet
	mRoverNet = roverNet::Create();

	if( !mRoverNet )
		printf("[rover]  failed to create roverNet instance\n");


#if 1	
	/** test code */
	roverNet::Tensor* tensor = mRoverNet->AllocTensor(5,8);

	if( !tensor )
		printf("[rover]  failed to allocate test tensor\n");

	if( !mRoverNet->updateNetwork(tensor, NULL, NULL) )
		printf("[rover]  failed to run test rovernet update\n");

#endif

	printf("[rover]  done initializing rover\n");
	return true;
}


// initMotors
bool Rover::initMotors()
{
	mUsbManager = new UsbManager();

	if( !mUsbManager->Init() )
		return false;

	const uint32_t numDetected = mUsbManager->GetNumMotorControllers();

	printf("[rover]  detected %u USB motor controllers\n", numDetected);

	for( uint n=0; n < numDetected; n++ )
		printf(" [%u] %s\n", n, mUsbManager->GetMotorController(n)->GetSerial());

	if( numDetected < NumMotorCon )
	{
		printf("[rover]  failed to detect at least %u motor controllers (only found %u)\n", NumMotorCon, numDetected);
		return false;
	}

	mMotorCon[0] = mUsbManager->FindBySerial(MOTOR_SERIAL_1);
	mMotorCon[1] = mUsbManager->FindBySerial(MOTOR_SERIAL_2);

	if( !mMotorCon[0] || !mMotorCon[1] )
	{
		printf("[rover]  failed to confirm motor controllers by serial number (check serials)\n");
		return false;
	}

	for( uint n=0; n < NumMotorCon; n++ )
	{
		printf("\n\ncontroller serial:  %s\n\n", mMotorCon[n]->GetSerial());

		MotorController::Variables var;
		memset(&var, 0, sizeof(MotorController::Variables));

		if( !mMotorCon[n]->ReadVariables(&var) )
		{
			printf("failed to read status of motor %u\n", n);
			return false;
		}

		// print variables
		printf("errors:       ");

		if( var.errorStatus.safeStart )		printf("safe-start ");
		if( var.errorStatus.lowVIN )			printf("low-VIN ");
		if( var.errorStatus.overheat )		printf("overheat ");

		printf("\n");

		printf("targetSpeed:  %i\n", var.targetSpeed);
		printf("speed:        %i\n", var.speed);
		printf("brake:        %u\n", var.brake);
		printf("temperature:  %f degrees C\n", ((float)var.temperature * 0.1f));
		printf("voltage:      %f V\n", ((float)var.voltage * 0.001f));

		if( var.errorStatus.safeStart )
		{
			if( !mMotorCon[n]->ExitSafeStart() )
				printf("\n\nfailed to exit safe-start\n");
		}

		if( !mMotorCon[n]->SetSpeed(0) )
			printf("failed to set 0 speed\n");
	}

	return true;
}


// initBtController
bool Rover::initBtController()
{
	// create HID controller
	mBtController = new evdevController();
	
	if( !mBtController || !mBtController->Open() )
	{
		printf("[rover]  failed to open bluetooth controller\n");
		return false;
	}

	return true;
}


// NextEpoch
bool Rover::NextEpoch()
{
	//printf("[rover]  next_epoch()\n");

	if( mBtController != NULL )
	{
		mBtController->Poll();

		for( int i=0; i < NumMotorCon; i++ )
		{
			float speed = mBtController->Axis[i] * -3200.0f;

			if( speed < -MAX_SPEED )
				speed = -MAX_SPEED;

			if( speed > MAX_SPEED )
				speed = MAX_SPEED;

			if( mMotorCon[i] != NULL )
				mMotorCon[i]->SetSpeed(speed);
		}
	}

	return true;
}


