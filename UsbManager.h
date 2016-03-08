/**
 * rover
 */

#ifndef __ROVER_USB_MANAGER_H
#define __ROVER_USB_MANAGER_H


#include "Config.h"

#include <string>
#include <vector>
#include <libusb-1.0/libusb.h>

#include "MotorController.h"
#include "ServoController.h"


/**
 * UsbManager
 */
class UsbManager
{
public:
	/**
	 * constructor
	 */
	UsbManager();

	/**
	 * destructor
	 */
	~UsbManager();

	/**
	 * Init
	 */
	bool Init();

	/**
	 * GetNumMotorControllers
	 */
	inline uint32_t GetNumMotorControllers() const										{ return mControllers.size(); }

	/**
	 * GetMotorController
	 */
	inline MotorController* GetMotorController( uint32_t index ) const						{ return mControllers[index]; }
	
	/**
	 * GetNumServoControllers
	 */
	inline uint32_t GetNumServoControllers() const										{ return mServos.size(); }

	/**
	 * GetServoController
	 */
	inline ServoController* GetServoController( uint32_t index )							{ return mServos[index]; }

	/**
	 * FindBySerial
	 */
	MotorController* FindBySerial( const char* serial ) const;

protected:

	std::vector<MotorController*> mControllers;
	std::vector<ServoController*> mServos;
	libusb_context*			mUsbContext;
};



#endif
