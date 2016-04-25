/*
 * rover
 */

#include "phidgetIMU.h"
#include "Matrix4.h"
#include <phidget21.h>


//#define DEBUG_SPATIAL


// constructor
phidgetIMU::phidgetIMU()
{	
	mHandle     = NULL;
	mBearing    = 0.0f;
	mNewBearing = false;
}


// destructor	
phidgetIMU::~phidgetIMU()
{
	if( mHandle != NULL )
	{
		CPhidget_close((CPhidgetHandle)mHandle);
		mHandle = NULL;
	}
}


void imuPrintInfo( void* handle )
{
	if( !handle )
		return;

	CPhidgetHandle phid = (CPhidgetHandle)handle;

	int serialNo = 0;
	int version = 0;
	const char* ptr;
	int numAccelAxes = 0, numGyroAxes = 0, numCompassAxes = 0;
	int dataRateMax = 0, dataRateMin = 0, dataRateCurr = 0;

	//CPhidget_getDeviceType(phid, &ptr);
	//printf("IMU  %s\n", ptr);

	CPhidget_getSerialNumber(phid, &serialNo);
	CPhidget_getDeviceVersion(phid, &version);
	CPhidgetSpatial_getAccelerationAxisCount((CPhidgetSpatialHandle)phid, &numAccelAxes);
	CPhidgetSpatial_getGyroAxisCount((CPhidgetSpatialHandle)phid, &numGyroAxes);
	CPhidgetSpatial_getCompassAxisCount((CPhidgetSpatialHandle)phid, &numCompassAxes);
	CPhidgetSpatial_getDataRateMax((CPhidgetSpatialHandle)phid, &dataRateMax);
	CPhidgetSpatial_getDataRateMin((CPhidgetSpatialHandle)phid, &dataRateMin);
	CPhidgetSpatial_getDataRate((CPhidgetSpatialHandle)phid, &dataRateCurr);
	
	
	printf("IMU  Serial Number: %10i\n", serialNo);
	printf("IMU  Version: %8i\n", version);
	printf("IMU  Number of Accel Axes: %i\n", numAccelAxes);
	printf("IMU  Number of Gyro Axes: %i\n", numGyroAxes);
	printf("IMU  Number of Compass Axes: %i\n", numCompassAxes);
	printf("IMU  datarate>   Min: %d  Max: %d  Current: %d\n", dataRateMin, dataRateMax, dataRateCurr);
}


//callback that will run if the Spatial is attached to the computer
int imuAttach(CPhidgetHandle spatial, void *userptr)
{
	int serialNo;
	CPhidget_getSerialNumber(spatial, &serialNo);
	printf("IMU %10d attached!\n", serialNo);
	imuPrintInfo(spatial);
	return 0;
}

//callback that will run if the Spatial is detached from the computer
int imuDetach(CPhidgetHandle spatial, void *userptr)
{
	int serialNo;
	CPhidget_getSerialNumber(spatial, &serialNo);
	printf("IMU %10d detached!\n", serialNo);
	return 0;
}

//callback that will run if the Spatial generates an error
int imuError(CPhidgetHandle spatial, void *userptr, int ErrorCode, const char *unknown)
{
	printf("Error handled. %d - %s\n", ErrorCode, unknown);
	return 0;
}

/*
 typedef struct _CPhidgetSpatial_SpatialEventData
{
 double acceleration[3];
 double angularRate[3];
 double magneticField[3];
 CPhidget_Timestamp timestamp;
} CPhidgetSpatial_SpatialEventData
*/



template<typename T> void normalize_vec( T* in, T* out )
{
	const T len = 1.0/Sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]);	

	out[0] = in[0] * len;
	out[1] = in[1] * len;
	out[2] = in[2] * len;
}


// calcBearing
template<typename T> T imuCalcBearing( CPhidgetSpatial_SpatialEventData* data )
{
	T accel[] = { data->acceleration[0], data->acceleration[2], data->acceleration[1] };
	T gravity[3];

	normalize_vec(accel, gravity);

	const T invert = (gravity[1] < 0.0) ? -1.0 : 1.0;		// the board is up-side down
	const T pitch  = asin((T)gravity[0]) * invert;			// BUG pitch is gravity.z
	const T roll   = asin((T)gravity[2]) * invert;			// BUG roll is gravity.x


	// construct a rotation matrix for rotating vectors measured in the body frame, into the earth frame
	// this is done by using the angles between the board and the gravity vector
	TMatrix<T> xRot, zRot, rot;

	xRot.SetRotationMatrixX(pitch);
	zRot.SetRotationMatrixZ(roll);

	rot.MultMatrix(xRot, zRot);

	
	// transform magnetic data by orientation
	T mag_field[] = { data->magneticField[0], data->magneticField[2], -data->magneticField[1] };
	T magnetic[3];


	rot.Mul3x3(mag_field, magnetic);

	

	// these represent the x and y components of the magnetic field vector in the earth frame
	T xh = -magnetic[2];
	T yh = -magnetic[0];


	// we use the computed X-Y to find a magnetic North bearing in the earth frame
	T bearing = 0.0;

	if( xh < 0.0 )					bearing = PI - atan(yh/xh);
	else if( xh > 0.0 && yh < 0.0 )	bearing = -atan(yh/xh);
	else if( xh > 0.0 && yh > 0.0 )	bearing = PI * 2.0 - atan(yh/xh);
	else if( xh == 0.0 && yh < 0.0 )	bearing = PI * 0.5;
	else if( xh == 0.0 && yh > 0.0 )	bearing = PI * 1.5;

	if( gravity[1] < 0.0 )
		bearing = abs(bearing - PI * 2.0);

#ifdef DEBUG_SPATIAL
	printf("IMU bearing %lf\n", double(bearing) * RAD_TO_DEG);
#endif
	//mDynamics.bearing = mFilters[0]->AddSample(bearing);
	//mDynamics.pitch   = mFilters[1]->AddSample(-roll);		// BUG pitch & roll are reversed
	//mDynamics.roll    = mFilters[2]->AddSample(pitch); 
	return bearing;
}

