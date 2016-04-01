/*
 * rover
 */

#include "rpLIDAR.h"
#include "rplidar.h"



using namespace rp::standalone::rplidar;


// constructor
rpLIDAR::rpLIDAR()
{	
	mDriver     = NULL;
	mMinQuality = 0;
	
	/*
		<zone angleMin="0.0" angleMax="40.0" distMin="1" distMax="2000"/>
		<zone angleMin="325.0" angleMax="361.0" distMin="1" distMax="2000"/>
		<zone angleMin="150.0" angleMax="210.0" distMin="1000" distMax="2000"/>*/
		
	AddZone(0.0f, 40.0f, 1.0f, 2000.0f, 3);
	AddZone(325.0f, 361.0f, 1.0f, 2000.0f, 3);
	AddZone(150.0f, 210.0f, 1000.0f, 2000.0f, 6);
}


// destructor	
rpLIDAR::~rpLIDAR()
{
	if( mDriver != NULL )
	{
		mDriver->disconnect();
		RPlidarDriver::DisposeDriver(mDriver);
		mDriver = NULL;
	}
}


// Create
rpLIDAR* rpLIDAR::Create( const char* path )
{
	if( !path )
		return NULL;

	rpLIDAR* l = new rpLIDAR();

	if( !l->init(path) )
	{
		delete l;
		return NULL;
	}

	return l;
}

	
// init
bool rpLIDAR::init( const char* path )
{
	mDriver = RPlidarDriver::CreateDriver();

	if( !mDriver )
	{
		printf("rpLidar -- failed to create RPlidarDriver instance.\n");
		return false;
	}

	// enable serial connection
	const u_result result = mDriver->connect(path, 115200);

	printf("rpLIDAR::connect(%s) %s  %u %x\n", path, IS_FAIL(result) ? "FAIL" : "OK", result, result); 	

	if( IS_FAIL(result) )
		return false;

	mPath = path;

	// check device stats
	rplidar_response_device_info_t info;
	memset(&info, 0, sizeof(rplidar_response_device_info_t));

	if( IS_OK(mDriver->getDeviceInfo(info)) )
	{
		printf("rpLIDAR device %s\n", path);
		printf("   + model     %hhu\n", info.model);
		printf("   + firmware v%hu\n", info.firmware_version);
		printf("   + hardware v%hu\n", info.hardware_version);
	}
	else
		printf("failed to get rpLIDAR device data...\n");


	rplidar_response_device_health_t health;
	memset(&health, 0, sizeof(health));

	if( IS_OK(mDriver->getHealth(health)) )
		printf("   + health %u %u\n", (uint32_t)health.status, (uint32_t)health.error_code);

	return true;
}


/*typedef struct _rplidar_response_measurement_node_t {
    _u8    sync_quality;      // syncbit:1;syncbit_inverse:1;quality:6;
    _u16   angle_q6_checkbit; // check_bit:1;angle_q6:15;
    _u16   distance_q2;
} __attribute__((packed)) rplidar_response_measurement_node_t;*/


