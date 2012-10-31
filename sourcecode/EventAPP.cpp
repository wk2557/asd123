//*********************************************************
//This file defines the basic interface of EventAPP
//Author:WK
//Data:2012.09.22`
//**********************************************************
#define __DEBUG
#include "EventAPP.h"
#include "MediaConverter.h"
#include "SubtitleOverlay.h"
#include <deque>
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <LPRVideo.h>
#include <LPR.h>

#include "ImageSynthesis.h"

#if defined(WIN32)
#define Int64 __int64
#elif  defined(LINUX)
#define Int64 long long
#else
#error "No platform specified!"
#endif

#ifdef __DEBUG
using namespace std;
#include <assert.h>
#include <iostream>
#include <string>
#include <ctime>
#include <algorithm>
#include <cstring>
#define TRACE(string) std::cout << string << std::endl
ofstream lNoBreakRuleLog("NoBreakRule.txt");
wofstream lBreakRuleHistoryLog("BreakRuleHistory.txt");
ofstream lBreakOutputLog("BreakRuleOutput.txt");
ofstream lTimeConsumeLog("f:\\TimeConsume.txt");
std::wstring lCurrentPicName;
#endif


typedef std::map<int, int> StatusMap;
typedef std::map<int, VSDRatioRECT> RatioRectMap;

struct PoolData
{
	StatusMap mBreakRules;
	//RatioRectMap mRatioRectMap;
	LPRImage* mpImage;
};

typedef std::deque<PoolData> ImagePool;
typedef std::map<int, LPRImage*> CaptureImageMap;
typedef std::map<int, wchar_t*> PlateMap; 
typedef std::list<EventAPPResult> ResultList;
typedef std::map<int, VSDRect> RectMap;
typedef std::map<int, int> PriorityMap;
typedef std::map<int, int> RecordMap;
typedef std::map<int, int> TouchMap;
typedef std::map<int, int> LoopMap;
typedef std::vector<LPRImage*> VideoImageVector;
struct RuleVideoImage
{
	int mRuleType;
	VideoImageVector mVideoImage;
	LPRImage* mBreakRuleImage;
};
//typedef std::map<int, VideoImageVector> RuleVideoImageMap;
typedef std::map<int, RuleVideoImage> ObjectVideoImageMap;

#ifdef __cplusplus
extern "C"{
#endif
	class EventAPPImpl
	{

	public:
		EventAPPImpl();
		APPRESULT Init(const EventAPPParam& irParam);
		void ConstructVideo(/*int iObjectBreakRule, */int iRuleType, int uid, int startIndex/*, EventMultiAPPResult* opResultMulti, int& orResultCount*/);
		void ConstructResult(int uid, EventMultiAPPResult* opResultMulti);//, int& orResultCount);
		APPRESULT ProcessFrame(const LPRImage* ipImage, const VSDObjectMulti* ipObjectMulti,const VSDObjectTrackMulti* ipObjectTrackMult, int iLightStatus[MAX_VIRTUAL_LOOPS], EventMultiAPPResult* opResult); 
		APPRESULT AddSubTitle(const LPRImage* ipImage, const EventSubtitleOverlay &subtitles, const EventSubtitleImages* ipSubtitleImages ,LPRImage** oppImage);
		APPRESULT Convert2Media(LPRImage** ipImage, int iNumOfImages, EventMedia& orMedia);
		APPRESULT SynthesisImages(LPRImage** ipImage, int iNumOfImages, const VSDRect& irRect, LPRImage** oppImage);
		APPRESULT GetPlate(const LPRImage* ipImage, const VSDRect& irRect, wchar_t oPlate[LPR_PLATE_STR_LEN]);
		void GrabStopLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage);
		void GrabVirtualLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage);
		void GrabCenterLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage);
		void GrabLeftTurnLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage);
		void GrabRightTurnLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage);
		void TranversePool(EventMultiAPPResult* opResult);
		bool IsBreakRedLight(int iLightStatus, const VSDRatioRECT& irObjectRect, float iCrossRatio);
		bool IsBreakCrossLine(int iLoopID,  const VSDRatioRECT& irObjectRect, float iCrossRatio);
		bool IsBreakTurnLeft(const VSDRatioRECT& irObjectRect, int iLoopID, float iCrossRatio);
		bool IsBreakTurnRight(const VSDRatioRECT& irObjectRect, int iLoopID, float iCrossRatio);
		bool IsBreakStraight(const VSDRatioRECT& irObjectRect, int iLoopID);
		bool IsBreakReverse(int iObjectID, const VSDObjectTrackMulti& irObjectTractMulti, const VSDObject& irObject);
		bool IsBreakStop(int iObjectID, const VSDObjectTrackMulti& irObjectTractMulti, const VSDRatioRECT& irObjectRect, const VSDObject& irObject);
		~EventAPPImpl();
		EventAPPParam mEventAPPParam;
		ImagePool mImagePool;
		VSDRatioLine mLaneMark[MAX_VIRTUAL_LOOPS * 2];
		int mMaxPoolLength;
		CaptureImageMap mTouchStopLineImage;
		CaptureImageMap mTouchLeftTurnImage;
		CaptureImageMap mTouchRightTurnImage;
		CaptureImageMap mTouchCentreLineImage;
		CaptureImageMap mTouchVirtualLoopLineImage;
		CaptureImageMap mLeaveStopLineImage;
		StatusMap mStatusMap;
		MediaConverter mMediaConverter;
		ObjectVideoImageMap mVideoImage;
		LPR mLPR;
		int mImageWidth;
		int mImageHeight;
		ImageSynthesis mImageSynthesis;
		RectMap mRectMap;
		PriorityMap mPriorityMap;
		RecordMap mRecordMap;	
		int mStartFrameIndex;
		int mCheckPoolIndex;
		TouchMap mTouchMap;
		LoopMap mLoopMap;
		bool mIsLPRInited;
		LPRImage* mpDecodeBuffer;
	};
#ifdef __cplusplus
}
#endif


// 返回两个输入值中的最小值
template<typename T>
static T __stdcall MinT(T a, T b)
{
	return a < b ? a : b; 
}

// 返回两个输入值中的最大值
template<typename T>
static T __stdcall MaxT(T a, T b)
{
	return a > b ? a : b;
}

// 图像上的绝对矩形转换为相对矩形
static inline void __stdcall ToRatioRECT(const VSDRect& irRect, float iXRatio, float iYRatio, VSDRatioRECT& orRECT)
{
	orRECT.left = irRect.x * iXRatio;
	orRECT.top = irRect.y * iYRatio;
	orRECT.right = orRECT.left + irRect.width * iXRatio;
	orRECT.bottom = orRECT.top + irRect.height * iYRatio;
}

// 判断点是否在矩形内
static bool __stdcall IsInRect(const VSDRatioPoint& irPoint,const VSDRatioRECT& irRect)
{
	return (irPoint.x > irRect.left) && (irPoint.x < irRect.right) && (irPoint.y < irRect.bottom) && (irPoint.y > irRect.top);
}

// 判断点是否在直线上
static bool __stdcall IsOnLine(const VSDRatioPoint& irPoint, const VSDRatioLine& irLine)
{
	int maxX = MaxT(irLine.pt1.x, irLine.pt2.x);
	int minX = MinT(irLine.pt1.x, irLine.pt2.x);
	int maxY = MaxT(irLine.pt1.y, irLine.pt2.y);
	int minY = MinT(irLine.pt1.y, irLine.pt2.y);
	if (irPoint.x < minX || irPoint.x > maxX || irPoint.y < minY || irPoint.y > maxY)
	{
		return false;
	}
	return (irLine.pt1.y - irPoint.y) * (irPoint.x - irLine.pt2.x) == (irPoint.y - irLine.pt2.y) * (irLine.pt1.x - irPoint.x);

}

// 判断两条线段是否相交，若相交则返回true并返回orPointC，否则返回false
static bool __stdcall GetCrossRatioPoint(const VSDRatioLine& irLineA, const VSDRatioLine& irLineB, VSDRatioPoint& orPointC)
{
	if(IsOnLine(irLineA.pt1, irLineB))
	{
		orPointC = irLineA.pt1;
		return true;
	}
	if(IsOnLine(irLineA.pt2, irLineB))
	{
		orPointC = irLineA.pt2;
		return true;
	}

	const VSDRatioPoint a = irLineA.pt1;
	const VSDRatioPoint b = irLineA.pt2;
	const VSDRatioPoint c = irLineB.pt1;
	const VSDRatioPoint d = irLineB.pt2;

	Int64 area_abc = Int64((a.x - c.x)) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
	Int64 area_abd = Int64((a.x - d.x)) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x);

	if(Int64(area_abc) * area_abd >= 0)
		return false;

	Int64 area_cda = Int64((c.x - a.x)) * (d.y - a.y) - (c.y - a.y) * (d.x - a.x);
	Int64 area_cdb = Int64(area_cda) + area_abc - area_abd;
	if(Int64(area_cda) * area_cdb >= 0)
		return false;
	
	float t = (float)(area_cda) / (area_abd - area_abc);
	orPointC.x = int(a.x + t * (b.x - a.x));
	orPointC.y = a.y + t * (b.y - a.y);
	return true;
}


static inline bool __stdcall IsSame(const VSDRatioPoint& irPointA, const VSDRatioPoint& irPointB)
{
	return irPointA.x == irPointB.x && irPointA.y == irPointB.y;
}