#define MAG_MAX 1000000000.0f

//callback that will run at datarate
//data - array of spatial event data structures that holds the spatial data packets that were sent in this event
//count - the number of spatial data event packets included in this event
int imuData(CPhidgetSpatialHandle spatial, void *userPtr, CPhidgetSpatial_SpatialEventDataHandle* data, int count)
{
	phidgetIMU* node = (phidgetIMU*)userPtr;

	if( !node || !data || count <= 0 )
		return 0;

	// only use the most recent data
	//CPhidgetSpatial_SpatialEventData* ptr = data[count-1];

	// display packets
	int i;
	//printf("IMU data\nNumber of Data Packets in this event: %d\n", count);
	for(i = 0; i < count; i++)
	{
		bool spurious = false;

		for( uint32_t n=0; n < 3; n++ )
		{
			if( data[i]->magneticField[n] > MAG_MAX || data[i]->magneticField[n] < -MAG_MAX )
				spurious = true;
		}

		if( spurious )
			continue;
			
#ifdef DEBUG_SPATIAL
		printf("=== Data Set: %d ===\n", i);
		printf("Acceleration>   x: %6f  y: %6f  z: %6f\n", data[i]->acceleration[0], data[i]->acceleration[1], data[i]->acceleration[2]);
		printf("Angular Rate>   x: %6f  y: %6f  z: %6f\n", data[i]->angularRate[0], data[i]->angularRate[1], data[i]->angularRate[2]);
		printf("Magnetic Field> x: %6f  y: %6f  z: %6f\n", data[i]->magneticField[0], data[i]->magneticField[1], data[i]->magneticField[2]);
		printf("Timestamp> seconds: %d -- microseconds: %d\n", data[i]->timestamp.seconds, data[i]->timestamp.microseconds);
#endif
		
		node->SetBearing(imuCalcBearing<float>(data[i]));	
	}

	//pthread_t tid = pthread_self();
	//printf("thread id:  %lu\n", (unsigned long)tid);
#ifdef DEBUG_SPATIAL
	printf("---------------------------------------------\n");
#endif
	return 0;
}


// Init
bool phidgetIMU::Init()
{
	// create driver object
	if( CPhidgetSpatial_create((CPhidgetSpatialHandle*)&mHandle) != 0 )
	{
		printf("phidgetIMU -- failed to create IMU device\n");
		return false;
	}

	//displayProperties();

	// set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)mHandle, imuAttach, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)mHandle, imuDetach, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)mHandle, imuError, NULL);

	// registers a callback that will run according to the set data rate that will return the spatial data changes
	CPhidgetSpatial_set_OnSpatialData_Handler((CPhidgetSpatialHandle)mHandle, imuData, this);


	// open the spatial object for device connections
	if( CPhidget_open((CPhidgetHandle)mHandle, -1) != 0 )
	{
		printf("failed to open IMU device\n");
		return false;
	}

	//displayProperties();

	// get the program to wait for a spatial device to be attached 
	/*printf("waiting for IMU to be attached...\n");
	const int result = CPhidget_waitForAttachment((CPhidgetHandle)mImu, 2000);

	if( result != 0 )
	{
		const char* err = NULL;
		CPhidget_getErrorDescription(result, &err);
		printf("failed to find attached IMU  (%s)\n", err);
		return false;
	}*/

	//Set the data rate for the spatial events
	CPhidgetSpatial_setDataRate((CPhidgetSpatialHandle)mHandle, /*16*/ 4);


	printf("phidgetIMU -- initialized IMU device\n");
	return true;
}


// Create
phidgetIMU* phidgetIMU::Create()
{
	phidgetIMU* imu = new phidgetIMU();

	if( !imu )
		return NULL;

	if( !imu->Init() )
	{
		delete imu;
		return NULL;
	}

	return imu;
}



