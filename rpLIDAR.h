/*
 * rover
 */

#ifndef __ROVER_RPLIDAR_H
#define __ROVER_RPLIDAR_H


#include "Config.h"

#include <string>
#include <vector>


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

	/**
	 * Add a collision detection zone.
	 */
	void AddZone( float angleMin, float angleMax, float distMin, float distMax, uint32_t detectionLimit=5 );
	
	/**
	 * Report if any zones have detected a collision.
	 */
	bool CheckZones();
	
	/**
	 * Null out motor controls if they are headed for a collision.
	 * Returns true if the collision avoidance had to kick in and cancel the controls.
	 * Otherwise, returns false if the controls were passed through untouched.
	 */
	bool AvoidZones( float* controls );
	
	/**
	 * Check if a zone is actively detecting.
	 */
	bool ZoneActive( uint32_t index );
	

protected:

	rpLIDAR();
	bool init( const char* path );

	std::string mPath;
	uint32_t    mMinQuality;		// minimum quality level of LIDAR scan point (default=0)

	bool rpConnected();
	rp::standalone::rplidar::RPlidarDriver* mDriver;
		
	struct Zone
	{
		float    angleMin;
		float    angleMax;
		float    distMin;
		float    distMax;
		uint32_t detections;
		uint32_t detectionLimit;
	};

	std::vector<Zone*> mZones;
};
	


#endif
