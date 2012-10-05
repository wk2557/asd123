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
	APPRESULT ProcessFram(LPRImage* ipImage, const VSDObjectMulti* ipObjectMulti, int iLightStatus[MAX_VIRTUAL_LOOPS], EventMultiAPPResult* opResult); 

	// 
	APPRESULT AddSubTitle(LPRImage* ipImage, const std::string& irString, LPRImage** opImage);

private:
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