/*
 * rover
 */
 
#ifndef __VIDEO_ENCODER_H
#define __VIDEO_ENCODER_H

#include "Config.h"
#include <string>


struct _GstBus;
struct _GstCaps;
struct _GstElement;
struct _GstBuffer;



/**
 * videoEncoder
 */
class videoEncoder
{
public:
	static videoEncoder* Create( uint32_t width, uint32_t height );
	~videoEncoder();

	bool Open();
	bool Close();
	
	bool ProcessFrame( uint8_t* yuv_I420 );

private:
	videoEncoder( uint32_t width, uint32_t height );
	bool init();
	
	static void onNeed(_GstElement* pipeline, uint32_t size, void* user_data);
	static void onEnough(_GstElement* pipeline, void* user_data);

	bool buildLaunchStr();
	bool buildCapsStr();
	
	_GstBus*     mBus;
	_GstCaps*    mBufferCaps;
	_GstElement* mAppSrc;
	_GstElement* mPipeline;
	_GstBuffer*  mInputBuffer;
	bool         mNeedData;
	bool		 mOpened;
	
	std::string  mLaunchStr;
	std::string  mCapsStr;
	std::string  mOutputPath;
	std::string  mOutputIP;
	uint32_t     mOutputPort;
	
	uint32_t     mWidth;
	uint32_t     mHeight;
};



#endif