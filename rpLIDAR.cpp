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

	if( IS_FAIL(result) )
	{
		printf("rpLIDAR::grabScan(%s) %s  %u %x\n", mPath.c_str(), IS_FAIL(result) ? "FAIL" : "OK", result, result); 	
		return false;
	}

	printf("rpLIDAR -- recieved scan with %zu entries\n", scanEntries);

	memset(samples_out, 0, sizeof(float) * 360);
	
	// mDriver->ascendScanData(scan, scanEntries);
	for( size_t n=0; n < scanEntries; n++ )
	{
		const float angle     = (scan[n].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f;
		const uint16_t dist   = scan[n].distance_q2;		
		const uint8_t quality = (scan[n].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);

		if( quality >= mMinQuality )
			if( angle >= 0.0f && angle <= 360.0f )
				samples_out[(int)angle] = dist;
		
		printf("  [%03zu]  angle %07.3f   dist %05hu   quality %hhu\n", n, angle, dist, quality);
	}

	return true;
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




