/*
 * rover
 */

#ifndef __ROVER_RPLIDAR_H
#define __ROVER_RPLIDAR_H


#include "Config.h"
#include <string>


namespace rp { namespace standalone{ namespace rplidar { class RPlidarDriver; } } }


/**
 * rpLIDAR
 */
class rpLIDAR
{
public:
	/**
	 * Create
	 */
	static rpLIDAR* Create( const char* path );

	/**
	 * Destructor
	 */
	~rpLIDAR();

	/**
	 * Attempt to retrieve data from the rpLIDAR
	 */
	bool Poll( float* samples_out, uint32_t timeout=0 );

	/**
	 * Start the rpLIDAR scanning.
	 */
	bool Open();
	
	/**
	 * Stop the rpLIDAR scanning.
	 */
	bool Close();

protected:

	rpLIDAR();
	bool init( const char* path );

	std::string mPath;
	uint32_t    mMinQuality;		// minimum quality level of LIDAR scan point (default=0)

	bool rpConnected();
	rp::standalone::rplidar::RPlidarDriver* mDriver;
};
	


#endif
