#ifndef __EVENT_APP_STRUCTURE_H_
#define __EVENT_APP_STRUCTURE_H_

#include "VSDEventStructure.h"
#include "LPRStructure.h"
#include "EventAPPConstant.h"
#include <inttypes.h>

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
};

// 返回的视频结构
struct EventMedia
{
	uint8_t *mBufferPtr;			  // 指向视频的buffer
	int mBufferSize;				  // 视频buffer的大小
};

// 合成图片的字幕的字体信息
struct EventFont 
{
	wchar_t mCharactors[MAX_CHARACTOR_NUM]; // 字幕的字符集
	int mFontSize;					   // 字体大小
	int mFontFamily;				   // 字体格式
	COLORREF mFontColor;			   // 字体颜色
	int mFontOrientation;			   // 字体排放朝向，取值为范围为EventAPPFontOrientation
	int mFontX;						   // 字体起始位置的X坐标
	int mFontY;                        // 字体起始位置的Y坐标
};

// N张图片合成一张
struct EventImageSynthesis
{
	int mNumberofImage;				   // 合成图片的数目
	int mPicOrientation;               // 图片的摆放，取值范围为EventAPPPicOrientation
	double mZoonRatio;                 // 原始图片的缩放比例
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
	EventFont mFont;					// 合成图片的字母信息
	EventImageSynthesis mImageSynthesis;// 将N张图片合成一个图片的参数
};


// EventAPP处理图片的输出结果
struct EventAPPResult
{
	int mID;															// 物体的ID
	int mNumOfViedoImage;												// 用于合成录像的图像的个数
	int mNumOfSynthesisImage;											// 用于合成大图片的原始图片个数
	wchar_t mPlate[LPR_PLATE_STR_LEN];									// 物体的车牌号
	VSDBreakRule mBreakRule;											// ID物体违反的规则
	LPRImage* mViedoImage[MAX_FRAM_AHEAD + MAX_FRAM_BHEIND];			// 物体的图片序列，如果mBreakRule是VSD_BR_NONE，那么mNumberOfImage的值一定是1，保存其位于停车线附近的照片
																		// 如果mBreakRule不是VSD_BR_NONE，那么前MAX_FRAM_AHEAD + MAX_FRAM_BEHIND张用来合成录像，后面三张分别是接触停车线，离开停车线，违章时的三张图片
	LPRImage* mSynthesisImage[MAX_SYNTHESIS_NUM];
	VSDRect mRect;														// 用于保存物体在mSynthesisImage的第一个图片中的位置，用于取放大图片的位置用 
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
