/**
 * rover
 */

#ifndef __ROVER_OBJECT_H
#define __ROVER_OBJECT_H


#include "UsbManager.h"
#include "v4l2Camera.h"
#include "evdevController.h"
#include "panTilt.h"
#include "rpLIDAR.h"
#include "phidgetIMU.h"
#include "rovernet.h"


/**
 * Rover primary object.
 */
class Rover
{
public:
	/**
	 * Create
	 */
	static Rover* Create();

	/**
	 * destructor
	 */
	~Rover();

	/**
 	 * Run the next frame / main loop.
	 */
	bool NextEpoch();

	/**
	 * SetGoal
	 */
	inline void SetGoal( float goal )	{ if(mGoalTensor) mGoalTensor->cpuPtr[0] = goal; }


protected:

	Rover();

	bool init();
	bool initMotors();
	bool initBtController();

	static const uint32_t NumMotorCon = 2;	/**< number of motor controllers */

	MotorController* mMotorCon[NumMotorCon];
	ServoController* mServoCon;
	UsbManager*      mUsbManager;	
	panTilt*		  mPanTilt;
	v4l2Camera*	  mCamera;
	evdevController* mBtController;		/**< Bluetooth /dev/event controller */
	rpLIDAR*		  mLIDAR;
	phidgetIMU*	  mIMU;
	roverNet*		  mRoverNet;

	roverNet::Tensor* mIMUTensor;
	roverNet::Tensor* mLIDARTensor;
	roverNet::Tensor* mRangeMap;
	roverNet::Tensor* mOutputTensor;
	roverNet::Tensor* mGoalTensor;

	static const uint32_t RangeMapSize     = 256;
	static const uint32_t RangeMapMax	   = 5000;
	static const uint32_t OutputStates     = 2;
	static const uint32_t DownsampleFactor = 2;

	void*             mCameraInputCPU;
	void*		      mCameraInputGPU;
	float*            mCameraResizeCPU;
	float*		   	  mCameraResizeGPU;
	roverNet::Tensor* mCameraTensor;		/**< input video in grayscale floating-point */
};



#endif
