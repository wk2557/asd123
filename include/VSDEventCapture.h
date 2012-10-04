#ifndef __VSDEVENT_CAPTURE_H__
#define __VSDEVENT_CAPTURE_H__

/// 视频检测抓拍合成模块，完成对抓拍图像的识别和拼图工作

#include <VSDEvent.h>

#if defined(WIN32)
#ifdef VSDEVENTCAPTURE_EXPORTS
#define VSDEVENTCAPTURE_API __declspec(dllexport)
#else
#define VSDEVENTCAPTURE_API __declspec(dllimport)
#endif
#elif defined(LPR_LINUX)
#ifdef VSDEVENTCAPTURE_EXPORTS
#define VSDEVENTCAPTURE_API __attribute__ ((visibility("default")))
#else
#define VSDEVENTCAPTURE_API
#endif
#else
#error "No platform specified!"
#endif

enum VSDEventCapMode
{
	CAPMOD_LOCAL = 0,				/// 本地模式，用于软件版本
	CAPMOD_NETWORK = 1				/// 网络模式，用于相机版本
};

#ifdef __cplusplus
extern "C" {
#endif

struct VSDCaptureParam
{
	int			nMode;				/// 取值为VSDEventCapMode
	int			nCombineMode;		/// 图像合成模式
									/// 0:关闭 1:四格彩色 2:四格高清
	int			nRec;				/// 是否对抓拍进行识别
									/// 0:不识别 1:识别

	int			pReserved[32];		/// 保留
};

/// 描述抓拍的数据结构
struct CaptureRec 
{
	int			nObjID;				/// 物体ID
	int			nBreakRule;			/// 违章类型
	LPRImage*	pCombinedCapture;	/// pCombinedCapture的nColorMode为CS_RGB24
	LPRImage*	pCaptures[MAX_CAPTURE_PER_OBJ];	/// pCapture的nColorMode为CS_JPEG
	LPROutput	recOutput;			/// 识别结果
	float		fSpeed;				/// 速度，公里/小时
	int			pReserved[32];		/// 保留
};

struct CaptureRec_Multi
{
	int nCapRecs;
	CaptureRec capRec[MAX_OBJECTS];
};

VSDEVENTCAPTURE_API void* __stdcall VSDEventCapture_Create();
VSDEVENTCAPTURE_API void __stdcall VSDEventCapture_Destroy(void* pObject);
VSDEVENTCAPTURE_API LPRRESULT __stdcall VSDEventCapture_Init(void* pObject, const VSDEventParam &eventParam, const VSDCaptureParam &capParam);
VSDEVENTCAPTURE_API bool __stdcall VSDEventCapture_IsInited(void* pObject);
VSDEVENTCAPTURE_API void __stdcall VSDEventCapture_Fini(void* pObject);

/// 输入给VSDEventCapture的接口
/// LPRImage的数据类型：DSP版本为CS_RGB24，PC版本为CS_JPEG
/// 参数pTrackObjects只需输入VSDEvent_ProcessFrame的输出结果即可
/// 内部会根据VSDObject的status字段判断是否缓存图像
VSDEVENTCAPTURE_API LPRRESULT __stdcall VSDEventCapture_FeedCapture(void* pObject, const LPRImage* pImage, VSDObjectMulti *pTrackObjects);
VSDEVENTCAPTURE_API LPRRESULT __stdcall VSDEventCapture_RequestCapRec(void* pObject, int nObjectID, int nTotalCaps);
VSDEVENTCAPTURE_API LPRRESULT __stdcall VSDEventCapture_GetCapRec(void* pObject, CaptureRec_Multi *pCapRecMulti);

VSDEVENTCAPTURE_API void __stdcall VSDEventCapture_EnableRec(void* pObject, int nEableRec);

VSDEVENTCAPTURE_API LPRRESULT __stdcall VSDEventCapture_AddSpeedDetector(void* pObject, VSDRatioPoint ptMarkerOnImage[4], VSDPoint ptMarkerOnRoad[4]);

class VSDEventCapture
{
public:
	VSDEventCapture() { m_pObject = VSDEventCapture_Create(); }
	~VSDEventCapture() { VSDEventCapture_Destroy(m_pObject); }

	LPRRESULT Init(const VSDEventParam &eventParam, const VSDCaptureParam &capParam)
	{
		return VSDEventCapture_Init(m_pObject, eventParam, capParam);
	}

	void IsInited()
	{
		VSDEventCapture_IsInited(m_pObject);
	}

	void Fini()
	{
		VSDEventCapture_Fini(m_pObject);
	}

	LPRRESULT FeedCapture(const LPRImage *pImage, VSDObjectMulti *pTrackObjects)
	{
		return VSDEventCapture_FeedCapture(m_pObject, pImage, pTrackObjects);
	}
	LPRRESULT RequestCapRec(int nObjectID, int nTotalCaps)
	{
		return VSDEventCapture_RequestCapRec(m_pObject, nObjectID, nTotalCaps);
	}

	LPRRESULT GetCaptureRec(CaptureRec_Multi *pCapRecMulti)
	{
		return VSDEventCapture_GetCapRec(m_pObject, pCapRecMulti);
	}

	void EnableRec(int nEnableRec)
	{
		VSDEventCapture_EnableRec(m_pObject, nEnableRec);
	}

	LPRRESULT AddSpeedDetector(VSDRatioPoint ptMarkerOnImage[4],
								VSDPoint ptMarkerOnRoad[4])
	{
		return VSDEventCapture_AddSpeedDetector(m_pObject, ptMarkerOnImage, ptMarkerOnRoad);
	}

private:
	void*	m_pObject;
};

#if 0
class VSDEventEx
{
public:
	VSDEventEx() {}
	~VSDEventEx() {}

	LPRRESULT Init(const VSDEventParam &eventParam, const VSDCaptureParam &capParam)
	{
		LPRRESULT lr = LPR_OK;
		lr = m_event.Init(eventParam);
		if ( lr != LPR_OK )
			return lr;

		lr = m_capture.Init(eventParam, capParam);
		if ( lr != LPR_OK )
			return lr;

		return lr;
	}

	void Fini()
	{
		m_event.Fini();
		m_capture.Fini();
	}

	LPRRESULT ProcessFrame(const LPRImage *pImage, const VSDEventControlParam *param, VSDObjectMulti *trackObjects)
	{
		LPRRESULT lr = m_event.ProcessFrame(pImage, trackObjects, param);

		if ( lr == LPR_OK )
		{
			lr = m_capture.FeedCapture(pImage, trackObjects);
			
			for ( int i = 0; i < trackObjects->nObjects; i++ )
			{
				if ( trackObjects->objects[i].status & VSD_OBJ_STATUS_DELETE )
				{
					m_capture.RequestCapRec(trackObjects->objects[i].uid);
				}
			}
		}

		return lr;
	}

	LPRRESULT GetCapRec(CaptureRec_Multi *pCapRec)
	{
		return m_capture.GetCaptureRec(pCapRec);
	}
public:
	VSDEvent		m_event;
	VSDEventCapture m_capture;
};

#endif

#ifdef __cplusplus
}
#endif

#endif
