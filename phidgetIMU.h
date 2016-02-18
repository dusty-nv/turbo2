/*
 * rover
 */

#ifndef __ROVER_PHIDGET_IMU_H
#define __ROVER_PHIDGET_IMU_H


#include "Config.h"

//typedef struct _CPhidgetSpatial *CPhidgetSpatialHandle;

#ifndef PI
#define PI 3.14159265358979323846264338327950288
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG  57.295779513082320876798154814105f
#endif


/**
 * phidgetIMU
 */
class phidgetIMU
{
public:
	static phidgetIMU* Create();
	~phidgetIMU();

	inline float GetBearing() const		{ return mBearing; }
	inline void SetBearing( float value )	{ mBearing = value; mNewBearing = true; }


	inline bool GetNewBearing( float* output )
	{
		if( mNewBearing )
		{
			*output = mBearing;
			mNewBearing = false;
			return true;
		}
	
		return false;
	}


private:
	phidgetIMU();
	bool Init();

	void* mHandle;
	float mBearing;
	bool  mNewBearing;
};


#endif

