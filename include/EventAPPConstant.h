#ifndef __EVENT_APP_CONSTANT_H_
#define __EVENT_APP_CONSTANT_H_

enum EventAPPLightStatus
{
	EVENT_APP_LIGHT_RED			= 0,
	EVENT_APP_LIGHT_GREEN		= 1,
	EVENT_APP_LIGHT_YELLOW		= 2,
};

enum EventAPPViedoFormat
{
	EVENT_APP_VIEDO_MJPG		= 0,
	EVENT_APP_VIEDO_H264		= 1,
};

typedef int APPRESULT;

#define APP_OK						     0

#define APP_FAIL						-1
#define APP_INPUT_NULL_POINTER			-2
#define APP_OUT_OF_MEMORY				-3
#define APP_FILE_NOT_FOUND				-4
#define APP_CONFIG_PARAM_FAULT			-5
#define APP_VSD_PARAM_FAULT				-6
#define APP_IMAGE_FORMAT_FAULT			-7


#define RULE_TYPES						9
#define MAX_FRAM_AHEAD					40
#define MAX_FRAM_BHEIND					20
#define DEFAULT_PLATE_MIN_WIDTH			70
#define DEFAULT_PLATE_MAX_WIDTH			300	
#define DEFAULT_NUMBER_PLATE			1	
#define MAX_DIR_PATH					1024	

#endif