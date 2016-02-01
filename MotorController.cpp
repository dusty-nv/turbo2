/**
 * rover
 */

#include "MotorController.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>



// constructor
MotorController::MotorController( libusb_device* device ) : mDevice(device)
{
	mDeviceCtx = NULL;

	memset(&mVariables, 0, sizeof(Variables));
}


// destructor
MotorController::~MotorController()
{
	Close();
}


// Open
bool MotorController::Open()
{
	if( libusb_open(mDevice, &mDeviceCtx) != 0 )
	{
		printf("MotorController - failed to open USB device\n");
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
void MotorController::Close()
{
	if( mDeviceCtx != NULL )
	{
		libusb_close(mDeviceCtx);
		mDeviceCtx = NULL;
	}
}


// SetSpeed
bool MotorController::SetSpeed( MotorController::Direction dir, int speed )
{
	if( speed > 3200 )
		return false;

	//printf("set speed %i direction %i\n", speed, (int)dir);

	return ControlTransfer(0x40, 0x90, speed, dir);
}

// SetSpeed
bool MotorController::SetSpeed( int speed )
{
	//printf("set speed %i\n", speed);

	//if( speed < 3200 || speed > 3200 )
		//return false;

	if( speed < 0 )
		return SetSpeed(DIRECTION_REVERSE, -speed);
		
	return SetSpeed(DIRECTION_FORWARD, speed);
}


bool MotorController::ExitSafeStart()
{
	if( !ControlTransfer(0x40, 0x91, (uint)0, 0) )
		return false;

	return true;
}


// ReadVariables
bool MotorController::ReadVariables( MotorController::Variables* var )
{
	if( !var )
		return false;

	if( !ControlTransfer(0xC0, 0x83, var, sizeof(Variables)) )
		return false;

	return true;
}


// ControlTransfer
bool MotorController::ControlTransfer( uint8_t requestType, uint8_t request, void* data, uint size )
{
	return ControlTransfer(requestType, request, 0, 0, data, size);
} 


// ControlTransfer
bool MotorController::ControlTransfer( uint8_t requestType, uint8_t request, uint value, uint index, void* data, uint size )
{
	const int res = libusb_control_transfer(mDeviceCtx, requestType, request, value, index, (uint8_t*)data, size, 5000); 

	if( res != size )
	{
		printf("MotorController - usb control transfer failed (size=%i result=%i)\n", size, res);
		return false;
	}
	
	return true;
}  


// ControlTransfer
bool MotorController::ControlTransfer( uint8_t requestType, uint8_t request, uint value, uint index )
{
	return ControlTransfer(requestType, request, value, index, 0, 0);
} 


#if 0
#define PRINT_ENUM(var, x)	if(var.x) printf(#x  " ");

// GetErrorStatus
MotorController::ErrorStatus MotorController::GetErrorStatus()
{
	ErrorStatus status;

	GetVariable(VAR_ERROR_STATUS, &status, sizeof(status));

	printf("ErrorStatus:   ");
	//PRINT_ENUM(status, SafeStart)
	if(status.SafeStart) printf("SafeStart ");
	printf("\n");

	return status;
}

		
// GetTemperature
float MotorController::GetTemperature( bool fahrenheit )
{
	uint16_t_t raw = 0;

	if( !GetVariable(VAR_TEMPERATURE, &raw, sizeof(raw)) )
		return 0.0f;

	float temp = (float)raw * 0.1f;

	if( fahrenheit )
		temp = (temp * 1.8f) + 32.0f;

	return temp;
}


// GetVoltage
float MotorController::GetVoltage()
{
	uint16_t_t raw = 0;

	if( !GetVariable(VAR_VOLTAGE, &raw, sizeof(raw)) )
		return 0.0f;

	return (float)raw * 0.001f;
}

// GetVariable
bool MotorController::GetVariable( MotorController::Variable var, void* buffer, uint size )
{
	if( size != 2 )
	{
		printf("MotorController::GetVariable - size of buffer must be 2 uint8_ts (%u uint8_ts)\n", size);
		return false;
	}

	// send variable query
	uint8_t query[] = { 0xA1, (uint8_t)var };
	
	if( !Send(query, sizeof(query)) )
	{
		printf("MotorController::GetVariable - failed to query variable %i\n", var);
		return false;
	}

	// wait for reception
	if( Recieve(buffer, size) != size )
	{
		printf("MotorController::GetVariable - failed to recieve variable %i\n", var);
		return false;
	}

	return true;
}


// Receieve
uint MotorController::Recieve( void* buffer, uint size )
{
	if( mFD == -1 )
		return 0;

	// initialise the timeout structure
	struct timeval timeout;
		
	timeout.tv_sec  = 2; // 2 second timeout
	timeout.tv_usec = 0;

	// do the select
	fd_set rdfs;
	FD_ZERO( &rdfs );
	FD_SET( mFD, &rdfs );

	const int sRes = select(mFD + 1, &rdfs, NULL, NULL, &timeout);
	
	// check if an error has occured
	if(sRes < 0)
	{
		printf("MotorController::Recieve - select failed (errno %i)\n", sRes);
		return 0;
	}
	else if( sRes == 0 )
	{
		printf("MotorController::Recieve - timeout\n");
		return 0;
	}

	// read
	const int rRes = read(mFD, buffer, size);

	if( rRes < 0 )
	{
		printf("MotorController::Recieve - read error %i\n", rRes);
		return 0;
	}

	return rRes;
}



// Close
void MotorController::Close()
{
	if( mFD == -1 )
		return;

	close(mFD);
	mFD = -1;
}


// Send
bool MotorController::Send( void* data, uint size )
{
	if( mFD == -1 )
		return false;

	if( write(mFD, data, size) != size )
		return false;

	return true;
}


// Senduint8_t
bool MotorController::Senduint8_t( uint8_t value )
{
	if( mFD == -1 )
		return false;

	if( write(mFD, &value, 1) != 1 )
		return false;

	return true;
}

#endif


