/*
 * rover
 */

#include "evdevController.h"

#include <linux/input.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>



// constructor
evdevController::evdevController()
{	
	mFD = -1;
	//mRingbufferCurrent = 0;

	mMaxAxisCount = ABS_CNT;
	mAxisState = (int*)malloc(sizeof(int) * mMaxAxisCount);
	memset(mAxisState, 0, sizeof(int) * mMaxAxisCount); 

	Axis[0] = 0.0f;
	Axis[1] = 0.0f;
}


// destructor	
evdevController::~evdevController()
{
	Close();
}


// printAxis
void evdevController::printAxis()
{
	const int numAxis = 16;

	for( int i=0; i < numAxis; i++ )
		printf("  %02i ", i);

	printf("\n");

	for( int i=0; i < numAxis; i++ )
		printf(" %+04i", mAxisState[i]);

	printf("\n");
}


// ProcessEmit
bool evdevController::Poll( uint32_t timeout )
{
	const uint32_t max_ev = 64;
	struct input_event ev[max_ev];

	//printf("evDevcontroller::read()\n");

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(mFD, &fds);

	struct timeval tv;
 
	tv.tv_sec  = 0;
	tv.tv_usec = timeout*1000;

	const int result = select(mFD + 1, &fds, NULL, NULL, &tv);

	if( result == -1 ) 
	{
		//if (EINTR == errno)
		printf("evdevController -- select() failed (errno=%i) (%s)\n", errno, strerror(errno));
		return false;
	}
	else if( result == 0 )
	{
		//printf("evdevController -- select() timed out...\n");
		return false;	// timeout, not necessarily an error (TRY_AGAIN)
	}

	const int bytesRead = read(mFD, ev, sizeof(struct input_event) * max_ev);

	//printf("evDevcontroller::read() ==> %i\n", bytesRead);

	if( bytesRead < (int)sizeof(struct input_event) ) 
	{
		printf("evdev read() expected %d bytes, got %d\n", (int)sizeof(struct input_event), bytesRead);
		return false;
	}


   /*struct input_event {
		struct timeval time;
		__u16 type;
		__u16 code;
		__s32 value;
	};*/

#define DUAL_AXIS

#ifdef DUAL_AXIS
	const int axisA = 1;
	const int axisB = 3;
#else
	const int axisA = 0;
	const int axisB = 1;
#endif

	const int num_ev = bytesRead / sizeof(struct input_event);
	bool triggerMsg  = false;

	for( int i = 0; i < num_ev; i++ ) 
	{
		/*if( ev[i].type == EV_REL || ev[i].type == EV_ABS )
		{
			printf("evt  type %hu  code %hu  value %+04i\n",
				  ev[i].type, ev[i].code, ev[i].value);
		}*/

		if( ev[i].type == EV_ABS )
		{
			if( ev[i].code >= mMaxAxisCount )
				continue;
			
			const int axisCurr = ev[i].value;
			const int axisPrev = mAxisState[ev[i].code];
			const int axisDiff = axisCurr - axisPrev;
		
			/*if( ev[i].code > 4 )
			{
				const int sw = 50;
	
				if( axisCurr >= sw && axisPrev < sw )
					execBind(ev[i].code, 1);
				else if( axisCurr < sw && axisPrev >= sw )
					execBind(ev[i].code, 0);
			}*/

			if( axisDiff != 0 )
				if( ev[i].code == axisA || ev[i].code == axisB )	// TEMP only use axis 1 and 3 (Y-axis of left & right joysticks)
					triggerMsg = true;

			mAxisState[ev[i].code] = axisCurr;
		}
	}


	//if( GetFlag(NODE_VERBOSE) )
//		printAxis();

	
	Axis[0] = float(mAxisState[axisA]) / 128.0f;
	Axis[1] = float(mAxisState[axisB]) / 128.0f;

/*
	// get the next slot from the ringbuffer
	Buffer* buffer     = mRingbuffer[mRingbufferCurrent];
	mRingbufferCurrent = (mRingbufferCurrent + 1) % mRingbuffer.size();

	float* vec = (float*)buffer->GetCPU();
	const float axisMax = 128.0f;

	vec[0] = float(mAxisState[axisA]) / axisMax;
	vec[1] = float(mAxisState[axisB]) / axisMax * -1.0f;

#ifdef DUAL_AXIS
	vec[0] *= -1.0f;
#endif

	//printf("%f %f\n", vec[0], vec[1]);
	Emit(buffer);	*/
	return true;
}


/**
 * Filter for the AutoDevProbe scandir on /dev/input.
 * @param dir The current directory entry provided by scandir.
 * @return Non-zero if the given directory entry starts with "event", or zero
 * otherwise.
 */
static int is_event_device(const struct dirent *dir) 
{
	return strncmp("event", dir->d_name, 5) == 0;
}


static char* scan_devices( const char* searchName )
{
	if( !searchName )
		return NULL;

	struct dirent **namelist;
	int ndev = scandir("/dev/event", &namelist, is_event_device, versionsort);

	if (ndev <= 0)
	{
		ndev = scandir("/dev/input", &namelist, is_event_device, versionsort);

		if (ndev <= 0)		
			return NULL;
	}

	printf("Available devices:\n");

	for(int i = 0; i < ndev; i++)
	{
		char fname[64];
		char name[256] = "???";

		snprintf(fname, sizeof(fname), "%s/%s", "/dev/input", namelist[i]->d_name);
		int fd = open(fname, O_RDONLY);
		if (fd < 0)
			continue;
		ioctl(fd, EVIOCGNAME(sizeof(name)), name);

		printf("%s:	%s\n", fname, name);

		bool matched = false;

		if( strcmp(name, searchName) == 0 )		
			matched = true;

		close(fd);
		free(namelist[i]);

		if( matched )
		{
			char *filename;
			asprintf(&filename, "/dev/input/event%d",i);
			return filename;
		}
	}

	printf("failed to find /dev/input/event* node matching evdev %s\n", searchName);
	return NULL;
}


// Open
bool evdevController::Open()
{
	printf("evdevController::Open()\n");

	// locate the /dev/event* path for this device
	char* filename = scan_devices(BLUETOOTH_DEVICE);
	
	if( !filename )
		filename = "/dev/input/event1"; //	return false;

	
	// open file
	mFD = open(filename, O_RDONLY);

	if( mFD < 0)
	{
		const int error = errno;

		if( error == EACCES && getuid() != 0 )
			printf("You do not have access to %s. Try "
					       "running as root instead.\n", filename);
		else
			printf("errno %i opening %s\n", error, filename);

		return false;
	}

	//
	/*if( !allocRingbuffer(8) )
	{
		printf("evdevController - failed to allocate ringbuffer\n");
		return false;
	}*/

	return true; //Node::Open();
}
	
	
// allocRingbuffer
/*bool evdevController::allocRingbuffer( uint32_t count )
{
	if( mRingbuffer.size() == count )
		return true;

	for( uint32_t n=0; n < count; n++ )
	{
		Buffer* buf = NULL;

		buf = Buffer::Alloc(8, typeOf<float>(2));

		if( !buf )
			continue;

		mRingbuffer.push_back(buf);
	}

	if( mRingbuffer.size() > 1 )
		return true;

	return false;
}*/



// Close
bool evdevController::Close()
{
	//if( !Node::Close() )
	//d	return false;

	if( mFD >= 0 )
	{
		close(mFD);
		mFD = -1;
	}

	return true;
}




