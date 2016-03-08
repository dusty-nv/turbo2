/**
 * rover
 */

#include "ServoController.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>



// constructor
ServoController::ServoController( libusb_device* device ) : mDevice(device)
{
	mDeviceCtx = NULL;
}


// destructor
ServoController::~ServoController()
{
	Close();
}


// Open
bool ServoController::Open()
{
	if( libusb_open(mDevice, &mDeviceCtx) != 0 )
	{
		printf("ServoController - failed to open USB device\n");
		return false;
	}

	// get serial number
	libusb_device_descriptor desc;

	if( libusb_get_device_descriptor(mDevice, &desc) != 0 )
		return false;

	char serialStr[512];
	memset(serialStr, 0, sizeof(serialStr));

	if( libusb_get_string_descriptor_ascii(mDeviceCtx, desc.iSerialNumber, (uint8_t*)serialStr, sizeof(serialStr)) > 0 )
		mSerial = serialStr;

	return true;
}



// Close
void ServoController::Close()
{
	if( mDeviceCtx != NULL )
	{
		libusb_close(mDeviceCtx);
		mDeviceCtx = NULL;
	}
}


// SetSpeed
bool ServoController::SetPosition( uint8_t servo, uint16_t position )
{
	//if( speed > 3200 )
		//return false;

	printf("[ServoController]  set position %u  servo %u\n", (uint32_t)position, (uint32_t)servo);

	// 0x85 REQUEST_SET_TARGET
	return ControlTransfer(0x40, 0x85, position * 4, servo);
}


// ClearErrors
bool ServoController::ClearErrors()
{
	// 0x86 REQUEST_CLEAR_ERRORS
	if( !ControlTransfer(0x40, 0x86, 0, 0) )
		return false;

	return true;
}


    
// ControlTransfer
/*bool ServoController::ControlTransfer( uint8_t requestType, uint8_t request, void* data, uint size )
{
	return ControlTransfer(requestType, request, 0, 0, data, size);
} */


// ControlTransfer
bool ServoController::ControlTransfer( uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, void* data, uint size )
{
	const int res = libusb_control_transfer(mDeviceCtx, requestType, request, value, index, (uint8_t*)data, size, 5000); 

	if( res != size )
	{
		printf("ServoController - usb control transfer failed (size=%i result=%i)\n", size, res);
		return false;
	}
	
	return true;
}  


// ControlTransfer
bool ServoController::ControlTransfer( uint8_t requestType, uint8_t request, uint16_t value, uint16_t index )
{
	return ControlTransfer(requestType, request, value, index, 0, 0);
} 