static float __stdcall GetCrossRatio(const VSDRatioLine& irLine, const VSDRatioRECT& irRECT)
{
	VSDRatioLine lRectLineArray[4] = {{{irRECT.left, irRECT.top}, {irRECT.right, irRECT.top}},{{irRECT.right, irRECT.top}, {irRECT.right, irRECT.bottom}},
									 {{irRECT.right, irRECT.bottom}, {irRECT.left, irRECT.bottom}},{{irRECT.left, irRECT.bottom}, {irRECT.left, irRECT.top}}};


	VSDRatioPoint crossTmp;
	VSDRatioPoint crossPoint[2];
	int MAXCROSSPOINT = 2;
	int index = 0;
	bool isCross = false;

	for(int i = 0; i < sizeof(lRectLineArray)/sizeof(VSDRatioLine); ++i)
	{
		isCross = GetCrossRatioPoint(irLine, lRectLineArray[i], crossTmp);
		if(isCross)
		{
			if(index == 0)
			{
				crossPoint[0] = crossTmp;
				index = 1;
			}
			else if(index == 1)
			{
				if(!IsSame(crossPoint[0], crossTmp))
				{
					crossPoint[1] = crossTmp;
					index = 2;
					break;
				}
			}
		}
	}
	// 如果只有一个交点
	if(1 == index)
	{
		if(IsInRect(irLine.pt1, irRECT))
		{
			crossPoint[1] = irLine.pt1;
			index = 2;
		}
		else if(IsInRect(irLine.pt2, irRECT))
		{
			crossPoint[1] = irLine.pt2;
			index = 2;
		}
	}

	if(2 != index)
		return 0;
	float middleX =  (float(crossPoint[0].x) + crossPoint[1].x) / 2;
	float middleY =  (float(crossPoint[0].y) + crossPoint[1].y) / 2;
	
	float areaA = (middleX - irRECT.left) * ( middleY - irRECT.top);
	float areaB = (irRECT.right - middleX) * (middleY - irRECT.top );
	float areaC = (irRECT.right - middleX) * (irRECT.bottom- middleY);
	float areaD = (middleX - irRECT.left) * (irRECT.bottom - middleY);

	float minArea = MinT(MinT(areaA,areaB), MinT(areaC, areaD));
	
	float areaRect = (irRECT.bottom - irRECT.top) * (irRECT.right - irRECT.left);
	return minArea * 4 / areaRect;
}

static int __stdcall getIndex(int iRule)
{
	int ret = 0;
	while(iRule)
	{
		iRule = iRule >> 1;
		++ret;
	}
	return ret;
}


void __stdcall InitEventMultiAPPResult(EventMultiAPPResult& irEventMultiAPPResult)
{
	::memset(&irEventMultiAPPResult, 0, sizeof(irEventMultiAPPResult));
}

APPRESULT __stdcall EventAPP_Init(void* ipObject, const EventAPPParam& irEventAPPParam)
{
	EventAPPImpl* lpEventAPPImpl = (EventAPPImpl*)ipObject;
	return lpEventAPPImpl->Init(irEventAPPParam);
}

void* __stdcall EventAPP_Create()
{
	return new EventAPPImpl;
}

void __stdcall EventAPP_Destory(void* ipObject)
{
	EventAPPImpl* lpEventAPPImpl = (EventAPPImpl*)ipObject;
	delete lpEventAPPImpl;
}

APPRESULT __stdcall EventAPP_ProcessFrame(void* ipObject, const LPRImage* ipImage, const VSDObjectMulti* ipObjectMulti,const VSDObjectTrackMulti* ipObjectTrackMult, int iLightStatus[MAX_VIRTUAL_LOOPS], EventMultiAPPResult* opResult)
{
	EventAPPImpl* lpEventAPPImpl = (EventAPPImpl*)ipObject;
	return lpEventAPPImpl->ProcessFrame(ipImage, ipObjectMulti, ipObjectTrackMult, iLightStatus, opResult);
}

APPRESULT __stdcall EventAPP_AddSubTitle(void* ipObject, const LPRImage* ipImage, const EventSubtitleOverlay &subtitles, const EventSubtitleImages* ipSubtitleImages ,LPRImage** oppImage)
{
	EventAPPImpl* lpEventAPPImpl = (EventAPPImpl*)ipObject;
	return lpEventAPPImpl->AddSubTitle(ipImage, subtitles, ipSubtitleImages, oppImage);
}

APPRESULT __stdcall EventAPP_SynthesisImages(void* ipObject, LPRImage** ipImage, int iNumOfImages, const VSDRect& irRect, LPRImage** oppImage)
{
	EventAPPImpl* lpEventAPPImpl = (EventAPPImpl*)ipObject;
	return lpEventAPPImpl->SynthesisImages(ipImage, iNumOfImages, irRect, oppImage);
}

APPRESULT __stdcall EventAPP_Convert2Media(void* ipObject, LPRImage** ipImage, int iNumOfImages, EventMedia& orMedia)
{
	EventAPPImpl* lpEventAPPImpl = (EventAPPImpl*)ipObject;
	return lpEventAPPImpl->Convert2Media(ipImage, iNumOfImages, orMedia);
}

APPRESULT __stdcall EventAPP_GetPlate(void* ipObject, const LPRImage* ipImage, const VSDRect& irRect, wchar_t oPlate[LPR_PLATE_STR_LEN])
{
	EventAPPImpl* lpEventAPPImpl = (EventAPPImpl*)ipObject;
	return lpEventAPPImpl->GetPlate(ipImage, irRect, oPlate);
}


EventAPPImpl::EventAPPImpl() : mMaxPoolLength(0), mImageHeight(0), mImageWidth(0), mStartFrameIndex(0),mCheckPoolIndex(0), mIsLPRInited(false), mpDecodeBuffer(NULL)
{
}

APPRESULT EventAPPImpl::Init(const EventAPPParam& irParam)
{
	mEventAPPParam = irParam;
	APPRESULT lResult = mMediaConverter.Init((EventAPPVideoFormat)irParam.mRecordParam.mVideoFormat, irParam.mRecordParam.mFrameFrequent, irParam.mRecordParam.mBitFrequent);
	if (lResult != APP_OK)
	{
#ifdef __DEBUG
		TRACE("初始化MediaConverter失败");
#endif
		return lResult;
	}
	int maxAhead = irParam.mRecordParam.mBreakRuleAhead[0];					
	int maxBehind = irParam.mRecordParam.mBreakRuleBehind[0];
	for(int i = 1; i < RULE_TYPES; ++i)
	{
		maxAhead = MaxT(maxAhead, irParam.mRecordParam.mBreakRuleAhead[i]);
		maxBehind = MaxT(maxBehind, irParam.mRecordParam.mBreakRuleBehind[i]);
	}
	mMaxPoolLength = maxAhead + maxBehind + DEFAULT_EXTRA_IMAGE_BUFFER;
	mStartFrameIndex = maxAhead;
	return APP_OK;
}

void EventAPPImpl::GrabStopLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage)
{
	// 抓取物体接触停车线的图片
	TouchMap::iterator itTouchStopLine = mTouchMap.find(lObject.uid);
	if( itTouchStopLine == mTouchMap.end() && GetCrossRatio(mEventAPPParam.mStopLine, objectRatioRECT) > 0)
	{
		mTouchMap.insert(std::make_pair(lObject.uid, 1));
		CaptureImageMap::iterator itVirtualImage = mTouchStopLineImage.find(lObject.uid);
		if(itVirtualImage == mTouchStopLineImage.end())
		{
			LPRImage *pImage = LPRCloneImage(ipImage);
			mTouchStopLineImage.insert(std::make_pair(lObject.uid, pImage));
			RectMap::iterator itRect = mRectMap.find(lObject.uid); 
			if(itRect == mRectMap.end())
				mRectMap.insert(std::make_pair(lObject.uid, lObject.rect));
		}
	}

	// 抓取物体离开停车线的图片 
	if (itTouchStopLine != mTouchMap.end() && itTouchStopLine->second == 1)
	{
		if (GetCrossRatio(mEventAPPParam.mStopLine, objectRatioRECT) == 0)
		{
			itTouchStopLine->second = 2;
			CaptureImageMap::iterator itVirtualImage = mLeaveStopLineImage.find(lObject.uid);
			if(itVirtualImage == mLeaveStopLineImage.end())
			{
				LPRImage *pImage = LPRCloneImage(ipImage);
				mLeaveStopLineImage.insert(std::make_pair(lObject.uid, pImage));
			}

		}
	}
}

void EventAPPImpl::GrabCenterLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage)
{
	// 抓取物体接触中间线的图片, 
	int lMiddlCentreLine = (mEventAPPParam.mCentreLine.pt1.y + mEventAPPParam.mCentreLine.pt2.y) / 2;
	
	if(lMiddlCentreLine <= objectRatioRECT.bottom && lMiddlCentreLine >= objectRatioRECT.top)
	{
		CaptureImageMap::iterator itVirtualImage = mTouchCentreLineImage.find(lObject.uid);
		if(itVirtualImage == mTouchCentreLineImage.end())
		{
			LPRImage* pImage = LPRCloneImage(ipImage);
			mTouchCentreLineImage.insert(std::make_pair(lObject.uid, pImage));
		}
	}
}

void EventAPPImpl::GrabVirtualLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage)
{
	if(lObject.status & VSD_OBJ_STATUS_TOUCH_LINE)
	{
		CaptureImageMap::iterator itVirtualImage = mTouchVirtualLoopLineImage.find(lObject.uid);
		if(itVirtualImage == mTouchVirtualLoopLineImage.end())
		{
			LPRImage* pImage = LPRCloneImage(ipImage);
			mTouchVirtualLoopLineImage.insert(std::make_pair(lObject.uid, pImage));
		}
	}
}

void EventAPPImpl::GrabLeftTurnLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage)
{
	CaptureImageMap::iterator itLeftTurnImage = mTouchLeftTurnImage.find(lObject.uid);
	if (itLeftTurnImage == mTouchLeftTurnImage.end())
	{
		if (GetCrossRatio(mEventAPPParam.mLeftTurnLine, objectRatioRECT) > 0)
		{
			LPRImage* lpImage = LPRCloneImage(ipImage);
			mTouchLeftTurnImage.insert(make_pair(lObject.uid, lpImage));
		}
	}
}

