/**
 * rover
 */

#ifndef __ROVER_SERVO_CONTROLLER_H
#define __ROVER_SERVO_CONTROLLER_H


#include "Config.h"
#include "stdio.h"

#include <string>
#include <libusb-1.0/libusb.h>


/**
 * ServoController
 */
class ServoController
{
public:
	/**
	 * constructor
	 */
	ServoController( libusb_device* device );

	/**
	 * destructor
	 */
	~ServoController();

	/**
	 * Open
	 */
	bool Open();

	/**
	 * Close
	 */
	void Close();

	/**
	 * Return device serial string.
	 */
	inline const char* GetSerial() const								{ return mSerial.c_str(); }

	/**
	 * Usb Vendor Id
	 */
	static const uint16_t VendorId = 0x1ffb;		// Pololu Corporation

	/**
	 * Usb Product Id
	 */
	static const uint16_t ProductId = 0x89;			// Maestro 6-channel


	/**
 	 * Set servo position
	 */
	bool SetPosition( uint8_t servo, uint16_t position );

	/**
	 * Clear errors
	 */
	bool ClearErrors();

protected:

	//bool ControlTransfer( uint8_t requestType, uint8_t request, void* data, uint size );
	bool ControlTransfer( uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, void* data, uint size );
	bool ControlTransfer( uint8_t requestType, uint8_t request, uint16_t  value, uint16_t index );

	libusb_device* mDevice;
	libusb_device_handle* mDeviceCtx;

	std::string mSerial;
};


#endif

