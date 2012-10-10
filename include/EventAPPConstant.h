#ifndef __EVENT_APP_CONSTANT_H_
#define __EVENT_APP_CONSTANT_H_

enum EventAPPFontFamily
{
	EVENT_APP_FONT_SONG			= 0,           // 宋体
	EVENT_APP_FONT_HEI			= 1,           // 黑体
};

enum EventAPPFontOrientation
{
	EVENT_APP_FONT_VERTICAL		= 0,		   // 字体垂直摆放
	EVENT_APP_FONT_HORIZONTAL	= 1,		   // 字体水平摆放
};

enum EventAPPPicOrientation
{
	EVENT_APP_PIC_VERTICAL		= 0,		   // 原始图片竖直着合成新的图片
	EVENT_APP_PIC_HORIZONTAL	= 1,		   // 水平着合成新的图片
	EVENT_APP_PIC_GRID			= 2,		   // 田字型的合成新的图片
};

enum EventAPPLightStatus
{
	EVENT_APP_LIGHT_RED			= 0,			// 红灯
	EVENT_APP_LIGHT_GREEN		= 1,			// 绿灯
	EVENT_APP_LIGHT_YELLOW		= 2,			// 黄灯
};

enum EventAPPVideoFormat
{
	EVENT_APP_VIDEO_MJPG		= 0,			// 合成视频的格式为MJPG
	EVENT_APP_VIDEO_H264		= 1,			// 合成视频的格式为H264
};

enum EventAPPRuleSwitch
{
	EVENT_APP_RULE_SWITCH_OFF         = 0,
	EVENT_APP_RULE_SWITCH_ON		= 1,
};

enum EventAPPPlateCaptureSwitch
{
	EVENT_APP_PLATE_SWITCH_ON      = 0,
	EVENT_APP_PLATE_SWITCH_OFF     = 1,
};

typedef int APPRESULT;

#define APP_OK						     0		// 成功

#define APP_FAIL						-1		// 失败，未知原因
#define APP_INPUT_NULL_POINTER			-2		// 失败，原因输入为空指针
#define APP_OUT_OF_MEMORY				-3		// 失败，原因内存分配失败
#define APP_FILE_NOT_FOUND				-4		// 失败，原因是没找到配置文件
#define APP_CONFIG_PARAM_FAULT			-5		// 失败，原因是配置文件里参数格式不对
#define APP_VSD_PARAM_FAULT				-6		// 失败，原因是VSD配置参数不对
#define APP_IMAGE_FORMAT_FAULT			-7		// 失败，原因是输入文件的格式不对


#define RULE_TYPES						10		// 违反规则的个数，不包括VSD_BR_NONE
#define MAX_FRAME_AHEAD					40		// 违规前的最大录制帧数
#define MAX_FRAME_BEHIND					20		// 违规后的最大录制帧数
#define DEFAULT_PLATE_MIN_WIDTH			70		// 车牌的最小宽度，供车牌识别用
#define DEFAULT_PLATE_MAX_WIDTH			300		// 车牌的最大宽度，供车牌识别用
#define DEFAULT_NUMBER_PLATE			1		// 车牌的数目，供车牌识别用
#define MAX_CHARACTOR_NUM				100     // 字幕中最大的字符数目
#define MAX_SYNTHESIS_NUM				3		// 合成大图片的原图片最多数目
#define DEFAULT_EXTRA_IMAGE_BUFFER			100

#endif