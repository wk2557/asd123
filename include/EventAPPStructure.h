#ifndef __EVENT_APP_STRUCTURE_H_
#define __EVENT_APP_STRUCTURE_H_

#include "VSDEventStructure.h"
#include "LPRStructure.h"
#include "EventAPPConstant.h"

#if defined(WIN32)
#ifdef EVENTAPP_EXPORTS
#define EVENTAPP_API __declspec(dllexport)
#else
#define EVENTAPP_API __declspec(dllimport)
#endif
#elif defined(LINUX)
#ifdef EVENTAPP_EXPORTS
#define EVENTAPP_API __attribute__ ((visibility("default")))
#else
#define EVENTAPP_API
#endif
#else
#error "No platform specified!"
#endif

// 记录各种违章录像范围
struct EventRecordParam
{
	int mBreakRuleAhead[RULE_TYPES];   // VSD_BR_TURN_LEFT 到 VSD_BR_LOW_SPEED 9种违章，数组中的每一项分别记录对于违章行为，应提前多少帧进行录像
	int mBreakRuleBehind[RULE_TYPES];  // VSD_BR_TURN_LEFT 到 VSD_BR_LOW_SPEED 9种违章，数组中的每一项分别记录对于违章行为，应录像到违章行为后的多少帧 
	int mViedoFormat;				   // 视频格式，取值范围为为EventAPPViedoFormat
	int mBitFrequent;				   // 视频比特率
	wchar_t mDirPath[MAX_DIR_PATH];    // 保存视频和图像路径
};

struct EventFont 
{
	int mFontSize;
	int mFontFamily;
	COLORREF mFontColor;
	int mFontOrientation;
	int mFontX;
	int mFontY;
};

struct EventImageSynthesis
{
	int mNumberofImage;
	int mPicOrientation;
};

// EventAPP参数
struct EventAPPParam
{
	VSDEventParam mVSDParam;			// VSDEvent的参数
	VSDRatioLine mleftTurnLine;			// 左转线标志线，禁止左拐时跨过为违章
	VSDRatioLine mRightTurnLine;		// 右转线标志线，禁止右拐时跨过为违章
	VSDRatioLine mStraightLine;			// 直行标志线，越过改线后禁止左右转
	EventRecordParam mRecordParam;		// 各种违章情况下违章录像情况
	double mRatioToCrossLine;			// 车道压线比例阈值，超过这个阈值判定压道行驶
	EventFont mFont;
	EventImageSynthesis mImageSynthesis;
};


// EventAPP处理图片的输出结果
struct EventAPPResult
{
	int mID;															// 物体的ID
	int mNumOfImage;													// 图像的个数
	wchar_t mPlate[LPR_PLATE_STR_LEN];
	VSDBreakRule mBreakRule;											// ID物体违反的规则
	LPRImage* mImage[MAX_FRAM_AHEAD + MAX_FRAM_BHEIND];					// 物体的图片序列
};


// 多个APPResult
struct EventMultiAPPResult
{
	int mNumOfResult;													// APPResult的个数
	EventAPPResult mppAPPResult[MAX_OBJECTS * RULE_TYPES];				// 指向APPResult指针的数组
};


EVENTAPP_API void __stdcall FreeMultiAPPResult(EventMultiAPPResult* ipMultiAPPResult);
EVENTAPP_API void __stdcall FreeAPPResult(EventAPPResult* ipAPPResult);
EVENTAPP_API APPRESULT __stdcall EventAPP_LoadParam(const char* ipFileName, EventAPPParam* ipEventParam);

#endif