void EventAPPImpl::GrabRightTurnLineImage(const VSDRatioRECT& objectRatioRECT, const VSDObject& lObject, const LPRImage* ipImage)
{
	CaptureImageMap::iterator itRightTurnImage = mTouchRightTurnImage.find(lObject.uid);
	if (itRightTurnImage == mTouchRightTurnImage.end())
	{
		if (GetCrossRatio(mEventAPPParam.mRightTurnLine, objectRatioRECT) > 0)
		{
			LPRImage* lpImage = LPRCloneImage(ipImage);
			mTouchRightTurnImage.insert(make_pair(lObject.uid, lpImage));
		}
	}
}

void EventAPPImpl::ConstructResult(int uid, EventMultiAPPResult* opResultMulti)//, int& orResultCount)
{
	EventAPPResult lAPPResult;
	ObjectVideoImageMap::iterator itObjectVideoImage = mVideoImage.find(uid);
	lAPPResult.mID = uid;
	// 填充EventAPPResult的mVideoImage信息
	if (itObjectVideoImage == mVideoImage.end())
	{
		lAPPResult.mBreakRule = VSD_BR_NONE;
		lAPPResult.mNumOfVideoImage = 0;
	}
	else
	{
		int lVideoImageSize = itObjectVideoImage->second.mVideoImage.size();
		lAPPResult.mBreakRule = itObjectVideoImage->second.mRuleType;
		for(int i = 0; i < lVideoImageSize; ++i)
		{
			LPRImage* lpImage = LPRCloneImage(itObjectVideoImage->second.mVideoImage[i]);
			lAPPResult.mVideoImage[i] = lpImage;
		}
		lAPPResult.mNumOfVideoImage = lVideoImageSize;
	}

	// 填充车道信息
	LoopMap::iterator itLoop = mLoopMap.find(uid);
	if (itLoop != mLoopMap.end())
	{
		lAPPResult.mLoopID = itLoop->second;
	}
	else
		lAPPResult.mLoopID = 0;

	// 填充EventAPPResult的车的位置矩形信息
	RectMap::iterator itRect = mRectMap.find(uid);
	if(itRect != mRectMap.end())
		lAPPResult.mRect = itRect->second;
	else
	{
		lAPPResult.mRect.height = 0;
		lAPPResult.mRect.width  = 0;
		lAPPResult.mRect.x = 0;
		lAPPResult.mRect.y = 0;
	}

	// 填充EventAPPResult的mSynthesisImage信息
	int lSynthesisNum = 0;
	CaptureImageMap::iterator itVirtualLoopTouch = mTouchStopLineImage.find(uid);
	if(itVirtualLoopTouch != mTouchStopLineImage.end())
	{
		LPRImage* lpImage = LPRCloneImage(itVirtualLoopTouch->second);
		lAPPResult.mSynthesisImage[lSynthesisNum++] = lpImage;
	}
	// 如果有违章
	if (lAPPResult.mBreakRule != VSD_BR_NONE)
	{
		CaptureImageMap::iterator itVirtualLoopLeave = mLeaveStopLineImage.find(uid);
		if(itVirtualLoopLeave != mLeaveStopLineImage.end())
		{
			LPRImage* lpImage = LPRCloneImage(itVirtualLoopLeave->second);
			lAPPResult.mSynthesisImage[lSynthesisNum++] = lpImage;
		}
		if(lAPPResult.mBreakRule == VSD_BR_RED_LIGHT)
		{
			CaptureImageMap::iterator itCentreImage = mTouchCentreLineImage.find(uid);
			if(itCentreImage != mTouchCentreLineImage.end())
			{
				LPRImage* lpImage = LPRCloneImage(itCentreImage->second);
				lAPPResult.mSynthesisImage[lSynthesisNum++] = lpImage;
			}
			else
			{
				int lLoopLaneProperty = mEventAPPParam.mVSDParam.loopLaneProperty[lAPPResult.mLoopID];
				if (lLoopLaneProperty == VSD_LANE_TURN_LEFT)
				{
					CaptureImageMap::iterator itLeftTurnImage = mTouchLeftTurnImage.find(uid);
					if (itLeftTurnImage != mTouchLeftTurnImage.end())
					{
						LPRImage* lpImage = LPRCloneImage(itLeftTurnImage->second);
						lAPPResult.mSynthesisImage[lSynthesisNum++] = lpImage;
					}
				}
				else if (lLoopLaneProperty == VSD_LANE_TURN_RIGHT)
				{
					CaptureImageMap::iterator itRightTurnImage = mTouchRightTurnImage.find(uid);
					if (itRightTurnImage != mTouchRightTurnImage.end())
					{
						LPRImage* lpImage = LPRCloneImage(itRightTurnImage->second);
						lAPPResult.mSynthesisImage[lSynthesisNum++] = lpImage;
					}
				}
			}		
		}
		else 
		{
			LPRImage* lpImage = LPRCloneImage(itObjectVideoImage->second.mBreakRuleImage);//mImagePool.at(iStartIndex).mpImage);
			lAPPResult.mSynthesisImage[lSynthesisNum++] = lpImage;
		}
	}
	lAPPResult.mNumOfSynthesisImage = lSynthesisNum;
	// 填充EventAPPResult的mPlateImage信息
	CaptureImageMap::iterator itPlateImage = mTouchVirtualLoopLineImage.find(uid);
	if(itPlateImage != mTouchVirtualLoopLineImage.end())
	{
		LPRImage* lpImage = LPRCloneImage(itPlateImage->second);
		lAPPResult.mPlateImage = lpImage;
	}
	else
		lAPPResult.mPlateImage = NULL;
	// 构造好EventAPPResult后把他插入到opResultMulti中
	opResultMulti->mppAPPResult[opResultMulti->mNumOfResult++] = lAPPResult;
	//opResultMulti->mNumOfResult = orResultCount;
	return;
}

void EventAPPImpl::TranversePool(EventMultiAPPResult* opResult)
{
	//int lResultCount = 0;
	if (mImagePool.size() > mMaxPoolLength + 1)
	{
		PoolData lCheckPoolData;

		for(int startIndex = mCheckPoolIndex; startIndex <= mStartFrameIndex; ++startIndex)
		{
			lCheckPoolData  = mImagePool.at(startIndex);
			// 遍历当前poolData中的所有object的违章记录
			for(StatusMap::iterator itObject = lCheckPoolData.mBreakRules.begin(); itObject != lCheckPoolData.mBreakRules.end(); ++itObject)
			{
				//if(itObject->second == VSD_BR_NONE)
				//	ConstructVideo(/*itObject->second, */VSD_BR_NONE, itObject->first, startIndex, opResult, lResultCount);
				if(itObject->second == VSD_BR_TURN_LEFT)
					ConstructVideo(/*itObject->second,*/ VSD_BR_TURN_LEFT, itObject->first, startIndex/*, opResult, lResultCount*/);
				else if(itObject->second == VSD_BR_TURN_RIGHT)
					ConstructVideo(/*itObject->second,*/ VSD_BR_TURN_RIGHT, itObject->first, startIndex/*, opResult, lResultCount*/);
				else if(itObject->second == VSD_BR_STRAIGHT_THROUGH)
					ConstructVideo(/*itObject->second,*/ VSD_BR_STRAIGHT_THROUGH, itObject->first, startIndex);//, opResult, lResultCount);
				else if(itObject->second == VSD_BR_CROSS_LANE)
					ConstructVideo(/*itObject->second,*/ VSD_BR_CROSS_LANE, itObject->first, startIndex);//, opResult, lResultCount);
				else if(itObject->second == VSD_BR_REVERSE)
					ConstructVideo(/*itObject->second,*/ VSD_BR_REVERSE, itObject->first, startIndex);//, opResult, lResultCount);
				else if(itObject->second == VSD_BR_RED_LIGHT)
					ConstructVideo(/*itObject->second,*/ VSD_BR_RED_LIGHT, itObject->first, startIndex);//, opResult, lResultCount);
				else if(itObject->second == VSD_BR_STOP)
					ConstructVideo(/*itObject->second, */VSD_BR_STOP, itObject->first, startIndex);//, opResult, lResultCount);
			}
			++mCheckPoolIndex;
		}
		if (mCheckPoolIndex > 0)
		{
			--mCheckPoolIndex;
		}
		
		// 清除不再需要的数据，并且构造没有违规的记录	
		lCheckPoolData = mImagePool.front();
		for(StatusMap::iterator itObject = lCheckPoolData.mBreakRules.begin(); itObject != lCheckPoolData.mBreakRules.end(); ++itObject)
		{
			int lShowUp = 0;
			for(int i =  1; i < mImagePool.size(); ++i)
			{
				PoolData lTmpPoolData = mImagePool.at(i);
				if(lTmpPoolData.mBreakRules.find(itObject->first) != lTmpPoolData.mBreakRules.end())
				{
					lShowUp = 1;
					break;
				}
			}
			if(0 == lShowUp)
			{		
				//if(itObject->second == VSD_BR_NONE)
				//	ConstructVideo(/*itObject->second, */VSD_BR_NONE, itObject->first, 0);//, opResult, lResultCount);
				ConstructResult(itObject->first, opResult);
				StatusMap::iterator itStatusMap = mStatusMap.find(itObject->first);
				if(itStatusMap != mStatusMap.end())
					mStatusMap.erase(itStatusMap);

				PriorityMap::iterator itPirorityMap = mPriorityMap.find(itObject->first);
				if (itPirorityMap != mPriorityMap.end())
					mPriorityMap.erase(itPirorityMap);

				RecordMap::iterator itRecord = mRecordMap.find(itObject->first);
				if (itRecord != mRecordMap.end())
					mRecordMap.erase(itRecord);

				RectMap::iterator itRect = mRectMap.find(itObject->first);
				if (itRect != mRectMap.end())
					mRectMap.erase(itRect);

				TouchMap::iterator itTouchStopLine = mTouchMap.find(itObject->first);
				if (itTouchStopLine != mTouchMap.end())
					mTouchMap.erase(itTouchStopLine);

				LoopMap::iterator itLoop = mLoopMap.find(itObject->first);
				if(itLoop != mLoopMap.end())
					mLoopMap.erase(itLoop);


				CaptureImageMap::iterator itVirtualLoopImage = mTouchStopLineImage.find(itObject->first);
				if (itVirtualLoopImage != mTouchStopLineImage.end())
				{
					LPRReleaseImage(itVirtualLoopImage->second);
					mTouchStopLineImage.erase(itVirtualLoopImage);
				}

				itVirtualLoopImage = mLeaveStopLineImage.find(itObject->first);
				if(itVirtualLoopImage != mLeaveStopLineImage.end())
				{
					LPRReleaseImage(itVirtualLoopImage->second);
					mLeaveStopLineImage.erase(itVirtualLoopImage);
				}

				itVirtualLoopImage = mTouchCentreLineImage.find(itObject->first);
				if(itVirtualLoopImage != mTouchCentreLineImage.end())
				{
					LPRReleaseImage(itVirtualLoopImage->second);
					mTouchCentreLineImage.erase(itVirtualLoopImage);
				}
				itVirtualLoopImage = mTouchVirtualLoopLineImage.find(itObject->first);
				if (itVirtualLoopImage != mTouchVirtualLoopLineImage.end())
				{
					LPRReleaseImage(itVirtualLoopImage->second);
					mTouchVirtualLoopLineImage.erase(itVirtualLoopImage);
				}
				itVirtualLoopImage = mTouchLeftTurnImage.find(itObject->first);
				if (itVirtualLoopImage != mTouchLeftTurnImage.end())
				{
					LPRReleaseImage(itVirtualLoopImage->second);
					mTouchLeftTurnImage.erase(itVirtualLoopImage);
				}
				itVirtualLoopImage = mTouchRightTurnImage.find(itObject->first);
				if (itVirtualLoopImage != mTouchRightTurnImage.end())
				{
					LPRReleaseImage(itVirtualLoopImage->second);
					mTouchRightTurnImage.erase(itVirtualLoopImage);
				}
				ObjectVideoImageMap::iterator itObjectVideoMap = mVideoImage.find(itObject->first);
				if (itObjectVideoMap != mVideoImage.end())
				{
					if (itObjectVideoMap->second.mRuleType != VSD_BR_NONE)
					{
						int lVideoSize = itObjectVideoMap->second.mVideoImage.size();
						for (int i = 0; i < lVideoSize; ++i)
						{
							LPRReleaseImage(itObjectVideoMap->second.mVideoImage[i]);
						}
						LPRReleaseImage(itObjectVideoMap->second.mBreakRuleImage);
					}
					mVideoImage.erase(itObjectVideoMap);
				}
			}
		}
		// 释放掉队列头已不再需要录像的缓存的图片
		LPRReleaseImage(mImagePool.front().mpImage);
		mImagePool.pop_front();
	}
}

