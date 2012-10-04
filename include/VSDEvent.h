#ifndef __VSDEVENT_H__
#define __VSDEVENT_H__

#include "LPRStructure.h"
#include "VSDEventStructure.h"

#if defined(WIN32)
#ifdef VSDEVENT_EXPORTS
#define VSDEVENT_API __declspec(dllexport)
#else
#define VSDEVENT_API __declspec(dllimport)
#endif
#elif defined(LPR_LINUX)
#ifdef VSDEVENT_EXPORTS
#define VSDEVENT_API __attribute__ ((visibility("default")))
#else
#define VSDEVENT_API
#endif
#else
#error "No platform specified!"
#endif

struct LPRImage;

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
/// C接口的功能函数封装
//////////////////////////////////////////////////////////////////////////

/// 初始化VSDObject数据结构
VSDEVENT_API void __stdcall VSDObject_Init(VSDObject* pObject);
/// 初始化VSDObjectMulti数据结构
VSDEVENT_API void __stdcall VSDObjectMulti_Init(VSDObjectMulti* pObjectMulti);

/// 初始化VSDObjectTrack数据结构
VSDEVENT_API void __stdcall VSDObjectTrack_Init(VSDObjectTrack* pObjectMulti);
/// 初始化VSDObjectTrackMulti数据结构
VSDEVENT_API void __stdcall VSDObjectTrackMulti_Init(VSDObjectTrackMulti* pObjectMulti);

/// 获得初始化的默认参数
VSDEVENT_API VSDEventParam __stdcall VSDEvent_GetDefaultParam();
/// 从文件加载参数
VSDEVENT_API LPRRESULT __stdcall VSDEvent_LoadParam(const char* paramFile, VSDEventParam *pParam);
/// 将参数写入文件
VSDEVENT_API LPRRESULT __stdcall VSDEvent_SaveParam(const char* paramFile, VSDEventParam *pParam);

/// 辅助函数, 根据路面标定点, 获取矫正图像
//  参数:	pImage			--- 待矫正图像
//  参数:	ptRoad[4]		--- 路面标定点（相对坐标）
//  参数:	nWidthBase		--- x坐标基数
//  参数:   nHeightBase		--- y坐标基数
//  参数:	nImCaliWidth	--- 输出的矫正图像的宽度
//  参数:	nImCaliHeight	--- 输出的矫正图像的高度
//  参数：  nMargin			--- 输出的矫正图像的边缘留白
//  返回值：LPRImage*		--- 输出的矫正图像，宽度为nImCaliWidth+nMargin*2，高度为nImCaliHeight+nMargin*2
//								若为NULL, 表示获取矫正图像失败
VSDEVENT_API LPRImage * __stdcall VSDGetCalibrateImage(const LPRImage* pImage,
													   VSDRatioPoint ptRoad[4], int nWidthBase, int nHeightBase,
													   int nImCaliWidth, int nImCaliHeight,
													   int nMargin = 0);

/// 辅助函数，根据路面标定点和虚拟线圈的端点，获取车道线信息
//  参数：  ptRoad[4]		--- 路面标定点
//  参数：  virtualLoopLine[MAX_VIRTUAL_LOOPS]
//							--- 虚拟线圈位置(相对坐标)
//  参数:	nVirtualLoop	--- 虚拟线圈个数
//  参数:   nImWidth		--- 图像宽度
//  参数:   nImHeight		--- 图像高度
//  参数:	nWidthBase		--- x坐标基数
//  参数:   nHeightBase		--- y坐标基数
//  输出:   laneMark[MAX_VIRTUAL_LOOPS*2]
//							--- 车道线的端点(分别在图像顶端和图像底端)
//							--- 每个线圈对应左右两条车道线, 线圈i的车道线分别为laneMark[2*i], laneMark[2*i+1]
//  输出:	pPtLaneCross	--- 所有车道线延长线的交汇点
VSDEVENT_API LPRRESULT __stdcall VSDEvent_GenerateLaneMark(const VSDRatioPoint ptRoad[4],
														  const VSDRatioLine virtualLoopLine[MAX_VIRTUAL_LOOPS], const int nVirtualLoop,
														  int nImWidth, int nImHeight,
														  int nWidthBase, int nHeightBase,
														  VSDRatioLine laneMark[MAX_VIRTUAL_LOOPS*2], VSDRatioPoint *pPtLaneCross);
/// [待删除]辅助函数
VSDEVENT_API LPRRESULT __stdcall VSDCopySubImage(const LPRImage* pImSmall, LPRImage* pImLarge, const VSDRect& roi);



//////////////////////////////////////////////////////////////////////////
/// 以下函数提供的接口，主要供VSDEvent这个类调用，不推荐直接使用
//////////////////////////////////////////////////////////////////////////

VSDEVENT_API void* __stdcall VSDEvent_Create();
VSDEVENT_API void __stdcall VSDEvent_Destroy(void* pObject);
VSDEVENT_API LPRRESULT __stdcall VSDEvent_Init(void* pObject, const VSDEventParam& param);
VSDEVENT_API void __stdcall VSDEvent_Fini(void* pObject);
VSDEVENT_API bool __stdcall VSDEvent_IsInited(void* pObject);
VSDEVENT_API void __stdcall VSDEvent_ClearObjects(void* pObject);
VSDEVENT_API void __stdcall VSDEvent_ClearOneObject(void* pObject, int nID);

