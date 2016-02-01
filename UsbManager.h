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
	inline const uint32_t GetNumMotorControllers() const										{ return mControllers.size(); }

	/**
	 * GetMotorController
	 */
	inline MotorController* GetMotorController( uint index ) const							{ return mControllers[index]; }
	
	/**
	 * FindBySerial
	 */
	MotorController* FindBySerial( const char* serial ) const;

protected:

	std::vector<MotorController*> mControllers;
	libusb_context*			mUsbContext;
};



#endif
