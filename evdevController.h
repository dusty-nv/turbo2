/*
 * rover
 */

#ifndef __GASKET_EVDEV_CONTROLLER_H
#define __GASKET_EVDEV_CONTROLLER_H


#include "Config.h"
#include <vector>

#define BLUETOOTH_DEVICE "PLAYSTATION(R)3 Controller (64:D4:BD:0C:6D:0D)"


/**
 * evdevController
 */
class evdevController 
{
public:
	evdevController();
	~evdevController();

	void Poll();

	virtual bool Open();
	virtual bool Close();

	float Axis[2];

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

