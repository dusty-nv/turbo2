/*
 * rover
 */

#include "videoEncoder.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include <sstream> 
#include <algorithm>
#include <time.h>


//------------------------------------------------------------------------------
static void gst_print_one_tag(const GstTagList * list, const gchar * tag, gpointer user_data)
{
  int i, num;

  num = gst_tag_list_get_tag_size (list, tag);
  for (i = 0; i < num; ++i) {
    const GValue *val;

    /* Note: when looking for specific tags, use the gst_tag_list_get_xyz() API,
     * we only use the GValue approach here because it is more generic */
    val = gst_tag_list_get_value_index (list, tag, i);
    if (G_VALUE_HOLDS_STRING (val)) {
      printf("\t%20s : %s\n", tag, g_value_get_string (val));
    } else if (G_VALUE_HOLDS_UINT (val)) {
      printf("\t%20s : %u\n", tag, g_value_get_uint (val));
    } else if (G_VALUE_HOLDS_DOUBLE (val)) {
      printf("\t%20s : %g\n", tag, g_value_get_double (val));
    } else if (G_VALUE_HOLDS_BOOLEAN (val)) {
      printf("\t%20s : %s\n", tag,
          (g_value_get_boolean (val)) ? "true" : "false");
    } else if (GST_VALUE_HOLDS_BUFFER (val)) {
      GstBuffer *buf = gst_value_get_buffer (val);
      guint buffer_size = GST_BUFFER_SIZE(buf);

      printf("\t%20s : buffer of size %u\n", tag, buffer_size);
    } /*else if (GST_VALUE_HOLDS_DATE_TIME (val)) {
      GstDateTime *dt = (GstDateTime*)g_value_get_boxed (val);
      gchar *dt_str = gst_date_time_to_iso8601_string (dt);

      printf("\t%20s : %s\n", tag, dt_str);
      g_free (dt_str);
    }*/ else {
      printf("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
    }
  }
}

static const char* gst_stream_status_string( GstStreamStatusType status )
{
	switch(status)
	{
		case GST_STREAM_STATUS_TYPE_CREATE:	return "CREATE";
		case GST_STREAM_STATUS_TYPE_ENTER:		return "ENTER";
		case GST_STREAM_STATUS_TYPE_LEAVE:		return "LEAVE";
		case GST_STREAM_STATUS_TYPE_DESTROY:	return "DESTROY";
		case GST_STREAM_STATUS_TYPE_START:		return "START";
		case GST_STREAM_STATUS_TYPE_PAUSE:		return "PAUSE";
		case GST_STREAM_STATUS_TYPE_STOP:		return "STOP";
		default:							return "UNKNOWN";
	}
}

// gst_message_print
gboolean gst_message_print(GstBus* bus, GstMessage* message, gpointer user_data)
{

	switch (GST_MESSAGE_TYPE (message)) 
	{
		case GST_MESSAGE_ERROR: 
		{
			GError *err = NULL;
			gchar *dbg_info = NULL;
 
			gst_message_parse_error (message, &err, &dbg_info);
			printf( "gstreamer %s ERROR %s\n", GST_OBJECT_NAME (message->src), err->message);
        		printf( "gstreamer Debugging info: %s\n", (dbg_info) ? dbg_info : "none");
        
			g_error_free(err);
        		g_free(dbg_info);
			//g_main_loop_quit (app->loop);
        		break;
		}
		case GST_MESSAGE_EOS:
		{
			printf( "gstreamer %s recieved EOS signal...\n", GST_OBJECT_NAME(message->src));
			//g_main_loop_quit (app->loop);		// TODO trigger plugin Close() upon error
			break;
		}
		case GST_MESSAGE_STATE_CHANGED:
		{
			GstState old_state, new_state;
    
			gst_message_parse_state_changed(message, &old_state, &new_state, NULL);
			
			printf( "gstreamer changed state from %s to %s ==> %s\n",
							gst_element_state_get_name(old_state),
							gst_element_state_get_name(new_state),
						     GST_OBJECT_NAME(message->src));
			break;
		}
		case GST_MESSAGE_STREAM_STATUS:
		{
			GstStreamStatusType streamStatus;
			gst_message_parse_stream_status(message, &streamStatus, NULL);
			
			printf( "gstreamer stream status %s ==> %s\n",
							gst_stream_status_string(streamStatus), 
							GST_OBJECT_NAME(message->src));
			break;
		}
		case GST_MESSAGE_TAG: 
		{
			GstTagList *tags = NULL;

			gst_message_parse_tag(message, &tags);

#ifdef gst_tag_list_to_string
			gchar* txt = gst_tag_list_to_string(tags);
#else
			gchar* txt = "missing gst_tag_list_to_string()";
#endif

			printf( "gstreamer %s %s\n", GST_OBJECT_NAME(message->src), txt);

			g_free(txt);			
			//gst_tag_list_foreach(tags, gst_print_one_tag, NULL);
			gst_tag_list_free(tags);
			break;
		}
		default:
		{
			printf( "gstreamer msg %s ==> %s\n", gst_message_type_get_name(GST_MESSAGE_TYPE(message)), GST_OBJECT_NAME(message->src));
			break;
		}
	}

	return TRUE;
}

std::string strFileExtension( const std::string& path )
{
	std::string ext = path.substr(path.find_last_of(".") + 1);
	transform(ext.begin(), ext.end(), ext.begin(), tolower);
	return ext;
}

void sleep_ms( uint64_t milliseconds )
{
	timespec duration;
	duration.tv_sec  = 0;
	duration.tv_nsec = milliseconds * 1000 * 1000;
	nanosleep(&duration, NULL);
}

//------------------------------------------------------------------------------


// constructor
videoEncoder::videoEncoder( uint32_t width, uint32_t height )
{	
	mAppSrc      = NULL;
	mBus         = NULL;
	mBufferCaps  = NULL;
	mPipeline    = NULL;
	mNeedData    = false;
	mInputBuffer = NULL;
	mOutputPort  = 0;	
	mOpened      = false;
	mWidth       = width;
	mHeight      = height;
}


// destructor	
videoEncoder::~videoEncoder()
{
	Close();
	
	if( mInputBuffer != NULL )
	{
		gst_buffer_unref(mInputBuffer);
		mInputBuffer = NULL;
	}
}


// onNeed
void videoEncoder::onNeed(GstElement * pipeline, guint size, gpointer user_data)
{
	//printf("gstreamer appsrc requesting data (%u bytes)\n", size);
	
	if( !user_data )
		return;

	videoEncoder* enc = (videoEncoder*)user_data;
	enc->mNeedData  = true;
}
 
 
 
// onEnough
void videoEncoder::onEnough(GstElement * pipeline, gpointer user_data)
{
	//printf("gstreamer appsrc signalling enough data\n");

	if( !user_data )
		return;

	videoEncoder* enc = (videoEncoder*)user_data;
	enc->mNeedData  = false;
}


// ProcessBuffer
bool videoEncoder::ProcessFrame( uint8_t* buffer )
{
	if( !buffer )
		return false;

	if( !mNeedData )
		return true;


	GST_BUFFER_DATA(mInputBuffer) = buffer;
	
	//static size_t num_frame = 0;
	//GST_BUFFER_TIMESTAMP(gstBuffer) = (GstClockTime)((num_frame / 30.0) * 1e9);
	//num_frame++;
	
	// queue buffer to gstreamer
	//GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(mAppSrc), gstBuffer);
	GstFlowReturn ret;	
	g_signal_emit_by_name(mAppSrc, "push-buffer", mInputBuffer, &ret);
     
	if( ret != 0 )
		printf( "gstreamer -- AppSrc pushed buffer (result %u)\n", ret);

	// check messages
	while(true)
	{
		GstMessage* msg = gst_bus_pop(mBus);

		if( !msg )
			break;

		gst_message_print(mBus, msg, this);
		gst_message_unref(msg);
	}
	
	return true;
}



//#define CAPS_STR "video/x-raw-rgb,width=640,height=480,bpp=24,depth=24"
//#define CAPS_STR "video/x-raw-yuv,width=640,height=480,format=(fourcc)I420"
//  #define CAPS_STR "video/x-raw-yuv,width=1280,height=720,format=(fourcc)I420,framerate=30/1"
//#define CAPS_STR "video/x-raw-gray,width=640,height=480,bpp=8,depth=8,framerate=30/1"


bool videoEncoder::buildCapsStr()
{
	std::ostringstream ss;
   
	ss << "video/x-raw-yuv,width=" << mWidth << ",height=" << mHeight;
	ss << ",format=(fourcc)I420,framerate=30/1";
	
	mCapsStr = ss.str();
	
	printf( "gstreamer encoder caps string:\n");
	printf("%s\n", mCapsStr.c_str());
	return true;
}

	
// buildLaunchStr
bool videoEncoder::buildLaunchStr()
{
	const size_t fileLen = mOutputPath.size();
	const size_t ipLen   = mOutputIP.size();

	std::ostringstream ss;
    //ss << "appsrc name=mysource ! " CAPS_STR " ! ";
	ss << "appsrc name=mysource ! ";
	ss << "nv_omx_h264enc quality-level=2 ! video/x-h264 ! ";

	if( fileLen > 0 && ipLen > 0 )
		ss << "tee name=t ! ";

	if( fileLen > 0 ) 
	{
		std::string ext = strFileExtension(mOutputPath);

		if( strcasecmp(ext.c_str(), "mkv") == 0 )
		{
			//ss << "matroskamux ! queue ! ";
			ss << "matroskamux ! ";
		}
		else if( strcasecmp(ext.c_str(), "h264") != 0 )
		{
			printf( "gstreamer invalid output extension %s\n", ext.c_str());
			return false;
		}

		ss << "filesink location=" << mOutputPath;

		if( ipLen > 0 )
			ss << " t. ! ";	// begin the second tee
	}

	if( ipLen > 0 )
	{
		ss << "rtph264pay config-interval=1 ! udpsink host=";
		ss << mOutputIP << " ";

		if( mOutputPort != 0 )
			ss << "port=" << mOutputPort;

		ss << " auto-multicast=true";
	}

	mLaunchStr = ss.str();

	printf( "gstreamer encoder pipeline string:\n");
	printf("%s\n", mLaunchStr.c_str());
	return true;
}


/*const char* launchStr = "appsrc name=mysource ! " CAPS_STR " ! "
					    //"nvvidconv ! nv_omx_h264enc quality-level=2 ! "
					    "nv_omx_h264enc ! video/x-h264 ! "
					    "matroskamux ! queue ! "
				         "filesink location=/media/ubuntu/SDU11/test.mkv";*/
						 
// init
bool videoEncoder::init()
{
	if( mWidth == 0 || mHeight == 0 )
	{
		printf( "gstreamer -- invalid width/height (%u x %u)\n", mWidth, mHeight);
		return false;
	}

	// build pipeline string
	if( !buildLaunchStr() || !buildCapsStr() )
	{
		printf( "gstreamer failed to build pipeline string\n");
		return false;
	}
	
	// launch pipeline
	GError* err = NULL;
	mPipeline   = gst_parse_launch(mLaunchStr.c_str(), &err);

	if( err != NULL )
	{
		printf( "gstreamer failed to create pipeline\n");
		printf( "   (%s)\n", err->message);
		g_error_free(err);
		return false;
	}

	GstPipeline* pipeline = GST_PIPELINE(mPipeline);

	if( !pipeline )
	{
		printf( "gstreamer failed to cast GstElement into GstPipeline\n");
		return false;
	}	

	// retrieve pipeline bus
	/*GstBus**/ mBus = gst_pipeline_get_bus(pipeline);

	if( !mBus )
	{
		printf( "gstreamer failed to retrieve GstBus from pipeline\n");
		return false;
	}

	// add watch for messages (disabled when we poll the bus ourselves, instead of gmainloop)
	//gst_bus_add_watch(mBus, (GstBusFunc)gst_message_print, NULL);

	// get the appsrc
	GstElement* appsrcElement = gst_bin_get_by_name(GST_BIN(pipeline), "mysource");
	GstAppSrc* appsrc = GST_APP_SRC(appsrcElement);

	if( !appsrcElement || !appsrc )
	{
		printf( "gstreamer failed to retrieve AppSrc element from pipeline\n");
		return false;
	}
	
	mAppSrc = appsrcElement;

	g_signal_connect(appsrcElement, "need-data", G_CALLBACK(onNeed), this);
	g_signal_connect(appsrcElement, "enough-data", G_CALLBACK(onEnough), this);

	/*GstCaps* caps = gst_caps_new_simple("video/x-raw-rgb",
								 "bpp",G_TYPE_INT,24,
								 "depth",G_TYPE_INT,24,
								 "width", G_TYPE_INT, 640,
								 "height", G_TYPE_INT, 480,
								 NULL);*/
	mBufferCaps = gst_caps_from_string(mCapsStr.c_str());

	if( !mBufferCaps )
	{
		printf( "gstreamer failed to parse caps from string\n");
		return false;
	}

	// convert to GstBuffer
	mInputBuffer = gst_buffer_new();

	//GST_BUFFER_MALLOCDATA(gstBuffer) = (guint8*)g_malloc(size);
	//GST_BUFFER_DATA(mInputBuffer) = GST_BUFFER_MALLOCDATA(gstBuffer);
	GST_BUFFER_SIZE(mInputBuffer) = (mWidth * mHeight * 12) / 8;

	if( mBufferCaps != NULL )
		gst_buffer_set_caps(mInputBuffer, mBufferCaps);



	//gst_app_src_set_caps(appsrc, mBufferCaps);
	//gst_app_src_set_size(appsrc, 640*480*10);
	//gst_app_src_set_max_bytes(appsrc, 640*480*20);
	gst_app_src_set_stream_type(appsrc, GST_APP_STREAM_TYPE_STREAM);
	//gst_app_src_set_latency(appsrc, 1, 20);

	//g_object_set(G_OBJECT(m_pAppSrc), "caps", m_pCaps, NULL); 
	g_object_set(G_OBJECT(mAppSrc), "is-live", TRUE, NULL); 
	//g_object_set(G_OBJECT(mAppSrc), "block", FALSE, NULL); 
	g_object_set(G_OBJECT(mAppSrc), "do-timestamp", TRUE, NULL); 

	return true;
}


// Open
bool videoEncoder::Open()
{
	if( mOpened )
		return true;
		
	printf( "gstreamer transitioning pipeline to GST_STATE_PLAYING\n");
	const GstStateChangeReturn result = gst_element_set_state(mPipeline, GST_STATE_PLAYING);

	if( result == GST_STATE_CHANGE_ASYNC )
	{
#if 0
		GstMessage* asyncMsg = gst_bus_timed_pop_filtered(mBus, 5 * GST_SECOND, 
    	 					      (GstMessageType)(GST_MESSAGE_ASYNC_DONE|GST_MESSAGE_ERROR)); 

		if( asyncMsg != NULL )
		{
			gst_message_print(mBus, asyncMsg, this);
			gst_message_unref(asyncMsg);
		}
		else
			printf( "gstreamer NULL message after transitioning pipeline to PLAYING...\n");
#endif
	}
	else if( result != GST_STATE_CHANGE_SUCCESS )
	{
		printf( "gstreamer failed to set pipeline state to PLAYING (error %u)\n", result);
		return false;
	}

	mOpened = true;
	return true;
}
	


// Close
bool videoEncoder::Close()
{
	if( !mOpened )
		return true;
		
	// send EOS
	mNeedData = false;
	
	printf( "gstreamer sending encoder EOS\n");
	GstFlowReturn eos_result = gst_app_src_end_of_stream(GST_APP_SRC(mAppSrc));

	if( eos_result != 0 )
		printf( "gstreamer failed sending appsrc EOS (result %u)\n", eos_result);

	sleep_ms(250);

	// stop pipeline
	printf( "gstreamer transitioning pipeline to GST_STATE_NULL\n");

	const GstStateChangeReturn result = gst_element_set_state(mPipeline, GST_STATE_NULL);

	if( result != GST_STATE_CHANGE_SUCCESS )
		printf( "gstreamer failed to set pipeline state to PLAYING (error %u)\n", result);

	sleep_ms(250);
	mOpened = false;
	return true;
}