// Poll
bool rpLIDAR::Poll( float* samples_out, uint32_t timeout )
{
	if( !rpConnected() )
		return false;

	// grab scan
	const uint32_t scanEntriesMax = 360;	
	rplidar_response_measurement_node_t scan[scanEntriesMax];

	size_t scanEntries    = scanEntriesMax;
	const u_result result = mDriver->grabScanData(scan, scanEntries, timeout);

	if( result == RESULT_OPERATION_TIMEOUT )
	{
		//printf("rpLIDAR::grabScan(%s) %s  %u 0x%x\n", mPath.c_str(), "TIMEOUT", result, result); 
		return false;
	}
	else if( IS_FAIL(result) )
	{
		printf("rpLIDAR::grabScan(%s) %s  %u 0x%x\n", mPath.c_str(), IS_FAIL(result) ? "FAIL" : "OK", result, result); 	
		return false;
	}

	printf("rpLIDAR -- recieved scan with %zu entries\n", scanEntries);
		
	// reset zones
	const uint32_t numZones = mZones.size();
	
	for( size_t z=0; z < numZones; z++ )
		mZones[z]->detections = 0;
		
	// reset sample histogram
	memset(samples_out, 0, sizeof(float) * 360);
	
	// mDriver->ascendScanData(scan, scanEntries);
	for( size_t n=0; n < scanEntries; n++ )
	{
		const float angle     = (scan[n].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f;
		const uint16_t dist   = scan[n].distance_q2;		
		const uint8_t quality = (scan[n].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);

		if( quality >= mMinQuality && dist > 0 && angle >= 0.0f && angle <= 360.0f )
		{
			samples_out[(int)angle] = dist;

			for( size_t z=0; z < numZones; z++ )
			{
				if( angle >= mZones[z]->angleMin && angle <= mZones[z]->angleMax &&
					dist >= mZones[z]->distMin && dist <= mZones[z]->distMax )
						mZones[z]->detections++;
			}
		}
		//printf("  [%03zu]  angle %07.3f   dist %05hu   quality %hhu\n", n, angle, dist, quality);
	}

	return true;
}


// AddZone
void rpLIDAR::AddZone( float angleMin, float angleMax, float distMin, float distMax, uint32_t detectionLimit )
{
	Zone* z = new Zone();
	
	z->angleMin = angleMin;
	z->angleMax = angleMax;
	z->distMin = distMin;
	z->distMax = distMax;
	z->detections = 0;
	z->detectionLimit = detectionLimit;
	
	mZones.push_back(z);
}
	
	
// CheckZones
bool rpLIDAR::CheckZones()
{
	const uint32_t numZones = mZones.size();
	bool detected = false;
	
	for( uint32_t z=0; z < numZones; z++ )
	{
		if( ZoneActive(z) )
		{
			detected = true;
			printf("rpLIDAR -- zone %u -- %u / %u detections\n", z, mZones[z]->detections, mZones[z]->detectionLimit);
		}
	}
	
	return detected;
}


// ZoneActive
bool rpLIDAR::ZoneActive( uint32_t z )
{
	//if( z >= mZones.size() )
	//	return false;
		
	if( mZones[z]->detections >= mZones[z]->detectionLimit )
		return true;
		
	return false;
}



// AvoidZones
bool rpLIDAR::AvoidZones( float* controls )
{
	const uint32_t numZones = mZones.size();
	bool detected = false;
	
	if( numZones != 3 )
		return false;
		
	if( !controls )
		return false;
		
	if( ZoneActive(0) || ZoneActive(1) )
	{
		if( controls[0] > 0.0f || controls[1] > 0.0f )
		{
			controls[0] = 0.0f;
			controls[1] = 0.0f;
			
			detected = true;
		}
	}
	
	if( ZoneActive(2) )
	{
		if( controls[0] < 0.0f || controls[1] < 0.0f )
		{
			controls[0] = 0.0f;
			controls[1] = 0.0f;
			
			detected = true;
		}
	}

	return detected;
}


// Open
bool rpLIDAR::Open()
{
	printf("rpLIDAR::Open()\n");

	// make sure serial connection to lidar is functioning
	if( !rpConnected() )
		return false;

	// start lidar scanning
	const u_result result = mDriver->startScan();

	printf("rpLIDAR::startScan(%s) %s  %u %x\n", mPath.c_str(), IS_FAIL(result) ? "FAIL" : "OK", result, result); 

	if( IS_FAIL(result) )
		return false;

	return true;
}
	


// rpConnected
bool rpLIDAR::rpConnected()
{
	if( !mDriver )
		return false;

	if( !mDriver->isConnected() )
	{
		printf("rpLIDAR is not connected...\n");
		return false;
	}

	return true;
}


// Close
bool rpLIDAR::Close()
{
	if( mDriver != NULL )
	{
		const u_result result = mDriver->stop();
		printf("rpLIDAR::stopScan(%s) %s  %u %x\n", mPath.c_str(), IS_FAIL(result) ? "FAIL" : "OK", result, result); 
	}

	return true;
}




