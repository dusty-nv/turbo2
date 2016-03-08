/*
 * rover
 */

#ifndef __GASKET_EVDEV_CONTROLLER_H
#define __GASKET_EVDEV_CONTROLLER_H


#include "Config.h"
#include <vector>


#define BLUETOOTH_DEVICE "PLAYSTATION(R)3 Controller (64:D4:BD:0C:6D:0D)"



/**
 * Event Human-Interface Device Controller (/dev/event)
 */
class evdevController 
{
public:
	evdevController();
	~evdevController();

	bool Poll( uint32_t timeout=0 );

	bool Open();
	bool Close();

	float Axis[2];

	static const int TRIGGER_LEVEL_ACTIVATE = 50;

	enum
	{
		AXIS_LX = 0,
		AXIS_LY = 1,
		AXIS_RX = 2,
		AXIS_RY = 3,
		AXIS_L_TRIGGER = 12,
		AXIS_L_BUMPER  = 14,
		AXIS_R_TRIGGER = 13,
		AXIS_R_BUMPER  = 15
	};

	inline int GetState( uint32_t index ) const			{ return mAxisState[index]; }

private:

	void printAxis();

	uint32_t mMaxAxisCount;		// normally equals 64 on linux
	int* mAxisState;

	/*bool allocRingbuffer( uint32_t buffers );
	std::vector<gasket::Buffer*> mRingbuffer;	
	size_t mRingbufferCurrent;*/

	int mFD;

	/*struct Bind
	{
		uint8_t button;
		uint8_t state;
		XMLElement* xml;
	};
	
	std::vector<Bind*> mBinds;
	
	void execBind( uint8_t button, uint8_t state );*/
};



#endif