VSDEVENT_API LPRRESULT __stdcall VSDEvent_GetParam(void* pObject, VSDEventParam *pParam);
VSDEVENT_API LPRRESULT __stdcall VSDEvent_UpdateParam(void* pObject, const VSDEventParam& param);

VSDEVENT_API LPRRESULT __stdcall VSDEvent_AddLightMatcher(void* pObject, int nLightID,
														  LPRImage *pLightStatusTemplate[MAX_LIGHT_STATUS], int nNumTemplate,
														  const VSDRatioRECT& searchRect);
VSDEVENT_API void __stdcall VSDEvent_RemoveLightMatcher(void* pObject, int nLightID);
VSDEVENT_API void __stdcall VSDEvent_RemoveAllLightMatcher(void* pObject);
VSDEVENT_API LPRRESULT __stdcall VSDEvent_GetLightStatus(void* pObject, int nLightID, int *pLightStatus, VSDRect *pLightRect, int *pConfid);

VSDEVENT_API LPRRESULT __stdcall VSDEvent_ProcessFrame(void* pObject, const LPRImage* pImage, VSDObjectMulti* vsdObjects, const VSDEventControlParam* param = NULL);

VSDEVENT_API LPRRESULT __stdcall VSDEvent_GetTrack(void* pObject, int uid, VSDObjectTrack* pTrack);
VSDEVENT_API LPRRESULT __stdcall VSDEvent_GetAllTracks(void* pObject, VSDObjectTrackMulti* pAllTracks);

VSDEVENT_API LPRRESULT __stdcall VSDEvent_GetEventStatus(void *pObject, VSDEventStatus *pEventStatus);
/// 还未定义的几个API
//VSDEVENT_API LPRRESULT __stdcall VSDEvent_AddCaptureLine(void *pObject, const VSDRatioLine *pLine, bool bDoubleShoot);
//
//VSDEVENT_API LPRRESULT __stdcall VSDEVent_AddSpeedDetector(void* pObject,
//														   VSDRatioPoint ptMarkerOnImage[4],
//														   VSDPoint ptMarkerOnRoad[4]);
//VSDEVENT_API LPRRESULT __stdcall VSDEVent_GetSpeed(void* pObject, int uid, int* pSpeed);
///

//////////////////////////////////////////////////////////////////////////
/// 封装好的调用视频检测功能的核心类，建议使用
//////////////////////////////////////////////////////////////////////////
class VSDEvent
{
public:
	VSDEvent() { m_pObject = VSDEvent_Create(); }
	~VSDEvent() { VSDEvent_Destroy(m_pObject); }

	/// 初始化
	LPRRESULT Init(const VSDEventParam &param)
	{
		return VSDEvent_Init(m_pObject, param);
	}

	/// 获取初始化状态
	bool IsInited()
	{
		return VSDEvent_IsInited(m_pObject);
	}

	/// 释放内存
	void Fini()
	{
		VSDEvent_Fini(m_pObject);
	}

	/// 视频检测核心函数
	/// PC版本支持CS_JPEG和CS_RGB24的LPRImage
	LPRRESULT ProcessFrame(const LPRImage *pImage, VSDObjectMulti *trackObjects, const VSDEventControlParam* param = NULL)
	{
		return VSDEvent_ProcessFrame(m_pObject, pImage, trackObjects, param);
	}

	/// 辅助函数
	/// 获取当前使用的参数
	LPRRESULT GetParam(VSDEventParam *pParam)
	{
		return VSDEvent_GetParam(m_pObject, pParam);
	}

	/// 辅助函数
	/// 更新Event内部参数
	/// 注意：仅部分参数可被更新
	/// 目前支持：nEnhanceImage
	LPRRESULT UpdateParam(const VSDEventParam &param)
	{
		return VSDEvent_UpdateParam(m_pObject, param);
	}

	/// 辅助函数
	/// 获取当前画面特定ID号物体的历史轨迹
	LPRRESULT GetTrack(int uid, VSDObjectTrack* pTrack)
	{
		return VSDEvent_GetTrack(m_pObject, uid, pTrack);
	}

	/// 辅助函数，获取当前帧所有目标的历史轨迹
	LPRRESULT GetAllTracks(VSDObjectTrackMulti* pAllTracks)
	{
		return VSDEvent_GetAllTracks(m_pObject, pAllTracks);
	}

	/// 辅助函数，清空当前所有记录的物体
	void ClearObjects()
	{
		VSDEvent_ClearObjects(m_pObject);
	}

	/// 辅助函数，删除指定的物体
	void ClearOneObject(int nID)
	{
		VSDEvent_ClearOneObject(m_pObject, nID);
	}


	//////////////////////////////////////////////////////////////////////////
	/// 以下为红绿灯识别辅助函数
	/// 红绿灯识别功能是通过图像比对算法完成的。
	/// 使用前需要针对每一个信号灯，采集信号灯显示各种信号的图像模板，
	/// 然后指定在一个范围内进行搜索比对。
	/// 本模块可以输出比对结果和输入的哪一个状态的图像模板最相似，并且输出灯的确切位置。

