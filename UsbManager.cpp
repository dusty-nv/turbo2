/**
 * rover
 */

#include "UsbManager.h"




// constructor
UsbManager::UsbManager()
{
	mUsbContext = NULL;

}


// destructor
UsbManager::~UsbManager()
{
	for( uint n=0; n < mControllers.size(); n++ )
	{
		if( mControllers[n] )
			delete mControllers[n];
	}

	if( mUsbContext )
		libusb_exit(mUsbContext);
}


MotorController* UsbManager::FindBySerial( const char* serial ) const
{
	const uint num = mControllers.size();

	for( uint n=0; n < num; ++n )
	{
		if( strcmp(mControllers[n]->GetSerial(), serial) == 0 )
			return mControllers[n];
	}

	return NULL;
}


// Init
bool UsbManager::Init()
{
	// open USB context
	if( libusb_init(&mUsbContext) != 0 )
	{
		printf("UsbManager - failed to create libusb context\n");
		return false;
	}

	// get devices
	libusb_device** devList;
	const uint numDevices = libusb_get_device_list(mUsbContext, &devList);
	
	printf("UsbManager - %u devices\n\n", numDevices);


	for( uint n=0; n < numDevices; n++ )
	{
		libusb_device_descriptor desc;

		if( libusb_get_device_descriptor(devList[n], &desc) != 0 )
			continue;

		printf("   [%02u]   vendor %#04x\t\tproduct %#04x\n", n, desc.idVendor, desc.idProduct);

		if( desc.idVendor == MotorController::VendorId && desc.idProduct == MotorController::ProductId )
		{
			MotorController* ctrl = new MotorController(devList[n]);

			if( !ctrl->Open() )
			{
				delete ctrl;
				continue;
			}

			mControllers.push_back(ctrl);
		}
	}
		
	printf("\nUsbManager - opened %u controllers\n", GetNumMotorControllers());
	return true;
}



