//*********************************************************
//This file defines the basic interface of EventAPP
//Author:WK
//Data:2012.09.22
//**********************************************************

#ifndef __EVENT_APP_H_
#define __EVENT_APP_H_

#include "EventAPPConstant.h"
#include "VSDEvent.h"
#include "EventAPPStructure.h"

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

#ifdef __cplusplus
extern "C"{
#endif

// EventAPP 处理图片序列并得到车辆的违章信息，输出有违章的图片序列
class EVENTAPP_API EventAPP
{
public:
	EventAPP();
	~EventAPP();

	// 初始化EventAPP，在调用其他接口函数之前，此函数应该已经调用过
	// 输入参数 irParam: 表示EventAPP需要的参数设置，见EventAPPParam的表述
	APPRESULT Init(const EventAPPParam& irParam);

	// 处理图片流的主要接口。
	// 输入参数 ipImage: 要处理的图像指针
	// 输入参数 ipObjectMulti: 通过VSDEvent::ProcssFram得到的输出结果
	// 输入参数 isRedLightOn: 表示当前帧红灯是否是亮
	// 输出参数 opResult: 输出到当前图片为止，刚刚离开跟踪区域的车辆的图片序列以及是否违章等信息，见EventMultiAPPResult的表述
	APPRESULT ProcessFram(LPRImage* ipImage, const VSDObjectMulti* ipObjectMulti,const VSDObjectTrackMulti* ipObjectTrackMult, int iLightStatus[MAX_VIRTUAL_LOOPS], EventMultiAPPResult* opResult); 

	// 给图片加指定的字幕
	// 输入参数 ipImage: 需要叠加字幕的源图片
	// 输入参数 ipString: 需要叠加的字幕
	// 输出参数 oppImage: 叠加好字幕的图片
	APPRESULT AddSubTitle(LPRImage* ipImage, const wchar_t* ipString, const EventSubtitleImages* ipSubtitleImages ,LPRImage** oppImage);

	// 把指定数目个图片合成一张图片
	// 输入参数 ipImage: 指向输入图片的数组
	// 输入参数 iNumOfImages: 输入图片的个数
	// 输入参数 irRect: 因为需要将ipImage所指的第一个图片的车的位置放大，所以irRect代表车所在的范围
	// 输出参数 oppImage; 指向合成的图片指针的指针
	APPRESULT SynthesisImages(LPRImage** ipImage, int iNumOfImages, const VSDRect& irRect, LPRImage** oppImage);

	// 把指定数目的图片合成视频
	// 输入参数 ipImage: 指向输入图片的数组
	// 输入参数 iNumOfImage: 输入图片的数目
	// 输出参数 orMeida: 输出的视频
	APPRESULT Convert2Media(LPRImage** ipImage, int iNumOfImages, EventMedia& orMedia);

private:
	
	APPRESULT ConstructResult(int iObjectBreakRule, int iRuleType,int uid, EventMultiAPPResult* opResultMulti, int& orResultCount);
	// We don't want user to copy this object
	EventAPP(const EventAPP&);
	EventAPP& operator=(const EventAPP&);
	// Hide the implementation
	void* mObject;

};

#ifdef __cplusplus
}
#endif

#endif