	//*********************************************************************
	// 函数名:	AddTrafficLightMatcher
	// 说明:	配置并添加一个信号灯匹配器
	// 返回值:	LPRRESULT
	// 参数:	nLightID		--- 当前添加的信号灯的序号
	// 参数:	pLightTemplate	--- 灯的各个状态的图像模板
	// 参数:	nNumTemplate	--- 灯的图像模板数目
	// 参数:	searchRect		--- 信号灯比对的搜索范围
	//*********************************************************************
	LPRRESULT AddTrafficLightMatcher(int nLightID, LPRImage *pLightStatusTemplate[MAX_LIGHT_STATUS], int nNumTemplate,
									const VSDRatioRECT& searchRect)
	{
		return VSDEvent_AddLightMatcher(m_pObject, nLightID, pLightStatusTemplate, nNumTemplate, searchRect);
	}

	//*********************************************************************
	// 函数名：	GetLightStatus
	// 说明:	获取信号灯匹配结果
	// 返回值:	LPRRESULT
	// 参数:	nLightID		--- 信号灯的序号
	// 参数:	pLightStatus	--- 输出灯状态，取值为0~nNumTemplate - 1
	//						(nNumTemplate为AddTrafficLightMatcher的输入参数）
	// 参数:	pLightRect		--- 输出灯的精确位置，可能不同于初始位置
	// 参数:	pConfid			--- 输出匹配置信度，取值0~100
	//*********************************************************************
	LPRRESULT GetLightStatus(int nLightID, int *pLightStatus, VSDRect *pLightRect, int *pConfid)
	{
		return VSDEvent_GetLightStatus(m_pObject, nLightID, pLightStatus, pLightRect, pConfid);
	}
	
	//*********************************************************************
	// 函数名：	RemoveLightMatcher
	// 说明:	删除一个信号灯匹配器
	// 返回值:  void
	// 参数:	nLightID	--- 待删除的灯序号
	//*********************************************************************
	void RemoveLightMatcher(int nLightID)
	{
		VSDEvent_RemoveLightMatcher(m_pObject, nLightID);
	}

	//*********************************************************************
	// 函数名：	RemoveAllLightMatcher
	// 说明:	删除所有的信号灯匹配器
	// 返回值:  void
	// 参数:	void
	//*********************************************************************
	void RemoveAllLightMatcher()
	{
		VSDEvent_RemoveAllLightMatcher(m_pObject);
	}

	//*********************************************************************
	// 函数名：	GetEventStatus
	// 说明:	获取Event运行状态信息
	// 返回值:  LPRRESULT
	// 参数:	pEventStatus	--- 输出Event运行状态信息
	//*********************************************************************
	void GetEventStatus(VSDEventStatus *pEventStatus)
	{
		VSDEvent_GetEventStatus(m_pObject, pEventStatus);
	}


	/// 以下还未定义
	//////////////////////////////////////////////////////////////////////////
	/// 以下为测速辅助函数
	/// 初始化VSDEvent模块后，调用AddSpeedDetector初始化测速功能，
	/// 在系统运行的任意时刻，调用GetSpeed可以获得指定id的物体的速度信息
	/// 
	/// 注意：VSDEvent模块提供精度较低但是实时的速度信息，
	///       VSDEventCapture模块提供更精确的测速，但是滞后输出结果
	/// 
	/// 为了进行测速，每一次ProcessFrame函数调用时，
	/// 输入的LPRImage* pImage需要携带准确的时间戳信息
	///

	//*********************************************************************
	// 函数名：	AddSpeedDetector
	// 说明:	VSDEvent::AddSpeedDetector
	// 返回值:	LPRRESULT 
	// 参数:	VSDRatioPoint ptMarkerOnImage[4]
	//			图像上四个标定点的相对坐标
	// 参数:	VSDPoint ptMarkerOnRoad[4]
	//			路面上四个标定点的实际距离，单位为厘米
	//          暂不开放
	//*********************************************************************

	//LPRRESULT AddSpeedDetector(VSDRatioPoint ptMarkerOnImage[4], VSDPoint ptMarkerOnRoad[4])
	//{
	//	return VSDEVent_AddSpeedDetector(m_pObject, ptMarkerOnImage, ptMarkerOnRoad);
	//}

	//*********************************************************************
	// 函数名：	GetSpeed
	// 说明:	VSDEvent::GetSpeed
	// 返回值:	LPRRESULT 
	// 参数:	int uid
	//			需要测速的物体id
	// 参数:	int* nSpeed
	//			输出的速度，单位为公里每小时
	//          暂不开放
	//*********************************************************************
	//LPRRESULT GetSpeed(int uid, int* pSpeed)
	//{
	//	return VSDEVent_GetSpeed(m_pObject, uid, pSpeed);
	//}


private:
	/// 内部指针，用于封装
	void *m_pObject;
};

#ifdef __cplusplus
}
#endif


#endif	/// end of __VSDEVENT_H__

