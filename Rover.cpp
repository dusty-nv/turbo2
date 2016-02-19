/**
 * rover
 */

#include "Rover.h"

#include "cudaMappedMemory.h"
#include "cudaYUV.h"
#include "cudaResize.h"


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
	mCameraTensor = NULL;
	mIMU   	    = NULL;
	mIMUTensor    = NULL;
	mOutputTensor = NULL;
	mGoalTensor   = NULL;

	mCameraInputGPU  = NULL;
	mCameraInputCPU  = NULL;
	mCameraResizeCPU = NULL;
	mCameraResizeGPU = NULL;
}


// destructor
Rover::~Rover()
{
	if( mRoverNet != NULL )
	{
		delete mRoverNet;
		mRoverNet = NULL;
	}

	if( mIMU != NULL )
	{
		delete mIMU;
		mIMU = NULL;
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

	
	// create IMU
	mIMU = phidgetIMU::Create();
	
	if( !mIMU )
		printf("[rover]  failed to create phidgetIMU\n");	

	mIMUTensor = mRoverNet->AllocTensor(1);

	if( !mIMUTensor )
		printf("[rover]  failed to alloc IMU tensor\n");

	mOutputTensor = mRoverNet->AllocTensor(OutputStates);

	if( !mOutputTensor )
		printf("[rover]  failed to alloc Output tensor\n");

	mGoalTensor = mRoverNet->AllocTensor(1);

	if( !mGoalTensor )
		printf("[rover]  failed to alloc Goal tensor\n");

#if 0	
	/** test code */
	roverNet::Tensor* tensor = mRoverNet->AllocTensor(5,8);

	if( !tensor )
		printf("[rover]  failed to allocate test tensor\n");

	if( !mRoverNet->updateNetwork(tensor, NULL, NULL) )
		printf("[rover]  failed to run test rovernet update\n");

#endif

	printf("[rover]  done initializing rover\n");

	if( mCamera != NULL )
	{	
		printf("[rover]  starting camera\n");
	
		if( !mCamera->Open() )
			printf("[rover]  failed to start camera streaming\n");

		printf("[rover]  camera streaming started\n");
	}

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


static bool joyDegree( int coord_x, int coord_y, float* out )
{
	const float x = float(coord_x) / 128.0f;
	const float y = float(coord_y) / 128.0f;

	const float length = sqrtf(x*x+y*y);

	if( length < 1.0f )
		return false;

	float rad = atan2f(y,x);
	float deg = rad * RAD_TO_DEG;

	if( deg < 0.0f )
		deg = 360.0f + deg;

	//printf("joyDegree %f %f %f %f\n", x, y, length, deg);

	if( out != NULL )
		out[0] = deg;

	return true;
}


// NextEpoch
bool Rover::NextEpoch()
{
	//printf("[rover]  next_epoch()\n");
	const int controllerAutonomousTriggerLevel = 50;	// range of trigger button is 0-255, when above 50 will trigger autonomous mode	

	if( mBtController != NULL && mBtController->Poll() )
	{
		if( mBtController->GetState(evdevController::AXIS_R_BUMPER) <= controllerAutonomousTriggerLevel )
		{
			// manual control mode
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
	}

	if( mIMU != NULL )
	{
		float bearing = 0.0f;
		const bool newIMU = mIMU->GetNewBearing(&bearing);

		if( newIMU )
		{
			printf("[rover]  IMU bearing %f degrees   (goal %f)\n", bearing * RAD_TO_DEG, mGoalTensor->cpuPtr[0]);
			mIMUTensor->cpuPtr[0] = bearing * RAD_TO_DEG;

			// autonomous mode
			if( mBtController && mBtController->GetState(evdevController::AXIS_R_BUMPER) > controllerAutonomousTriggerLevel )
			{
				joyDegree(mBtController->GetState(evdevController::AXIS_RX),
						mBtController->GetState(evdevController::AXIS_RY),
						mGoalTensor->cpuPtr);

				mRoverNet->updateNetwork(mIMUTensor, mGoalTensor, mOutputTensor);

				for( int i=0; i < NumMotorCon; i++ )
				{
					float speed = mOutputTensor->cpuPtr[i] * MAX_SPEED; //3200.0f;

					if( speed < -MAX_SPEED )
						speed = -MAX_SPEED;

					if( speed > MAX_SPEED )
						speed = MAX_SPEED;

					if( mMotorCon[i] != NULL )
						mMotorCon[i]->SetSpeed(speed);
				}
			}
		}
		//else
		//	printf("[rover]  no new IMU data\n");
	}

	if( mCamera != NULL )
	{
		void* img = mCamera->Capture();

		if( img != NULL )
		{
			const uint32_t width  = mCamera->GetWidth();
			const uint32_t height = mCamera->GetHeight();
			const uint32_t pitch  = mCamera->GetPitch();
			const size_t   size   = pitch * height;

			printf("[rover]  captured image %u x %u  pitch=%u bytes\n", width, height, pitch);

			const uint32_t width2  = width / DownsampleFactor;
			const uint32_t height2 = height / DownsampleFactor;
			const uint32_t pitch2  = width2 * sizeof(float);
			const size_t   size2   = pitch2 * height2;

			if( !mCameraTensor )
			{
				mCameraTensor = mRoverNet->AllocTensor(width2, height2);

				cudaAllocMapped(&mCameraInputCPU, &mCameraInputGPU, size);
				cudaAllocMapped((void**)&mCameraResizeCPU, (void**)&mCameraResizeGPU, width * height * sizeof(float));
			}

			if( mCameraTensor != NULL )
			{
				memcpy(mCameraInputCPU, img, size);

				CUDA(cudaYUYVToGray((uchar2*)mCameraInputGPU, (float*)mCameraResizeGPU, width, height));

				CUDA(cudaResize(mCameraResizeGPU, width * sizeof(float), width, height,
							 mCameraTensor->gpuPtr, pitch2, width2, height2));

				if( !mRoverNet->updateNetwork(mCameraTensor, NULL, NULL) )
					printf("[rover]  failed to update roverNet\n");				
			}
		}		
	}


	return true;
}