APPRESULT EventAPPImpl::GetPlate(const LPRImage* ipImage, const VSDRect& irRect, wchar_t oPlate[LPR_PLATE_STR_LEN])
{
	if (!mIsLPRInited)
	{
#ifdef __DEBUG
		TRACE("车牌识别模块尚未初始化");
#endif
		return APP_LPR_NOT_INITED;
	}
	if(mEventAPPParam.mPlateCaptureSwitch == EVENT_APP_PLATE_SWITCH_ON)
	{
		// 初始化局部参数。
		LPRParamLocal	localParam;
		// 识别区域设为当前物体的矩形框，因为物体矩形框有时比较小，没有包括车牌所在的范围，因为我们扩大搜索范围
		int lEnlargeWidth = irRect.width / 2;
		int lEnlargeHeight = irRect.height / 2;
		localParam.m_rectRegion.left = MaxT(irRect.x - lEnlargeWidth, 0);
		localParam.m_rectRegion.right = MinT(irRect.x + irRect.width + lEnlargeWidth, mImageWidth);
		localParam.m_rectRegion.top = MaxT(irRect.y - lEnlargeHeight, 0);
		localParam.m_rectRegion.bottom = MinT(irRect.y + irRect.height + lEnlargeHeight, mImageHeight);
		localParam.m_nMinPlateWidth = DEFAULT_PLATE_MIN_WIDTH; 
		localParam.m_nMaxPlateWidth = DEFAULT_PLATE_MAX_WIDTH;
		localParam.m_fltReserved0 = 0;
		localParam.m_fltReserved1 = 0;
		localParam.m_fltReserved2 = 0;
		localParam.m_fltReserved3 = 0;

		// 初始化LPRParamMulti结构
		LPRParamMulti	multiParam;
		LPRParamMulti_Init( &multiParam );
		LPRParamMulti_Add( &multiParam, localParam );

		// 初始化LPROutputMulti结构
		LPROutputMulti	multiOutput;
		LPROutputMulti_Init( &multiOutput );

		// 检测识别车牌
		//LPRImage* pDecodeImage = NULL;
		HRESULT lResult = LPRDecodeImage(&mpDecodeBuffer, (const unsigned char*)ipImage->pData, ipImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		if(lResult != LPR_OK)
		{
#ifdef __DEBUG
			TRACE("EventAPP::ProcessFrame Fail to decode image");
#endif
			return APP_FAIL;
		}
		lResult =mLPR.ProcessImage(mpDecodeBuffer, &multiOutput, multiParam, NULL );
		if (lResult != LPR_OK)
		{
#ifdef __DEBUG
			TRACE("EventAPP::ProcessFrame fail to recognize plate");
#endif
			return APP_FAIL;
		}
		if(multiOutput.m_nNumOutputs > 0)
		{
			for (int i = 0; i < LPR_PLATE_STR_LEN; ++i)
			{
				oPlate[i] = multiOutput.m_outputs[0].wszRec[i];
			}
		}
		else
			oPlate[0] = '\0';
		return APP_OK;
	}
	return APP_OK;
}

bool EventAPPImpl::IsBreakRedLight(int iLightStatus, const VSDRatioRECT& irObjectRect, float iCrossRatio)
{
	if(mEventAPPParam.mRuleSwitch[getIndex(VSD_BR_RED_LIGHT)] == EVENT_APP_RULE_SWITCH_ON && iLightStatus == EVENT_APP_LIGHT_RED )	
	{
		if (GetCrossRatio(mEventAPPParam.mStopLine, irObjectRect) >= iCrossRatio)
		{	
			return true;
		}
	}
	return false;
}

bool EventAPPImpl::IsBreakCrossLine(int iLoopID, const VSDRatioRECT& irObjectRect, float iCrossRatio)
{
	if(mEventAPPParam.mRuleSwitch[getIndex(VSD_BR_CROSS_LANE)] == EVENT_APP_RULE_SWITCH_ON)
	{
		if (GetCrossRatio(mLaneMark[2 * iLoopID], irObjectRect) >= iCrossRatio || GetCrossRatio(mLaneMark[2 * iLoopID + 1], irObjectRect) >= iCrossRatio)
		{
			return true;
		}
	}
	return false;
}

bool EventAPPImpl::IsBreakTurnLeft(const VSDRatioRECT& irObjectRect, int iLoopID, float iCrossRatio)
{
	if(mEventAPPParam.mRuleSwitch[getIndex(VSD_BR_TURN_LEFT)] == EVENT_APP_RULE_SWITCH_ON && !(mEventAPPParam.mVSDParam.loopLaneProperty[iLoopID] & VSD_LANE_TURN_LEFT) )
	{
		if (GetCrossRatio(mEventAPPParam.mLeftTurnLine, irObjectRect) >= iCrossRatio)
		{
			return true;
		}
	}
	return false;
}

bool EventAPPImpl::IsBreakTurnRight(const VSDRatioRECT& irObjectRect, int iLoopID, float iCrossRatio)
{
	if(mEventAPPParam.mRuleSwitch[getIndex(VSD_BR_TURN_RIGHT)] == EVENT_APP_RULE_SWITCH_ON && !(mEventAPPParam.mVSDParam.loopLaneProperty[iLoopID] & VSD_BR_TURN_RIGHT) )
	{
		if (GetCrossRatio(mEventAPPParam.mLeftTurnLine, irObjectRect) >= iCrossRatio)
		{
			return true;
		}
	}
	return false;
}

bool EventAPPImpl::IsBreakStraight(const VSDRatioRECT& irObjectRect, int iLoopID)
{
	if(mEventAPPParam.mRuleSwitch[getIndex(VSD_BR_STRAIGHT_THROUGH)] == EVENT_APP_RULE_SWITCH_ON && !(mEventAPPParam.mVSDParam.loopLaneProperty[iLoopID] & VSD_LANE_STRAIGHT)) 
	{
		if (GetCrossRatio(mEventAPPParam.mStraightLine, irObjectRect) > 0)
		{
			return true;
		}
	}
	return false;
}

bool EventAPPImpl::IsBreakReverse(int iObjectID, const VSDObjectTrackMulti& irObjectTractMulti, const VSDObject& irObject)
{
	if(mEventAPPParam.mRuleSwitch[getIndex(VSD_BR_REVERSE)] == EVENT_APP_RULE_SWITCH_ON)
	{
		VSDObjectTrack lObjectTrack;
		lObjectTrack.uid = -1;
		for (int i= 0; i < irObjectTractMulti.nObjects; ++i)
		{
			if (irObjectTractMulti.objTracks[i].uid == iObjectID)
			{
				lObjectTrack = irObjectTractMulti.objTracks[i];
				break;
			}
		}
		if (lObjectTrack.uid != -1)
		{
			int lTrackNum = lObjectTrack.nTracks;
			if (lTrackNum > 1)
			{
				if(mEventAPPParam.mVSDParam.nEventType == VSDEvent_VehicleHead && lObjectTrack.tracks[0].y - irObject.rect.height * (float)mEventAPPParam.mReverseRatio / 100 > lObjectTrack.tracks[lTrackNum - 1].y)
				{
					return true;
				}
				else if(mEventAPPParam.mVSDParam.nEventType == VSDEvent_VehicleTail && lObjectTrack.tracks[0].y + irObject.rect.height *(float) mEventAPPParam.mReverseRatio / 100 < lObjectTrack.tracks[lTrackNum - 1].y)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool EventAPPImpl::IsBreakStop(int iObjectID, const VSDObjectTrackMulti& irObjectTractMulti, const VSDRatioRECT& irObjectRect , const VSDObject& irObject)
{
	if(mEventAPPParam.mRuleSwitch[getIndex(VSD_BR_STOP)] == EVENT_APP_RULE_SWITCH_ON)
	{
		int lObjectMiddleX = (irObjectRect.left + irObjectRect.right ) / 2;//irRect.width / 2 + irRect.x;
		int lObjectMiddleY = (irObjectRect.bottom + irObjectRect.top) / 2;// irRect.height / 2 + irRect.y;
		VSDRatioPoint lTmpPoint = {lObjectMiddleX, lObjectMiddleY};
		if(IsInRect(lTmpPoint, mEventAPPParam.mStopForbidRect))
		{
			VSDObjectTrack lObjectTrack;
			lObjectTrack.uid = -1;
			for (int i= 0; i < irObjectTractMulti.nObjects; ++i)
			{
				if (irObjectTractMulti.objTracks[i].uid == iObjectID)
				{
					lObjectTrack = irObjectTractMulti.objTracks[i];
					break;
				}
			}
			if (lObjectTrack.uid != -1)
			{
				int lTrackNum = lObjectTrack.nTracks;
				if (lTrackNum > 1)
				{
					int lCurrentX = irObject.rect.width / 2 + irObject.rect.x;
					int lCurrentY = irObject.rect.height / 2 + irObject.rect.y;

					int lPreviousX = lObjectTrack.tracks[lTrackNum - 2].width / 2 + lObjectTrack.tracks[lTrackNum - 2].x;
					int lPreviousY = lObjectTrack.tracks[lTrackNum - 2].height / 2 + lObjectTrack.tracks[lTrackNum - 2].y;
					if ((lCurrentX - lPreviousX) * (lCurrentX - lPreviousX) + (lCurrentY - lPreviousY) * (lCurrentY - lPreviousY) < ((float)mEventAPPParam.mStopRatio / 100 * irObject.rect.width) * (mEventAPPParam.mStopRatio / 100 * irObject.rect.width))
					{
						return true;
					}
				}

			}
		}
	}
	return false;
}

APPRESULT EventAPPImpl::ProcessFrame(const LPRImage *ipImage, const VSDObjectMulti* ipObjectMulti, const VSDObjectTrackMulti* ipObjectTrackMulti, int isRedLightOn[MAX_VIRTUAL_LOOPS], EventMultiAPPResult* opResult)
{
	if(!ipImage || !ipObjectMulti || !ipObjectTrackMulti)
	{
#ifdef __DEBUG
		TRACE("EventAPP::ProcessFrame input fault");
#endif
		return APP_INPUT_NULL_POINTER;
	}

	if (ipImage->nColorMode != CS_JPEG)
	{
#ifdef __DEBUG
		TRACE("EventAPP::ProcessFrame input fault");
#endif
		return APP_IMAGE_FORMAT_FAULT;
	}

	if (ipObjectMulti->nObjects <= 0)
	{
#ifdef __DEBUG
		TRACE("EventAPP::ProcessFrame number of input ojbect is zero");
#endif
		return APP_OK;
	}

	
	VSDEventParam lVSDParam = mEventAPPParam.mVSDParam;
	LPRRESULT lResult = LPR_OK;	
	// 通过解码得到Image的长宽值，同时根据Image的长宽值计算lanemark，同时根据Image的长宽值初始化LPR模块
	if(mImageHeight == 0 || mImageWidth == 0)
	{
		//LPRImage* pDecodeImage = NULL;
		lResult = LPRDecodeImage(&mpDecodeBuffer, (const unsigned char*)ipImage->pData, ipImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		if(lResult != LPR_OK)
		{
#ifdef __DEBUG
			TRACE("EventAPP::ProcessFrame Fail to decode image");
#endif
			//LPRReleaseImage(pDecodeImage);
			return APP_FAIL;
		}
		mImageHeight = mpDecodeBuffer->height;
		mImageWidth = mpDecodeBuffer->width;

		VSDRatioPoint lTmpPoint;
		lResult = VSDEvent_GenerateLaneMark(lVSDParam.ptRoad, lVSDParam.virtualLoopLine, lVSDParam.nVirtualLoop, mImageWidth, mImageHeight, lVSDParam.nWidthBase, lVSDParam.nHeightBase, mLaneMark, &lTmpPoint);
		if(lResult != LPR_OK)
		{
#ifdef __DEBUG
			TRACE("EventAPP::ProcessFrame fail to generat lane mark");
#endif
			return APP_FAIL;
		}

		// 因为得到的mLaneMark太长，我们只取虚拟线圈内的Line
		for (int index = 0; index < lVSDParam.nVirtualLoop; ++index)
		{
			VSDRatioPoint lTmpPoint;
			GetCrossRatioPoint(mLaneMark[2 * index], mEventAPPParam.mStopLine, lTmpPoint);
			if (mLaneMark[2 * index].pt1.y < mLaneMark[2*index].pt2.y)
				mLaneMark[2 * index].pt1 = lTmpPoint;
			else
				mLaneMark[2 * index].pt2 = lTmpPoint;
			
			GetCrossRatioPoint(mLaneMark[2 * index + 1], mEventAPPParam.mStopLine, lTmpPoint);
			if (mLaneMark[2 * index + 1].pt1.y < mLaneMark[2*index].pt2.y)
				mLaneMark[2 * index + 1].pt1 = lTmpPoint;
			else
				mLaneMark[2 * index].pt2 = lTmpPoint;
		}

		LPRParam lLPRParam;
		lLPRParam.size = sizeof(lLPRParam);
		lLPRParam.nMaxPlate = DEFAULT_NUMBER_PLATE;
		lLPRParam.nMinPlateWidth = DEFAULT_PLATE_MIN_WIDTH;
		lLPRParam.nMaxPlateWidth = DEFAULT_PLATE_MAX_WIDTH;
		lLPRParam.plateModel.plateType = PT_CAR_NORMAL | PT_BIG_NORMAL;
		lLPRParam.plateModel.plateCharType[0] = LR_China;			// 第0位，全国汉字模型。
		lLPRParam.plateModel.plateCharType[1] = LR_Alpha;			// 第1位，识别字母
		lLPRParam.plateModel.plateCharType[2] = LR_DigitAlpha;	// 第2位，识别数字字母
		lLPRParam.plateModel.plateCharType[3] = LR_DigitAlpha;	// 第3位，识别数字字母
		lLPRParam.plateModel.plateCharType[4] = LR_Digit;			// 第4位，识别数字
		lLPRParam.plateModel.plateCharType[5] = LR_Digit;			// 第5位，识别数字
		lLPRParam.plateModel.plateCharType[6] = LR_Digit;			// 第6位，识别数字
		
		mLPR.Fini();
		lResult = mLPR.Init2(lLPRParam, mImageWidth, mImageHeight, true);
		if(lResult != LPR_OK)
		{
#ifdef __DEBUG
			TRACE("EventAPP::ProcessFram fail to init LPR");
#endif
			return APP_FAIL;
		}
		mIsLPRInited = true;
	}
	
	// 得到压线阈值
	float crossRatio = (float)mEventAPPParam.mRatioToCrossLine / 100;
	// 把物体的Rect转化为 RatioRect
	VSDRatioRECT objectRatioRECT;
	if(mImageHeight == 0 || mImageWidth == 0)
	{
#ifdef __DEBUG
		TRACE("图片的格式有误，无法得到图片的长宽值");
#endif
		return APP_IMAGE_FORMAT_FAULT;
	}
	float imageXRatio = lVSDParam.nWidthBase / (float)(mImageWidth);
	float imageYRatio = lVSDParam.nHeightBase / (float)(mImageHeight);
	
	int lObjectCount = ipObjectMulti->nObjects;
	VSDObject lObject;

	// 构造PoolData 
	PoolData lPoolData;
	// 因为ipImage为外部输入，我们不能保证图片何时被释放，我们需要自己拷贝一份到ImagePool
	LPRImage *lpImage = LPRCloneImage(ipImage);
	lPoolData.mpImage = lpImage;
	// 遍历输入的Image和ObjectMuli中的每个物体，检测每个物体的违章情况并记录到mBreakRules里
	for(int index = 0; index < lObjectCount; ++index)
	{		
		lObject = ipObjectMulti->objects[index];
		// objectRatioRECT 转化后的矩形
		ToRatioRECT(lObject.rect, imageXRatio, imageYRatio, objectRatioRECT);
		//lPoolData.mRatioRectMap[lObject.uid] = objectRatioRECT;
		// 抓取物体所需图片
		GrabStopLineImage(objectRatioRECT, lObject, ipImage);
		GrabCenterLineImage(objectRatioRECT, lObject, ipImage);
		GrabVirtualLineImage(objectRatioRECT, lObject, ipImage);	
		GrabLeftTurnLineImage(objectRatioRECT, lObject, ipImage);
		GrabRightTurnLineImage(objectRatioRECT, lObject, ipImage);
		// 记录物体的车道信息
		LoopMap::iterator itLoop = mLoopMap.find(lObject.uid);
		if(itLoop == mLoopMap.end())
			mLoopMap.insert(std::make_pair(lObject.uid, lObject.nLoopID));

		int lObjectHistoryStatus = VSD_BR_NONE;	
		int lMaxPriority = 0;		
		PriorityMap::iterator itPriorityMap = mPriorityMap.find(lObject.uid);
		if(itPriorityMap != mPriorityMap.end())
			lMaxPriority = itPriorityMap->second;	
		lPoolData.mBreakRules[lObject.uid] = VSD_BR_NONE;	
		bool isBreakRule = false;
		// 判断是否闯红灯
		if (mEventAPPParam.mRulePriority[getIndex(VSD_BR_RED_LIGHT)] > lMaxPriority)
		{
			isBreakRule = this->IsBreakRedLight(isRedLightOn[lObject.nLoopID], objectRatioRECT, crossRatio);
			if (isBreakRule)
			{
				lPoolData.mBreakRules[lObject.uid] |= VSD_BR_RED_LIGHT;
				lMaxPriority = MaxT(lMaxPriority, mEventAPPParam.mRulePriority[getIndex(VSD_BR_RED_LIGHT)]);
			}
		}
		// 判断是否压车道
		if (mEventAPPParam.mRulePriority[getIndex(VSD_BR_CROSS_LANE)] > lMaxPriority)
		{
			isBreakRule = this->IsBreakCrossLine(lObject.nLoopID, objectRatioRECT, crossRatio);
			if (isBreakRule)
			{
				lPoolData.mBreakRules[lObject.uid] |= VSD_BR_CROSS_LANE;
				lMaxPriority = MaxT(lMaxPriority, mEventAPPParam.mRulePriority[getIndex(VSD_BR_CROSS_LANE)]);
			}
		}
		// 判断是否辆违章左转
		if (mEventAPPParam.mRulePriority[getIndex(VSD_BR_TURN_LEFT)] > lMaxPriority)
		{
			isBreakRule = this->IsBreakTurnLeft(objectRatioRECT, lObject.nLoopID, crossRatio);
			if (isBreakRule)
			{
				lPoolData.mBreakRules[lObject.uid] |= VSD_BR_TURN_LEFT;
				lMaxPriority = MaxT(lMaxPriority, mEventAPPParam.mRulePriority[getIndex(VSD_BR_TURN_LEFT)]);
			}
		}
		// 判断是否违章右转
		if (mEventAPPParam.mRulePriority[getIndex(VSD_BR_TURN_RIGHT)] > lMaxPriority)
		{
			isBreakRule = this->IsBreakTurnRight(objectRatioRECT, lObject.nLoopID, crossRatio);
			if (isBreakRule)
			{
				lPoolData.mBreakRules[lObject.uid] |= VSD_BR_TURN_RIGHT;
				lMaxPriority = MaxT(lMaxPriority, mEventAPPParam.mRulePriority[getIndex(VSD_BR_TURN_RIGHT)]);
			}			
		}
		// 判断是否违章直行
		if (mEventAPPParam.mRulePriority[getIndex(VSD_BR_STRAIGHT_THROUGH)] > lMaxPriority)
		{
			isBreakRule = this->IsBreakStraight(objectRatioRECT, lObject.nLoopID);
			if (isBreakRule)
			{
				lPoolData.mBreakRules[lObject.uid] |= VSD_BR_STRAIGHT_THROUGH;
				lMaxPriority = MaxT(lMaxPriority, mEventAPPParam.mRulePriority[getIndex(VSD_BR_STRAIGHT_THROUGH)]);
			}
		}
		// 判断是否逆行
		if (mEventAPPParam.mRulePriority[getIndex(VSD_BR_REVERSE)] > lMaxPriority)
		{
			isBreakRule = this->IsBreakReverse(lObject.uid, *ipObjectTrackMulti, lObject);
			if (isBreakRule)
			{
				lPoolData.mBreakRules[lObject.uid] |= VSD_BR_REVERSE;
				lMaxPriority = MaxT(lMaxPriority, mEventAPPParam.mRulePriority[getIndex(VSD_BR_REVERSE)]);
			}
		}
		// 判断是否违章停车
		if (mEventAPPParam.mRulePriority[getIndex(VSD_BR_STOP)] > lMaxPriority)
		{
			isBreakRule = this->IsBreakStop(lObject.uid, *ipObjectTrackMulti, objectRatioRECT, lObject);
			if (isBreakRule)
			{
				lPoolData.mBreakRules[lObject.uid] |= VSD_BR_STOP;
				lMaxPriority = MaxT(lMaxPriority, mEventAPPParam.mRulePriority[getIndex(VSD_BR_STOP)]);
			}
		}
		// 记录违章行为
		mStatusMap[lObject.uid] |= lPoolData.mBreakRules[lObject.uid];
		mPriorityMap[lObject.uid] = lMaxPriority;	
	}
	// 把当前PoolData压入队尾
	mImagePool.push_back(lPoolData);
	TranversePool(opResult);
	return APP_OK;
}

void EventAPPImpl::ConstructVideo(int iRuleType, int uid, int iStartIndex)//, EventMultiAPPResult* opResultMulti, int& orResultCount)
{
	// objects是否有违规情况，并且是否要录制iRuleType
	if((mEventAPPParam.mRuleSwitch[getIndex(iRuleType)]))
	{
		// 如果有优先级更高的breakrule（包括相同的breakrule），我们简单的返回
		//if(pRecordMap->find(uid) != pRecordMap->end() || ((mEventAPPParam.mRulePriority)[(*pRuleIndexMap)[iRuleType]]) < (*pPriorityMap)[uid] || (iRuleType == VSD_BR_NONE && (*pStatusMap)[uid] != VSD_BR_NONE))
		if(mRecordMap.find(uid) != mRecordMap.end() || ((mEventAPPParam.mRulePriority)[getIndex(iRuleType)]) < mPriorityMap[uid] )//|| (iRuleType == VSD_BR_NONE && mStatusMap[uid] != VSD_BR_NONE))
			return;
		mRecordMap[uid] = 1;
		// 填充EventAPPResult的mVideoImage信息

		ObjectVideoImageMap::iterator itObjectVideoImage = mVideoImage.find(uid);
		if (itObjectVideoImage != mVideoImage.end())
		{
			if (itObjectVideoImage->second.mRuleType == iRuleType)
			{
				assert(false);
			}
			else
			{
				for(int i = 0; i < itObjectVideoImage->second.mVideoImage.size(); ++i)
				{
					LPRReleaseImage(itObjectVideoImage->second.mVideoImage[i]);
				}
				LPRReleaseImage(itObjectVideoImage->second.mBreakRuleImage);
			}
		}
		RuleVideoImage lRuleVideoImage;
		lRuleVideoImage.mRuleType = iRuleType;
		int lBeginIndex =  MaxT(iStartIndex - mEventAPPParam.mRecordParam.mBreakRuleAhead[getIndex(iRuleType)], 0);
		int lEndIndex = MinT(mEventAPPParam.mRecordParam.mBreakRuleBehind[getIndex(iRuleType)] + iStartIndex, mMaxPoolLength - 1);
		int lSizeToCopy = lEndIndex - lBeginIndex + 1;
		lSizeToCopy = MinT(lSizeToCopy, MAX_FRAME_AHEAD + MAX_FRAME_BEHIND);
		LPRImage* lpImage = NULL;
		for (int i = 0; i < lSizeToCopy; ++i )
		{
			lpImage = LPRCloneImage(mImagePool.at(i + lBeginIndex).mpImage);
			lRuleVideoImage.mVideoImage.push_back(lpImage);
		}
		lpImage = LPRCloneImage(mImagePool.at(iStartIndex).mpImage);
		lRuleVideoImage.mBreakRuleImage = lpImage;
		mVideoImage[uid] = lRuleVideoImage;
	}
	return;
}

EventAPPImpl::~EventAPPImpl()
{
	while(!mImagePool.empty())
	{
		LPRReleaseImage(mImagePool.front().mpImage);
		mImagePool.pop_front();
	}

	for(CaptureImageMap::iterator it = mTouchStopLineImage.begin(); it != mTouchStopLineImage.end(); ++it)
		LPRReleaseImage(it->second);

	mLPR.Fini();

	for(CaptureImageMap::iterator it = mLeaveStopLineImage.begin(); it != mLeaveStopLineImage.end(); ++it)
		LPRReleaseImage(it->second);

	for(CaptureImageMap::iterator it = mTouchCentreLineImage.begin(); it != mTouchCentreLineImage.end(); ++it)
		LPRReleaseImage(it->second);

	for(CaptureImageMap::iterator it = mTouchVirtualLoopLineImage.begin(); it != mTouchVirtualLoopLineImage.end(); ++it)
		LPRReleaseImage(it->second);

	for(CaptureImageMap::iterator it = mTouchLeftTurnImage.begin(); it != mTouchLeftTurnImage.end(); ++it)
		LPRReleaseImage(it->second);
	
	for(CaptureImageMap::iterator it = mTouchRightTurnImage.begin(); it != mTouchRightTurnImage.end(); ++it)
		LPRReleaseImage(it->second);

	for(ObjectVideoImageMap::iterator it = mVideoImage.begin(); it != mVideoImage.end(); ++it)
	{
		int lSize = it->second.mVideoImage.size();
		for(int i = 0; i < lSize; ++i)
		{
			LPRReleaseImage(it->second.mVideoImage[i]);
		}
		if (it->second.mRuleType != VSD_BR_NONE)
		{
			LPRReleaseImage(it->second.mBreakRuleImage);
		}
	}

	LPRReleaseImage(mpDecodeBuffer);
}

APPRESULT EventAPPImpl::AddSubTitle(const LPRImage* ipImage, const EventSubtitleOverlay& irSubTitleOverlay, const EventSubtitleImages* ipSubtitleImages, LPRImage** oppImage)
{
	if (ipImage == NULL || ipSubtitleImages == NULL || oppImage == NULL)
	{
#ifdef __DEBUG
		TRACE("EventAPP::AddSubTitle input null pointer");
#endif
		return APP_INPUT_NULL_POINTER;
	}
	*oppImage = LPROverlaySubtitle(ipImage, irSubTitleOverlay, ipSubtitleImages);
	if(*oppImage == NULL)
		return APP_FAIL;
	return APP_OK;
}

APPRESULT EventAPPImpl::SynthesisImages(LPRImage** ipImage, int iNumOfImages, const VSDRect& irRect, LPRImage** oppImage)
{
	if (ipImage == NULL || oppImage == NULL)
	{
#ifdef __DEBUG
		TRACE("EventAPP::SynthesisImages input null pointer");
#endif
		return APP_INPUT_NULL_POINTER;
	}


	mEventAPPParam.mImageSynthesis.mNumberofImage = iNumOfImages;
	*oppImage = mImageSynthesis.synthesis(ipImage, mEventAPPParam.mImageSynthesis, irRect);
	if (*oppImage == NULL)
		return APP_FAIL;
	return APP_OK;
}

APPRESULT EventAPPImpl::Convert2Media(LPRImage** ipImage, int iNumOfImages, EventMedia& orMedia)
{
	if (ipImage == NULL)
	{
#ifdef __DEBUG 
		TRACE("EventAPP::Convert2Media input null pointer");
#endif
		return APP_INPUT_NULL_POINTER;
	}

	bool ret = mMediaConverter.imgs2media(ipImage, iNumOfImages, orMedia);
	if(!ret)
		return APP_FAIL;
	return APP_OK;
}

 void __stdcall FreeAPPResult(EventAPPResult* ipAPPResult)
{
	if (!ipAPPResult)
	{
#ifdef __DEBUG
		TRACE("FreeAPPResult fault input param");
#endif
	}

	for(int i = 0; i < ipAPPResult->mNumOfVideoImage; ++i)
		LPRReleaseImage(ipAPPResult->mVideoImage[i]);

	for(int i = 0; i < ipAPPResult->mNumOfSynthesisImage; ++i)
		LPRReleaseImage(ipAPPResult->mSynthesisImage[i]);
	if (ipAPPResult->mPlateImage != NULL)
		LPRReleaseImage(ipAPPResult->mPlateImage);
}

void __stdcall FreeEventMedia(EventMedia *pMedia)
{
	av_free(pMedia->mBufferPtr);
}

void __stdcall FreeMultiAPPResult(EventMultiAPPResult* ipMultiAPPResult)
{
	if (!ipMultiAPPResult)
	{
#ifdef __DEBUG
		TRACE("FreeAPPResult fault input param");
#endif
	}
	for(int i = 0; i < ipMultiAPPResult->mNumOfResult; ++i)
	{
		FreeAPPResult(&(ipMultiAPPResult->mppAPPResult[i]));
	}
}

// 去掉string前后连续的空格和制表符
void __stdcall MyStrim(std::string& toTrimed)
{
	int start = 0;
	int end = toTrimed.size() - 1;
	while( start <= end && (toTrimed.at(start) == ' '|| toTrimed.at(start) == '\t'))
		++start;
	
	while( end > start && (toTrimed.at(end) == ' '|| toTrimed.at(end) == '\t'))
		--end;
	toTrimed = toTrimed.substr(start, end - start + 1);
}

static int str2wstr(wchar_t *dst, const char *src, int srcSize)
{
	int cchWideChar = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)src, srcSize, 0, 0);
	if(cchWideChar < 0) return false;
	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)src, srcSize, dst, cchWideChar);
	dst[cchWideChar] = L'\0';
	return cchWideChar;
}


typedef std::map<std::string, std::string> KeyValue;

template<typename T>
APPRESULT __stdcall CheckAndSetValue(const KeyValue& irKeyValue, const std::string& irKey, T iMinVlaue, T iMaxValue, T& orValue)
{
	KeyValue::const_iterator it = irKeyValue.find(irKey);
	if (it == irKeyValue.end())
	{
#ifdef __DEBUG
		TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
		return APP_CONFIG_PARAM_FAULT;
	}
	else
	{
		std::stringstream lValue(it->second);
		T number = -1;
		lValue >> number;
		if (number < iMinVlaue || number > iMaxValue)
		{
#ifdef __DEBUG
			TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
			return APP_CONFIG_PARAM_FAULT;
		}
		orValue = number;
	}
	return APP_OK;
}

void __stdcall InitEventAPPParam(EventAPPParam* opEventAPPParam)
{
	::memset(opEventAPPParam, 0, sizeof(EventAPPParam));
}

APPRESULT __stdcall EventAPP_LoadParam(const char* ipFileName, EventAPPParam* ipEventParam)
{
	if (!ipFileName || !ipEventParam)
	{
#ifdef __DEBUG
		TRACE("EventAPP_LoadParam input fault");
#endif
		return APP_INPUT_NULL_POINTER;
	}
	std::ifstream ifs(ipFileName);
	std::string lineString;
	KeyValue keyValue;
	string::size_type nPosition;
	string keyString;
	string valueString;
	while(std::getline(ifs,lineString))
	{
		MyStrim(lineString);
		if(lineString.empty() || lineString[0] == '#')
			continue;
		nPosition = lineString.find('=');
		if(nPosition == std::string::npos || nPosition == 0)
		{
#ifdef __DEBUG
			TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
			return APP_CONFIG_PARAM_FAULT;
		}
		keyString = lineString.substr(0, nPosition);
		MyStrim(keyString);
		if(keyString.empty())
		{
#ifdef __DEBUG
			TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
			return APP_CONFIG_PARAM_FAULT;
		}
		valueString = lineString.substr(nPosition + 1, lineString.size() - nPosition - 1);
		MyStrim(valueString);
		if(valueString.empty())
		{
#ifdef __DEBUG
			TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
			return APP_CONFIG_PARAM_FAULT;
		}
		keyValue[keyString] = valueString;
	}

	LPRRESULT lResult = LPR_OK;

	// 初始化左转线p1点的x坐标
	APPRESULT lAPPResult = CheckAndSetValue(keyValue, "TurnLeft.pt1.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mLeftTurnLine.pt1.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//初始化左转线p1点的y坐标
	lAPPResult = CheckAndSetValue(keyValue, "TurnLeft.pt1.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mLeftTurnLine.pt1.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "TurnLeft.pt2.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mLeftTurnLine.pt2.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "TurnLeft.pt2.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mLeftTurnLine.pt2.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	// 
	lAPPResult = CheckAndSetValue(keyValue, "TurnRight.pt1.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mRightTurnLine.pt1.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "TurnRight.pt1.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mRightTurnLine.pt1.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "TurnRight.pt2.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mRightTurnLine.pt2.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "TurnRight.pt2.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mRightTurnLine.pt2.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	// 
	lAPPResult = CheckAndSetValue(keyValue, "Straight.pt1.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mStraightLine.pt1.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "Straight.pt1.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mStraightLine.pt1.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "Straight.pt2.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mStraightLine.pt2.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "Straight.pt2.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mStraightLine.pt2.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	
	lAPPResult = CheckAndSetValue(keyValue, "StopLine.pt1.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mStopLine.pt1.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "StopLine.pt1.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mStopLine.pt1.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "StopLine.pt2.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mStopLine.pt2.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "StopLine.pt2.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mStopLine.pt2.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	lAPPResult = CheckAndSetValue(keyValue, "CentreLine.pt1.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mCentreLine.pt1.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "CentreLine.pt1.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mCentreLine.pt1.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "CentreLine.pt2.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mCentreLine.pt2.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "CentreLine.pt2.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mCentreLine.pt2.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	lAPPResult = CheckAndSetValue(keyValue, "StopForbidRect.left", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mStopForbidRect.left);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "StopForbidRect.top", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mStopForbidRect.top);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "StopForbidRect.right", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mStopForbidRect.right);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "StopForbidRect.bottom", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mStopForbidRect.bottom);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	// 初始化压线阈值
	lAPPResult = CheckAndSetValue(keyValue, "CrossRatio", 0, 100, ipEventParam->mRatioToCrossLine);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	lAPPResult = CheckAndSetValue(keyValue, "ReverseRatio", 0, 100, ipEventParam->mReverseRatio);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	lAPPResult = CheckAndSetValue(keyValue, "StopRatio", 0, 100, ipEventParam->mStopRatio);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	// 初始化各种rule的录制帧范围
	/*
	lAPPResult = CheckAndSetValue(keyValue, "RecordBreakNoneFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[0]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordBreakNoneFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[0]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
		*/
	ipEventParam->mRecordParam.mBreakRuleAhead[0] = 0;
	ipEventParam->mRecordParam.mBreakRuleBehind[0] = 0;

	lAPPResult = CheckAndSetValue(keyValue, "RecordTurnLeftFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[1]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordTurnLeftFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[1]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordTurnRightFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[2]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordTurnRightFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[2]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordStraightFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[3]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordStraightFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[3]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordCrossLineFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[4]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordCrossLineFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[4]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordReverseFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[5]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordReverseFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[5]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordRedLightFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[6]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordRedLightFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[6]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordStopFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[7]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordStopFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[7]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordHighSpeedFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[8]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordHighSpeedFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[8]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordLowSpeedFramAhead", 0, MAX_FRAME_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[9]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordLowSpeedFramBehind", 0, MAX_FRAME_BEHIND, ipEventParam->mRecordParam.mBreakRuleBehind[9]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	lAPPResult = CheckAndSetValue(keyValue, "VideoFormat", 0, 2, ipEventParam->mRecordParam.mVideoFormat);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	lAPPResult = CheckAndSetValue(keyValue, "VideoFramFrequent", 0, 20, ipEventParam->mRecordParam.mFrameFrequent);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "VideoBitFrequent", 0, 1000000000, ipEventParam->mRecordParam.mBitFrequent);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "ImageSynthesisNum", 0, 10, ipEventParam->mImageSynthesis.mNumberofImage);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "ImageSynthesisOrientation", 0, 3, ipEventParam->mImageSynthesis.mPicOrientation);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "ImageSynthesisZoom", 0, 100, ipEventParam->mImageSynthesis.mZoomRatio);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "PlateCaptureSwitch", 0, 1, ipEventParam->mPlateCaptureSwitch);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakNoneSwitch", 0, 1, ipEventParam->mRuleSwitch[0]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakTurnLeftSwitch", 0, 1, ipEventParam->mRuleSwitch[1]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakTurnRightSwitch", 0, 1, ipEventParam->mRuleSwitch[2]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakStraightSwitch", 0, 1, ipEventParam->mRuleSwitch[3]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakCrossSwitch", 0, 1, ipEventParam->mRuleSwitch[4]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakReverseSwitch", 0, 1, ipEventParam->mRuleSwitch[5]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakRedLightSwitch", 0, 1, ipEventParam->mRuleSwitch[6]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakStopSwitch", 0, 1, ipEventParam->mRuleSwitch[7]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakHighSpeedSwitch", 0, 1, ipEventParam->mRuleSwitch[8]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakLowSpeedSwitch", 0, 1, ipEventParam->mRuleSwitch[9]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	/*
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakNonePriority", 0, 9, ipEventParam->mRulePriority[0]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
		*/

	ipEventParam->mRulePriority[0] = 0;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakTurnLeftPriority", 0, 9, ipEventParam->mRulePriority[1]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakTurnRightPriority", 0, 9, ipEventParam->mRulePriority[2]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakStraightPriority", 0, 9, ipEventParam->mRulePriority[3]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakCrossPriority", 0, 9, ipEventParam->mRulePriority[4]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakReversePriority", 0, 9, ipEventParam->mRulePriority[5]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakRedLightPriority", 0, 9, ipEventParam->mRulePriority[6]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakStopPriority", 0, 9, ipEventParam->mRulePriority[7]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakHighSpeedPriority", 0, 9, ipEventParam->mRulePriority[8]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RuleBreakLowSpeedPriority", 0, 9, ipEventParam->mRulePriority[9]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	return APP_OK;
}


static bool CmpFileName(const string& a, const string& b)
{
	return a.length() == b.length() ? a < b : a.length() < b.length();
}


void EmumAllJPGFileInFolder(std::wstring folder, std::vector<std::wstring>& files)
{
	wstring imgPath = folder;
	folder += L"\\*.*";
	WIN32_FIND_DATA		findData;
	//wchar_t pFolder[256];
	//str2wstr(pFolder, folder.c_str(), folder.size());
	HANDLE  hFirstFile = ::FindFirstFile(folder.c_str(), &findData );
	if( hFirstFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			wstring fileName( findData.cFileName );
			int pos = fileName.find_last_of(L'.' );
			if( pos != -1 )
			{
				wstring ext = fileName.substr(pos+1, fileName.length()-pos);
				/// 寻找jpeg文件
				if( ext == L"jpg" || ext == L"JPG" || ext == L"jpeg" || ext == L"JPEG")
				{
					files.push_back( imgPath + L"\\" + findData.cFileName );
				}
			}

		}while( FindNextFile( hFirstFile, &findData ) );

		::FindClose( hFirstFile );
	}
	//std::sort(files.begin(), files.end(), CmpFileName);
}

int main(int argc, char *argv[])
{
	VSDEventParam lParam;	
	VSDEvent_LoadParam("F:\\EventData\\images\\120327\\VSDEvent.ini", &lParam);
	VSDEvent lEvent;
	LPRRESULT l = lEvent.Init(lParam);
	
	EventAPPParam lAPPParam;
	//////////////////////////////////////////////////////////////////////////
	InitEventAPPParam(&lAPPParam);
	lAPPParam.mVSDParam = lParam;
	EventAPP_LoadParam("E:\\work\\EventAPP\\EventAPP\\EventAPP\\EvenAPP_Param.ini", &lAPPParam);
	EventAPP lEventApp;
	//lEventApp.Init(lAPPParam);
	lEventApp.Init(lAPPParam);
	std::wstring fileDir(L"F:\\EventData\\images\\120327");
	std::vector<wstring> lFiles;
	EmumAllJPGFileInFolder(fileDir, lFiles);
	wstring fileName;
	time_t tBegin;
	tBegin = time(NULL);
	//////////////////////////////////////////////////////////////////////////
	EventSubtitleImages* pEventSubtitleIamge;
	int font[2] = {0,1};
	pEventSubtitleIamge = LPRGenerateCharacterImagesDat(L"1234567890km", font, 2, 128);
	
	for(vector<wstring>::iterator it = lFiles.begin(); it != lFiles.end(); ++it)
	{
		fileName = *it;
		std::ifstream ifs(fileName.c_str(), std::ios::binary);
		int nJpgLen = 0;
		char *pJpgBuf = NULL;
		if ( ifs.is_open() )
		{
			ifs.seekg( 0 , std::ios::end );
			nJpgLen = ifs.tellg();

			if ( nJpgLen > 0 )
			{
				pJpgBuf = new char [nJpgLen];
				ifs.seekg( 0 , std::ios::beg );
				ifs.read((char *)pJpgBuf, nJpgLen);
			}
			ifs.close();
		}

		LPRImage imgJPG;
		imgJPG.nColorMode = CS_JPEG;
		imgJPG.pData = (unsigned char *)pJpgBuf;
		imgJPG.imageSize = nJpgLen;
		imgJPG.info.nCamID = 0;

		EventMultiAPPResult lAPPResult;
		InitEventMultiAPPResult(lAPPResult);
		VSDObjectMulti lObjectMulti;
clock_t startclock_VSD = clock();
		LPRRESULT lResult = lEvent.ProcessFrame(&imgJPG, &lObjectMulti);
clock_t endclock_VSD = clock();
//cout << "time elaspeVSD:" << endclock_VSD - startclock_VSD << endl;
		VSDObjectTrackMulti lObjectTrackMulti;
		VSDObjectTrackMulti_Init(&lObjectTrackMulti);
		int lLights[MAX_VIRTUAL_LOOPS] = {0,0,0,0};
		lResult = lEvent.GetAllTracks(&lObjectTrackMulti);
		clock_t startclock = clock();
		lEventApp.ProcessFrame(&imgJPG,&lObjectMulti, &lObjectTrackMulti, lLights, &lAPPResult);
		clock_t endclock = clock();
		lTimeConsumeLog << "time elaspe:" << endclock - startclock << endl;
		wchar_t resultFileName[256];
		char* buf;
		ofstream ofs;
		EventSubtitleOverlay lSubTitleOverlay;
		EventFont lFont[2] = {{50, 0, 255, 0, 0, 1, 500, 200}, {80, 1, 0, 0, 255, 0, 800, 300}};
		lSubTitleOverlay.mFonts = lFont;
		lSubTitleOverlay.mFontSize = 2;
		wchar_t* lSubtiles[2] = {L"京123", L"津456"};
		lSubTitleOverlay.mSubtitles = lSubtiles;
		lSubTitleOverlay.mSubtitleSize = 2;

		for(int i = 0; i < lAPPResult.mNumOfResult; ++i)
		{
			wchar_t lPlate[LPR_PLATE_STR_LEN];
			lPlate[0] = 0;
			EventAPPResult lResult = lAPPResult.mppAPPResult[i];
			if (lResult.mBreakRule == VSD_BR_NONE)
			{
				if (lResult.mNumOfSynthesisImage > 0)
				{
					swprintf(resultFileName,256, L"F:\\resultdata\\ID%d_车道%d.jpg", lResult.mID, lResult.mLoopID);
					//swprintf(resultFileName,256, L"F:\\resultdata\\ID%d_车道%d_车牌%ls.jpg", lResult.mID, lResult.mLoopID, lResult.mPlate);
					ofs.open(resultFileName, std::ios::binary);
					ofs.write((const char*)lResult.mSynthesisImage[0]->pData, lResult.mSynthesisImage[0]->imageSize);
					ofs.close();
				}
			}
			else
			{
				if (lResult.mNumOfSynthesisImage == 3)
				{
					lEventApp.GetPlate(lResult.mPlateImage, lResult.mRect, lPlate);
					//swprintf(resultFileName,256, L"F:\\resultdata\\ID%d_违反%d_车道%d.jpg", lResult.mID, lResult.mBreakRule, lResult.mLoopID);
					swprintf(resultFileName,256, L"F:\\resultdata\\ID%d_违反%d_车道%d_车牌%ls.jpg", lResult.mID, lResult.mBreakRule, lResult.mLoopID, lPlate);
					ofs.open(resultFileName, std::ios::binary);
					LPRImage* lpImage = NULL;
					APPRESULT retCode = lEventApp.SynthesisImages(lResult.mSynthesisImage, lResult.mNumOfSynthesisImage, lResult.mRect, &lpImage);
					buf = new char[10 * nJpgLen]; 
					int length;
					LPREncodeImage(lpImage, (unsigned char*)buf, &length,LPR_ENCODE_FORMAT_JPG, 40);
					LPRImage* pSubtitleImage = new LPRImage;

					pSubtitleImage->pData = (unsigned char*) buf;
					pSubtitleImage->imageSize = length;
					LPRImage* lpImageSubtile;
					lEventApp.AddSubTitle(pSubtitleImage,lSubTitleOverlay, pEventSubtitleIamge, &lpImageSubtile);
					LPREncodeImage(lpImageSubtile, (unsigned char*)buf, &length,LPR_ENCODE_FORMAT_JPG, 80);
					ofs.write((const char*)buf, length);
					ofs.close();
					delete[] buf;
					LPRReleaseImage(lpImage);
					LPRReleaseImage(lpImageSubtile);
					delete pSubtitleImage;
				}
			/*
				EventMedia lMedia;
				lEventApp.Convert2Media(lResult.mVideoImage, lResult.mNumOfVideoImage, lMedia);
				swprintf(resultFileName,256, L"F:\\resultdata\\ID%d_违反%d_车道%d.avi", lResult.mID, lResult.mBreakRule, lResult.mLoopID);
				//swprintf(resultFileName,256, L"F:\\resultdata\\ID%d_违反%d_车道%d_车牌%ls.avi", lResult.mID, lResult.mBreakRule, lResult.mLoopID, lResult.mPlate);
				ofs.open(resultFileName, std::ios::binary);
				ofs.write((const char*)lMedia.mBufferPtr, lMedia.mBufferSize);
				ofs.close();
				FreeEventMedia(&lMedia);
				*/
				
			}
		}
		
		delete[] pJpgBuf;
		FreeMultiAPPResult(&lAPPResult);
	}
	
	time_t tEnd = time(NULL);
	cout << "运行" << (tEnd - tBegin) / 60 << "分" << (tEnd - tBegin)%60 << "秒" << endl;
	LPRReleaseSubtitleImages(pEventSubtitleIamge);
	int i;
	std::cin >> i;
	return 0;
}
