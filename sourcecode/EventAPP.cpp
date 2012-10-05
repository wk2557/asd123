//*********************************************************
//This file defines the basic interface of EventAPP
//Author:WK
//Data:2012.09.22`
//**********************************************************
#define __DEBUG
#include "EventAPP.h"
#include "MediaConverter.h"
#include "SubtitleOverlay.h"
#include <cstring>
#include <deque>
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <LPRVideo.h>
#include <LPR.h>
#include "ImageSynthesis.h"



using namespace std;

#if defined(WIN32)
#define Int64 __int64
#elif  defined(LINUX)
#define Int64 long long
#else
#error "No platform specified!"
#endif

#ifdef __DEBUG
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#define TRACE(string) std::cout << string << std::endl
ofstream lNoBreakRuleLog("NoBreakRule.txt");
wofstream lBreakRuleHistoryLog("BreakRuleHistory.txt");
ofstream lBreakOutputLog("BreakRuleOutput.txt");
std::wstring lCurrentPicName;
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
static inline void __stdcall ToRatioRECT(const VSDRect& irRect, double iXRatio, double iYRatio, VSDRatioRECT& orRECT)
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

// 判断点是否在直线上，因为irLine来自于矩形的边框，所以irLine总是平行于x或y坐标轴
static bool __stdcall IsOnLine(const VSDRatioPoint& irPoint, const VSDRatioLine& irLine)
{
	if(irLine.pt1.x == irLine.pt2.x)
		return irPoint.x == irLine.pt1.x && ((irPoint.y - irLine.pt1.y) * (irPoint.y - irLine.pt2.y) <= 0);
	else
		return irPoint.y == irLine.pt1.y && ((irPoint.x - irLine.pt1.x) * (irPoint.x - irLine.pt2.x) <= 0);
}

// 判断两条线段是否相交，若相交则返回true并返回orPointC，否则返回false
// ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
// ！！！！！！！！！！注意，irLineB来源于矩形的边框，所以irLineB总是平行于x或y坐标！！！！！！！！！！
// ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
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
	
	double t = (double)(area_cda) / (area_abd - area_abc);
	orPointC.x = int(a.x + t * (b.x - a.x));
	orPointC.y = a.y + t * (b.y - a.y);
	return true;
}


static inline bool __stdcall IsSame(const VSDRatioPoint& irPointA, const VSDRatioPoint& irPointB)
{
	return irPointA.x == irPointB.x && irPointA.y == irPointB.y;
}

static double __stdcall GetCrossRatio(const VSDRatioLine& irLine, const VSDRatioRECT& irRECT)
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
	double middleX =  (double(crossPoint[0].x) + crossPoint[1].x) / 2;
	double middleY =  (double(crossPoint[0].y) + crossPoint[1].y) / 2;
	
	double areaA = (middleX - irRECT.left) * ( middleY - irRECT.top);
	double areaB = (irRECT.right - middleX) * (middleY - irRECT.top );
	double areaC = (irRECT.right - middleX) * (irRECT.bottom- middleY);
	double areaD = (middleX - irRECT.left) * (irRECT.bottom - middleY);

	double minArea = MinT(MinT(areaA,areaB), MinT(areaC, areaD));
	
	double areaRect = (irRECT.bottom - irRECT.top) * (irRECT.right - irRECT.left);
	return minArea * 4 / areaRect;
}

struct PoolData
{
	std::map<int, int> mBreakRules;
	LPRImage* mpImage;
};


typedef std::deque<PoolData> ImagePool;
typedef std::map<int, LPRImage*> VirtualLoopImagePool;
typedef std::map<int, int> StatusMap;
typedef std::map<int, wchar_t*> PlateMap; 
typedef std::list<EventAPPResult> ResultList;

EventAPP::EventAPP()
{
	mObject = new int*[15];
}

APPRESULT EventAPP::Init(const EventAPPParam& irParam)
{
	int** pValue = (int**)mObject;
	*pValue = (int*)new EventAPPParam(irParam);								// EventAPP的第一个成员为EventAPPParam
	*(pValue + 1) = (int*)new ImagePool;									// EventAPP的第二个成员为一个ImagePool，缓存图片，供合成视频和选取未违章字母用
	*(pValue + 2) = NULL;													// EventAPP的第三个成员为每个虚拟线圈的lanemark	
	int maxAhead = irParam.mRecordParam.mBreakRuleAhead[0];
	int maxBehind = irParam.mRecordParam.mBreakRuleBehind[0];
	for(int i = 1; i < RULE_TYPES; ++i)
	{
		maxAhead = MaxT(maxAhead, irParam.mRecordParam.mBreakRuleAhead[i]);
		maxBehind = MaxT(maxBehind, irParam.mRecordParam.mBreakRuleBehind[i]);
	}
	*(pValue + 3) = new int(maxAhead + maxBehind);							// EventAPP的第六个成员保存ImagePool的最大长度
	*(pValue + 4) = (int*)new VirtualLoopImagePool;							// EventAPP的第五个成员保存每个车辆的在停车线附近的照片
	*(pValue + 5) = (int*)new StatusMap;									// EventAPP的第六个成员保存从跟踪开始到结束的历史breakrule状态的叠加值
	*(pValue + 6) = (int*)new StatusMap;									// EventAPP的第七个成员保存每个物体的某个breakrule是否已经录制过视频，防止录制相同物体的相同违章
	*(pValue + 7) = (int*)new MediaConverter((EventAPPViedoFormat)irParam.mRecordParam.mViedoFormat, irParam.mRecordParam.mBitFrequent);  // EventAPP的第八个成员保存MediaConvertoer，用了录制视频
	SubtitleOverlay* pSubTitleOverlay = &SubtitleOverlay::getInstance();
	pSubTitleOverlay->initialize(irParam.mFont.mCharactors, irParam.mFont);
	*(pValue + 8) = (int*)pSubTitleOverlay;					                // EventAPP的第九个成员用来叠加字母
	*(pValue + 9) = NULL;													// EventAPP的第十个成员用了识别车牌
	*(pValue + 10) = (int*)new PlateMap;									// EventAPP的第十一个成员用了保存每个车的车牌号
	*(pValue + 11) = (int*)new VirtualLoopImagePool;						// EventAPP的第五个成员保存每个车辆的在停车线附近的照片
	*(pValue + 12) = NULL;													// EventAPP的第六个成员保存图片宽度与相对宽度的比值
	*(pValue + 13) = NULL;													// EventAPP的第七个成员保存图片高度与相对高度的比值
	*(pValue + 14) = (int*)new ImageSynthesis;
	return APP_OK;
}

APPRESULT EventAPP::ProcessFram(LPRImage *ipImage, const VSDObjectMulti* ipObjectMulti,int isRedLightOn[MAX_VIRTUAL_LOOPS], EventMultiAPPResult* opResult)
{
	opResult->mNumOfResult = 0;
	if(!ipImage || !ipObjectMulti)
	{
#ifdef __DEBUG
		TRACE("EventAPP::ProcessFram input fault");
#endif
		return APP_INPUT_NULL_POINTER;
	}

	if (ipImage->nColorMode != CS_JPEG)
	{
#ifdef __DEBUG
		TRACE("EventAPP::ProcessFram input fault");
#endif
		return APP_IMAGE_FORMAT_FAULT;
	}

	if (ipObjectMulti->nObjects <= 0)
	{
#ifdef __DEBUG
		TRACE("EventAPP::ProcessFram number of input ojbect is zero");
#endif
		return APP_OK;
	}

	// 得到mObject对应的各项成员
	int** pValue = (int**)mObject;
	EventAPPParam* pAPPParam = (EventAPPParam*)(*pValue);
	ImagePool* pPool = (ImagePool*)(*(pValue + 1));
	VSDRatioLine* laneMark = (VSDRatioLine*)(*(pValue + 2));
	int* pPoolLength =(int*)(*(pValue + 3));
	VirtualLoopImagePool* pVirtualLoopImage = (VirtualLoopImagePool*)(*(pValue + 4));
	StatusMap* pStatusMap = (StatusMap*)(*(pValue + 5));
	StatusMap* pRemoveStatusMap = (StatusMap*)(*(pValue + 6));
	MediaConverter* pMediaConverter = (MediaConverter*)(*(pValue + 7));
	SubtitleOverlay* pSubtitleOverlay = (SubtitleOverlay*)(*(pValue + 8));
	LPR* pLPR = (LPR*)(*(pValue + 9));
	PlateMap* pPlateMap = (PlateMap*)(*(pValue + 10));
	VirtualLoopImagePool* pVirtualLoopLeaveImage = (VirtualLoopImagePool*)(*(pValue + 11));
	int* pImageWidth = (int*)(*(pValue + 12));
	int* pImageHeight = (int*)(*(pValue + 13));
	

	// 得到VSDEventParam的参数
	VSDEventParam lVSDParam = pAPPParam->mVSDParam;
	LPRRESULT lResult = LPR_OK;	

	// 通过解码得到Image的长宽值
	if(pImageHeight == NULL || pImageWidth == NULL)
	{
		LPRImage* pDecodeImage = NULL;
		lResult = LPRDecodeImage(&pDecodeImage, (const unsigned char*)ipImage->pData, ipImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		if(lResult != LPR_OK)
		{
#ifdef __DEBUG
			TRACE("EventAPP::ProcessFram Fail to decode image");
#endif
			LPRReleaseImage(pDecodeImage);
			return APP_FAIL;
		}
		pImageHeight = new int(pDecodeImage->height);
		pImageWidth = new int(pDecodeImage->width);
		*(pValue + 12) = pImageWidth;
		*(pValue + 13) = pImageHeight;
		LPRReleaseImage(pDecodeImage);
	}

	// 解码JPG图片为RGB图片，供车牌识别用

	// 初始化每个虚拟线圈的两个VSDRatioLine，只初始化一次
	if(laneMark == NULL)
	{
		laneMark = new VSDRatioLine[MAX_VIRTUAL_LOOPS*2];
		VSDRatioPoint lTmpPoint;
		if (pImageHeight == NULL || pImageWidth == NULL)
		{
#ifdef __DEBUG
			TRACE("EventAPP::ProcessFram image height and width has not been inited");
#endif
			return APP_FAIL;
		}
		lResult = VSDEvent_GenerateLaneMark(lVSDParam.ptRoad, lVSDParam.virtualLoopLine, lVSDParam.nVirtualLoop, *pImageWidth, *pImageHeight, lVSDParam.nWidthBase, lVSDParam.nHeightBase, laneMark, &lTmpPoint);
		if(lResult != LPR_OK)
		{
	#ifdef __DEBUG
			TRACE("EventAPP::ProcessFram fail to generat lane mark");
	#endif
			return APP_FAIL;
		}
		*(pValue + 2) = (int*)laneMark;
		// 因为得到的laneMark太长，我们只取虚拟线圈内的Line
		for (int index = 0; index < lVSDParam.nVirtualLoop; ++index)
		{
			laneMark[2 * index].pt1 = lVSDParam.virtualLoopLine[index].pt1;
			laneMark[2 * index + 1].pt1 = lVSDParam.virtualLoopLine[index].pt2;
		}
	}


	// 初始化LPR模块，供识别车牌用
	if (pLPR == NULL)
	{
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
		pLPR = new LPR;
		pLPR->Fini();
		lResult = pLPR->Init2(lLPRParam, *pImageWidth, *pImageHeight, true);
		if(lResult != LPR_OK)
		{
#ifdef __DEBUG
			TRACE("EventAPP::ProcessFram fail to init LPR");
#endif
			return APP_FAIL;
		}
		*(pValue + 9) = (int*)pLPR;
	}
	
	// 得到压线阈值
	double crossRatio = pAPPParam->mRatioToCrossLine;

	// 把物体的Rect转化为 RatioRect
	VSDRatioRECT objectRatioRECT;
	if(pImageHeight == NULL || pImageWidth == NULL || *pImageWidth == 0 || *pImageHeight == 0)
	{
#ifdef __DEBUG
		TRACE("图片的格式有误，无法得到图片的长宽值");
#endif
		return APP_IMAGE_FORMAT_FAULT;
	}
	double imageXRatio = lVSDParam.nWidthBase / (double)(*pImageWidth);
	double imageYRatio = lVSDParam.nHeightBase / (double)(*pImageHeight);
	
	int lObjectCount = ipObjectMulti->nObjects;
	VSDObject lObject;
	
	// 因为ipImage为外部输入，我们不能保证图片何时被释放，我们需要自己拷贝一份到ImagePool
	LPRImage *lpImage = LPRCloneImage(ipImage);

	// 构造PoolData 
	PoolData lPoolData;
	lPoolData.mpImage = lpImage;
	
	// 遍历输入的Image和ObjectMuli中的每个物体，检测每个物体的违章情况并记录到mBreakRules里
	for(int index = 0; index < lObjectCount; ++index)
	{
		lObject = ipObjectMulti->objects[index];
		// objectRatioRECT 转化后的矩形
		ToRatioRECT(lObject.rect, imageXRatio, imageYRatio, objectRatioRECT);
		// 在virtualLoopImagePool 里保存一张图片，供没有违章的车辆用。同时在这张图片里提取每个物体的车牌号到pPlateMap中
		if (lObject.status & VSD_OBJ_STATUS_TOUCH_LINE)
		{
			VirtualLoopImagePool::iterator itVirtualImage = pVirtualLoopImage->find(lObject.uid);
			PlateMap::iterator itPlateMap = pPlateMap->find(lObject.uid);
			if(itVirtualImage == pVirtualLoopImage->end())
			{
				LPRImage *pImage = LPRCloneImage(ipImage);
				pVirtualLoopImage->insert(make_pair(lObject.uid, pImage));
			}
			if(itPlateMap == pPlateMap->end())
			{
				// 初始化局部参数。
				LPRParamLocal	localParam;
				// 识别区域设为当前物体的矩形框，因为物体矩形框有时比较小，没有包括车牌所在的范围，因为我们扩大搜索范围
				int lEnlargeWidth = lObject.rect.width / 2;
				int lEnlargeHeight = lObject.rect.height / 2;
				localParam.m_rectRegion.left = MaxT(lObject.rect.x - lEnlargeWidth, 0);
				localParam.m_rectRegion.right = MinT(lObject.rect.x + lObject.rect.width + lEnlargeWidth, *pImageWidth);
				localParam.m_rectRegion.top = MaxT(lObject.rect.y - lEnlargeHeight, 0);
				localParam.m_rectRegion.bottom = MinT(lObject.rect.y + lObject.rect.height + lEnlargeHeight, *pImageHeight);
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
				LPRImage* pDecodeImage = NULL;
				lResult = LPRDecodeImage(&pDecodeImage, (const unsigned char*)ipImage->pData, ipImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
				if(lResult != LPR_OK)
				{
#ifdef __DEBUG
					TRACE("EventAPP::ProcessFram Fail to decode image");
#endif
					LPRReleaseImage(pDecodeImage);
					return APP_FAIL;
				}
				lResult =pLPR->ProcessImage(pDecodeImage, &multiOutput, multiParam, NULL );
				if (lResult != LPR_OK)
				{
#ifdef __DEBUG
					TRACE("EventAPP::ProcessFram fail to recognize plate");
#endif
					LPRReleaseImage(pDecodeImage);
					return APP_FAIL;
				}
				if(multiOutput.m_nNumOutputs > 0)
				{
					wchar_t* pPlateCharactor = new wchar_t[LPR_PLATE_STR_LEN];
					for (int i = 0; i < LPR_PLATE_STR_LEN; ++i)
					{
						pPlateCharactor[i] = multiOutput.m_outputs[0].wszRec[i];
					}
					pPlateMap->insert(make_pair(lObject.uid, pPlateCharactor));
				}
				LPRReleaseImage(pDecodeImage);
			}
		}
		else if(lObject.status & VSD_OBJ_STATUS_LEAVE_LINE)
		{
			VirtualLoopImagePool::iterator itVirtualImage = pVirtualLoopLeaveImage->find(lObject.uid);
			if(itVirtualImage == pVirtualLoopLeaveImage->end())
			{
				LPRImage *pImage = LPRCloneImage(ipImage);
				pVirtualLoopLeaveImage->insert(make_pair(lObject.uid, pImage));
			}
		}
		
		int lObjectHistoryStatus = VSD_BR_NONE;
		lPoolData.mBreakRules[lObject.uid] = VSD_BR_NONE;
		// 判断是否闯红灯
		if(isRedLightOn[lObject.nLoopID] == EVENT_APP_LIGHT_RED  && GetCrossRatio(lVSDParam.virtualLoopLine[lObject.nLoopID], objectRatioRECT) >= crossRatio)
		{
#ifdef __DEBUG
			lBreakRuleHistoryLog << lCurrentPicName << "车" << lObject.uid  << "闯红灯，车道" << lObject.nLoopID << std::endl; 
#endif
			lPoolData.mBreakRules[lObject.uid] |= VSD_BR_RED_LIGHT;
		}
		// 判断是否压车道
		if(GetCrossRatio(laneMark[2 * lObject.nLoopID], objectRatioRECT) >= crossRatio)
		{
#ifdef __DEBUG
			lBreakRuleHistoryLog << lCurrentPicName << "车" << lObject.uid << "压到" << lObject.nLoopID  << std::endl;
#endif
			lPoolData.mBreakRules[lObject.uid] |= VSD_BR_CROSS_LANE;
		}
		if(GetCrossRatio(laneMark[2 * lObject.nLoopID + 1], objectRatioRECT) >= crossRatio)
		{
#ifdef __DEBUG
			lBreakRuleHistoryLog << lCurrentPicName << "车" << lObject.uid << "压到" << lObject.nLoopID  <<  std::endl;
#endif
			lPoolData.mBreakRules[lObject.uid] |= VSD_BR_CROSS_LANE;
		}

		// 判断是否辆违章左转
		if(!(lVSDParam.loopLaneProperty[lObject.nLoopID] & VSD_LANE_TURN_LEFT) && GetCrossRatio(pAPPParam->mleftTurnLine, objectRatioRECT) >= crossRatio)
		{
#ifdef __DEBUG
			lBreakRuleHistoryLog << lCurrentPicName << "车" << lObject.uid << "违章左拐" << lObject.nLoopID << std::endl;
#endif
			lPoolData.mBreakRules[lObject.uid] |= VSD_BR_TURN_LEFT;
		}
		
		// 判断是否违章右转
		if( !(lVSDParam.loopLaneProperty[lObject.nLoopID] & VSD_LANE_TURN_RIGHT) && GetCrossRatio(pAPPParam->mRightTurnLine, objectRatioRECT) >= crossRatio)
		{
#ifdef __DEBUG
			lBreakRuleHistoryLog << lCurrentPicName << "车" << lObject.uid << "违章右转" << lObject.nLoopID << std::endl;
#endif
			lPoolData.mBreakRules[lObject.uid] |= VSD_BR_TURN_RIGHT;
		}
		
		// 判断是否违章直行
		if(!(lVSDParam.loopLaneProperty[lObject.nLoopID] & VSD_LANE_STRAIGHT) && GetCrossRatio(pAPPParam->mStraightLine, objectRatioRECT) >= crossRatio)
		{
#ifdef __DEBUG
			lBreakRuleHistoryLog << lCurrentPicName << "车" << lObject.uid << "违章直行" << lObject.nLoopID  << std::endl;
#endif
			lPoolData.mBreakRules[lObject.uid] |= VSD_BR_STRAIGHT_THROUGH;
		}
		// 判断是否逆行
		if(lObject.status & VSD_BR_REVERSE)
		{
#ifdef __DEBUG
			lBreakRuleHistoryLog << lCurrentPicName << "车" << lObject.uid << "逆行" << lObject.nLoopID << std::endl;
#endif
			lPoolData.mBreakRules[lObject.uid] |= VSD_BR_REVERSE;
		}
		// 判断是否违章停车
		if(lObject.status & VSD_BR_STOP)
		{
#ifdef __DEBUG
			lBreakRuleHistoryLog << lCurrentPicName << "车" << lObject.uid << "违章停车" << lObject.nLoopID << std::endl;
#endif
			lPoolData.mBreakRules[lObject.uid] |= VSD_BR_STOP;
		}
		(*pStatusMap)[lObject.uid] |= lPoolData.mBreakRules[lObject.uid];
	}
	//ResultList lResultList;
	int lResultCount = 0;
	// 对比新的PoolData和原来队尾的数据，如果发现原来队尾巴的物理在新的PoolData里消失，则说明有物体离开跟踪区域，这是我们根据其历史status来判断是该物体是否有违章，
	// 如果没有违章，则输出一张其停车线附近的图片
	if (!pPool->empty())
	{
		std::map<int, int> lastObject = pPool->back().mBreakRules;
		for(std::map<int, int>::iterator it = lastObject.begin(); it != lastObject.end(); ++it)
		{
			std::map<int, int> thisObject = lPoolData.mBreakRules;
			std::map<int, int>::iterator thisIt = thisObject.find(it->first);

			if(thisIt == thisObject.end())
			{
				VirtualLoopImagePool::iterator itImage = pVirtualLoopImage->find(it->first);
				StatusMap::iterator itStatus = pStatusMap->find(it->first);
				PlateMap::iterator itPlate = pPlateMap->find(it->first);
				if(itImage != pVirtualLoopImage->end())
				{
					if(itStatus != pStatusMap->end() && itStatus->second == VSD_BR_NONE)
					{
						LPRImage* lpImage = LPRCloneImage(itImage->second);
						EventAPPResult lpResult;
						lpResult.mBreakRule = VSD_BR_NONE;
						lpResult.mID = it->first;
						lpResult.mImage[0] = lpImage;
						lpResult.mNumOfImage = 1;
						if(itPlate != pPlateMap->end())
						{
							for(int i = 0; i < LPR_PLATE_STR_LEN; ++i)
							{
								lpResult.mPlate[i] = itPlate->second[i];
							}
						}
						else
						{
							lpResult.mPlate[0] = 0;
						}
						//lResultList.push_back(lpResult);
						opResult->mppAPPResult[lResultCount++] = lpResult;
						opResult->mNumOfResult = lResultCount;
						lNoBreakRuleLog << "车" << lpResult.mID << endl;
					}
					//LPRReleaseImage(itImage->second);
					//pVirtualLoopImage->erase(itImage);
				}
				if(itStatus != pStatusMap->end())
					pStatusMap->erase(itStatus);
				/*
				if(itPlate != pPlateMap->end())
				{
					delete[] itPlate->second;
					pPlateMap->erase(itPlate);
				}
				*/
			}
		}
	}

	// 把当前PoolData压入队尾
	pPool->push_back(lPoolData);
	if (pPool->size() > (*pPoolLength + 1))
	{
		// 释放掉队列头已不再需要录像的缓存的图片
		PoolData lCheckPoolData = pPool->front();
		LPRReleaseImage(lCheckPoolData.mpImage);
		pPool->pop_front();
		// 查看中间数据的违章情况
		int middle = (*pPoolLength + 1)/2;
		lCheckPoolData = pPool->at(middle);
		// 遍历当前poolData中的所有object的违章记录
		for(std::map<int, int>::iterator itObject = lCheckPoolData.mBreakRules.begin(); itObject != lCheckPoolData.mBreakRules.end(); ++itObject)
		{
			// itObjects所指向的车辆是否违章左转
			if(itObject->second & VSD_BR_TURN_LEFT)
			{
				if(!((*pRemoveStatusMap)[itObject->first] & VSD_BR_TURN_LEFT))
				{
					(*pRemoveStatusMap)[itObject->first] |= VSD_BR_TURN_LEFT;
					int lBeginIndex = middle - pAPPParam->mRecordParam.mBreakRuleAhead[0];
					lBeginIndex = lBeginIndex > 0 ? lBeginIndex : 0;
					int lEndIndex = middle + pAPPParam->mRecordParam.mBreakRuleBehind[0];
					lEndIndex = lEndIndex > 0 ? lEndIndex : pPool->size();
					int lSizeToCopy = lEndIndex - lBeginIndex;
					int lImageCount = lSizeToCopy;
					EventAPPResult lpAPPResult;
					lpAPPResult.mID = itObject->first;
					lpAPPResult.mBreakRule = VSD_BR_TURN_LEFT;
					PlateMap::iterator itPlate = pPlateMap->find(itObject->first);
					if(itPlate != pPlateMap->end())
					{
						for(int i = 0; i < LPR_PLATE_STR_LEN; ++i)
						{
							lpAPPResult.mPlate[i] = itPlate->second[i];
						}
					}
					else
						lpAPPResult.mPlate[0] = 0;
					LPRImage *lpImage = NULL;
					for (int j = 0; j < lSizeToCopy; ++j)
					{
						lpImage = LPRCloneImage(pPool->at(j + lBeginIndex).mpImage);
						lpAPPResult.mImage[j] = lpImage;
						//lpAPPResult.mNumOfImage = lSizeToCopy;
					}
					VirtualLoopImagePool::iterator itVirtualLoopImage = pVirtualLoopImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;

					}
					itVirtualLoopImage = pVirtualLoopLeaveImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopLeaveImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;
					}
					lpAPPResult.mNumOfImage = lImageCount;
					//lResultList.push_back(lpAPPResult);
					opResult->mppAPPResult[lResultCount++] = lpAPPResult;
					opResult->mNumOfResult = lResultCount;
					lBreakOutputLog << "车" <<lpAPPResult.mID << "违反左转" <<endl;
				}
			}
			if(itObject->second & VSD_BR_TURN_RIGHT)
			{
				if(!((*pRemoveStatusMap)[itObject->first] & VSD_BR_TURN_RIGHT))
				{
					(*pRemoveStatusMap)[itObject->first] |= VSD_BR_TURN_RIGHT;
					int lBeginIndex = middle - pAPPParam->mRecordParam.mBreakRuleAhead[1];
					lBeginIndex = lBeginIndex > 0 ? lBeginIndex : 0;
					int lEndIndex = middle + pAPPParam->mRecordParam.mBreakRuleBehind[1];
					lEndIndex = lEndIndex > 0 ? lEndIndex : pPool->size();
					int lSizeToCopy = lEndIndex - lBeginIndex;
					int lImageCount = lSizeToCopy;
					EventAPPResult lpAPPResult;
					lpAPPResult.mID = itObject->first;
					lpAPPResult.mBreakRule = VSD_BR_TURN_RIGHT;
					PlateMap::iterator itPlate = pPlateMap->find(itObject->first);
					if(itPlate != pPlateMap->end())
					{
						for(int i = 0; i < LPR_PLATE_STR_LEN; ++i)
						{
							lpAPPResult.mPlate[i] = itPlate->second[i];
						}
					}
					else
						lpAPPResult.mPlate[0] = 0;
					LPRImage *lpImage = NULL;
					for (int j = 0; j < lSizeToCopy; ++j)
					{
						lpImage = LPRCloneImage(pPool->at(j + lBeginIndex).mpImage);
						lpAPPResult.mImage[j] = lpImage;
						//lpAPPResult.mNumOfImage = lSizeToCopy;
					}
					VirtualLoopImagePool::iterator itVirtualLoopImage = pVirtualLoopImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;

					}
					itVirtualLoopImage = pVirtualLoopLeaveImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopLeaveImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;
					}
					lpAPPResult.mNumOfImage = lImageCount;
					//lResultList.push_back(lpAPPResult);
					opResult->mppAPPResult[lResultCount++] = lpAPPResult;
					opResult->mNumOfResult = lResultCount;
					lBreakOutputLog << "车" <<lpAPPResult.mID << "违反右转" <<endl;
				}
			}
			if(itObject->second & VSD_BR_STRAIGHT_THROUGH)
			{
				if(!((*pRemoveStatusMap)[itObject->first] & VSD_BR_STRAIGHT_THROUGH))
				{
					(*pRemoveStatusMap)[itObject->first] |= VSD_BR_STRAIGHT_THROUGH;
					int lBeginIndex = middle - pAPPParam->mRecordParam.mBreakRuleAhead[2];
					lBeginIndex = lBeginIndex > 0 ? lBeginIndex : 0;
					int lEndIndex = middle + pAPPParam->mRecordParam.mBreakRuleBehind[2];
					lEndIndex = lEndIndex > 0 ? lEndIndex : pPool->size();
					int lSizeToCopy = lEndIndex - lBeginIndex;
					int lImageCount = lSizeToCopy;
					EventAPPResult lpAPPResult;
					lpAPPResult.mID = itObject->first;
					lpAPPResult.mBreakRule = VSD_BR_STRAIGHT_THROUGH;
					PlateMap::iterator itPlate = pPlateMap->find(itObject->first);
					if(itPlate != pPlateMap->end())
					{
						for(int i = 0; i < LPR_PLATE_STR_LEN; ++i)
						{
							lpAPPResult.mPlate[i] = itPlate->second[i];
						}
					}
					else
						lpAPPResult.mPlate[0] = 0;
					LPRImage *lpImage = NULL;
					for (int j = 0; j < lSizeToCopy; ++j)
					{
						lpImage = LPRCloneImage(pPool->at(j + lBeginIndex).mpImage);
						lpAPPResult.mImage[j] = lpImage;
						//lpAPPResult.mNumOfImage = lSizeToCopy;
					}
					VirtualLoopImagePool::iterator itVirtualLoopImage = pVirtualLoopImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;

					}
					itVirtualLoopImage = pVirtualLoopLeaveImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopLeaveImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;
					}
					lpAPPResult.mNumOfImage = lImageCount;
					//lResultList.push_back(lpAPPResult);
					opResult->mppAPPResult[lResultCount++] = lpAPPResult;
					opResult->mNumOfResult = lResultCount;
					lBreakOutputLog << "车" <<lpAPPResult.mID << "违反直行" <<endl;
				}
			}
			if(itObject->second & VSD_BR_CROSS_LANE)
			{
				if(!((*pRemoveStatusMap)[itObject->first] & VSD_BR_CROSS_LANE))
				{
					(*pRemoveStatusMap)[itObject->first] |= VSD_BR_CROSS_LANE;
					int lBeginIndex = middle - pAPPParam->mRecordParam.mBreakRuleAhead[3];
					lBeginIndex = lBeginIndex > 0 ? lBeginIndex : 0;
					int lEndIndex = middle + pAPPParam->mRecordParam.mBreakRuleBehind[3];
					lEndIndex = lEndIndex > 0 ? lEndIndex : pPool->size();
					int lSizeToCopy = lEndIndex - lBeginIndex;
					int lImageCount = lSizeToCopy;
					EventAPPResult lpAPPResult; 
					lpAPPResult.mID = itObject->first; 
					lpAPPResult.mBreakRule = VSD_BR_CROSS_LANE;
					PlateMap::iterator itPlate = pPlateMap->find(itObject->first);
					if(itPlate != pPlateMap->end())
					{
						for(int i = 0; i < LPR_PLATE_STR_LEN; ++i)
						{
							lpAPPResult.mPlate[i] = itPlate->second[i];
						}
					}
					else
						lpAPPResult.mPlate[0] = 0;
					LPRImage *lpImage = NULL;
					for (int j = 0; j < lSizeToCopy; ++j)
					{
						lpImage = LPRCloneImage(pPool->at(j + lBeginIndex).mpImage);
						lpAPPResult.mImage[j] = lpImage;
						//lpAPPResult.mNumOfImage = lSizeToCopy;
					}
					VirtualLoopImagePool::iterator itVirtualLoopImage = pVirtualLoopImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;

					}
					itVirtualLoopImage = pVirtualLoopLeaveImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopLeaveImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;
					}
					lpAPPResult.mNumOfImage = lImageCount;
					opResult->mppAPPResult[lResultCount++] = lpAPPResult;
					opResult->mNumOfResult = lResultCount;
					//lResultList.push_back(lpAPPResult);
					lBreakOutputLog << "车" <<lpAPPResult.mID << "违反压线" <<endl;
				}
			}
			if(itObject->second & VSD_BR_REVERSE)
			{
				if(!((*pRemoveStatusMap)[itObject->first] & VSD_BR_REVERSE))
				{
					(*pRemoveStatusMap)[itObject->first] |= VSD_BR_REVERSE;
					int lBeginIndex = middle - pAPPParam->mRecordParam.mBreakRuleAhead[4];
					lBeginIndex = lBeginIndex > 0 ? lBeginIndex : 0;
					int lEndIndex = middle + pAPPParam->mRecordParam.mBreakRuleBehind[4];
					lEndIndex = lEndIndex > 0 ? lEndIndex : pPool->size();
					int lSizeToCopy = lEndIndex - lBeginIndex;
					int lImageCount = lSizeToCopy;
					EventAPPResult lpAPPResult;
					lpAPPResult.mID = itObject->first;
					lpAPPResult.mBreakRule = VSD_BR_REVERSE;
					PlateMap::iterator itPlate = pPlateMap->find(itObject->first);
					if(itPlate != pPlateMap->end())
					{
						for(int i = 0; i < LPR_PLATE_STR_LEN; ++i)
						{
							lpAPPResult.mPlate[i] = itPlate->second[i];
						}
					}
					else
						lpAPPResult.mPlate[0] = 0;
					LPRImage *lpImage = NULL;
					for (int j = 0; j < lSizeToCopy; ++j)
					{
						lpImage = LPRCloneImage(pPool->at(j + lBeginIndex).mpImage);
						lpAPPResult.mImage[j] = lpImage;
						//lpAPPResult.mNumOfImage = lSizeToCopy;
					}
					VirtualLoopImagePool::iterator itVirtualLoopImage = pVirtualLoopImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;

					}
					itVirtualLoopImage = pVirtualLoopLeaveImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopLeaveImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;
					}
					lpAPPResult.mNumOfImage = lImageCount;
					//lResultList.push_back(lpAPPResult);
					opResult->mppAPPResult[lResultCount++] = lpAPPResult;
					opResult->mNumOfResult = lResultCount;
					lBreakOutputLog << "车" <<lpAPPResult.mID << "违反逆行" <<endl;
				}
			}
			if(itObject->second & VSD_BR_RED_LIGHT)
			{
				if(!((*pRemoveStatusMap)[itObject->first] & VSD_BR_RED_LIGHT))
				{
					(*pRemoveStatusMap)[itObject->first] |= VSD_BR_RED_LIGHT;
					int lBeginIndex = middle - pAPPParam->mRecordParam.mBreakRuleAhead[5];
					lBeginIndex = lBeginIndex > 0 ? lBeginIndex : 0;
					int lEndIndex = middle + pAPPParam->mRecordParam.mBreakRuleBehind[5];
					lEndIndex = lEndIndex > 0 ? lEndIndex : pPool->size();
					int lSizeToCopy = lEndIndex - lBeginIndex;
					int lImageCount = lSizeToCopy;
					EventAPPResult lpAPPResult;
					lpAPPResult.mID = itObject->first;
					lpAPPResult.mBreakRule = VSD_BR_RED_LIGHT;
					PlateMap::iterator itPlate = pPlateMap->find(itObject->first);
					if(itPlate != pPlateMap->end())
					{
						for(int i = 0; i < LPR_PLATE_STR_LEN; ++i)
						{
							lpAPPResult.mPlate[i] = itPlate->second[i];
						}
					}
					else
						lpAPPResult.mPlate[0] = 0;
					LPRImage *lpImage = NULL;
					for (int j = 0; j < lSizeToCopy; ++j)
					{
						lpImage = LPRCloneImage(pPool->at(j + lBeginIndex).mpImage);
						lpAPPResult.mImage[j] = lpImage;
						//lpAPPResult.mNumOfImage = lSizeToCopy;
					}
					VirtualLoopImagePool::iterator itVirtualLoopImage = pVirtualLoopImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;

					}
					itVirtualLoopImage = pVirtualLoopLeaveImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopLeaveImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;
					}
					lpAPPResult.mNumOfImage = lImageCount;
					//lResultList.push_back(lpAPPResult);
					opResult->mppAPPResult[lResultCount++] = lpAPPResult;
					opResult->mNumOfResult = lResultCount;
					lBreakOutputLog << "车" <<lpAPPResult.mID << "违反红灯" <<endl;
				}
			}
			if(itObject->second & VSD_BR_STOP)
			{
				if(!((*pRemoveStatusMap)[itObject->first] & VSD_BR_STOP))
				{
					(*pRemoveStatusMap)[itObject->first] |= VSD_BR_STOP;
					int lBeginIndex = middle - pAPPParam->mRecordParam.mBreakRuleAhead[6];
					lBeginIndex = lBeginIndex > 0 ? lBeginIndex : 0;
					int lEndIndex = middle + pAPPParam->mRecordParam.mBreakRuleBehind[6];
					lEndIndex = lEndIndex > 0 ? lEndIndex : pPool->size();
					int lSizeToCopy = lEndIndex - lBeginIndex;
					int lImageCount = lSizeToCopy;
					EventAPPResult lpAPPResult;
					lpAPPResult.mID = itObject->first;
					lpAPPResult.mBreakRule = VSD_BR_STOP;
					PlateMap::iterator itPlate = pPlateMap->find(itObject->first);
					if(itPlate != pPlateMap->end())
					{
						for(int i = 0; i < LPR_PLATE_STR_LEN; ++i)
						{
							lpAPPResult.mPlate[i] = itPlate->second[i];
						}
					}
					else
						lpAPPResult.mPlate[0] = 0;
					LPRImage *lpImage = NULL;
					for (int j = 0; j < lSizeToCopy; ++j)
					{
						lpImage = LPRCloneImage(pPool->at(j + lBeginIndex).mpImage);
						lpAPPResult.mImage[j] = lpImage;
						//lpAPPResult.mNumOfImage = lSizeToCopy;
					}
					VirtualLoopImagePool::iterator itVirtualLoopImage = pVirtualLoopImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;

					}
					itVirtualLoopImage = pVirtualLoopLeaveImage->find(itObject->first);
					if(itVirtualLoopImage != pVirtualLoopLeaveImage->end())
					{
						lpImage = LPRCloneImage(itVirtualLoopImage->second);
						lpAPPResult.mImage[lImageCount++] = lpImage;
					}
					lpAPPResult.mNumOfImage = lImageCount;
					//lResultList.push_back(lpAPPResult);
					opResult->mppAPPResult[lResultCount++] = lpAPPResult;
					opResult->mNumOfResult = lResultCount;
					lBreakOutputLog << "车" <<lpAPPResult.mID << "违章停车" <<endl;
				}
			}
			// 清除pRemoveStatsu里不需要的记录
			int lShowUp = 0;
			for(int i = middle + 1; i < pPool->size(); ++i)
			{
				PoolData lTmpPoolData = pPool->at(i);
				if(lTmpPoolData.mBreakRules.find(itObject->first) != lTmpPoolData.mBreakRules.end())
				{
					lShowUp = 1;
					break;
				}
			}
			if(0 == lShowUp)
			{
				StatusMap::iterator itStatusMap = pRemoveStatusMap->find(itObject->first);
				if(itStatusMap != pRemoveStatusMap->end())
					pRemoveStatusMap->erase(itStatusMap);
				PlateMap::iterator itPlate = pPlateMap->find(itObject->first);
				if(itPlate != pPlateMap->end())
				{
					delete[] itPlate->second;
					pPlateMap->erase(itPlate);
				}

				VirtualLoopImagePool::iterator itVirtualLoopImage = pVirtualLoopImage->find(itObject->first);
				if (itVirtualLoopImage != pVirtualLoopImage->end())
				{
					LPRReleaseImage(itVirtualLoopImage->second);
					pVirtualLoopImage->erase(itVirtualLoopImage);
				}

				itVirtualLoopImage = pVirtualLoopLeaveImage->find(itObject->first);
				if(itVirtualLoopImage != pVirtualLoopLeaveImage->end())
				{
					LPRReleaseImage(itVirtualLoopImage->second);
					pVirtualLoopLeaveImage->erase(itVirtualLoopImage);
				}
			}
		}
	}

	/*
	if (lResultCount > 0)
	{
		char buf[256];
		char img[256];
		for(int i = 0; i < lResultCount;++i)
		{
			_snprintf(buf, 256, "test_%d_%d.avi", opResult->mppAPPResult[i].mID, opResult->mppAPPResult[i].mBreakRule);
			_snprintf(img, 256, "F:\\test_%d_%d.jpg", opResult->mppAPPResult[i].mID, opResult->mppAPPResult[i].mBreakRule);
			if (opResult->mppAPPResult[i].mBreakRule != VSD_BR_NONE)
			{
				pMediaConverter->imgs2media((opResult->mppAPPResult[i].mImage), opResult->mppAPPResult[i].mNumOfImage, buf);
			}
			else
			{
			//	pSubtitleOverlay->overlaySubtitle((opResult->mppAPPResult[i].mImage[0]), img, "123445km");
			}
		}
		for(ResultList::iterator it = lResultList.begin(); it != lResultList.end(); ++it)
		{
			_snprintf(buf, 256, "test_%d_%d.avi", it->mID, it->mBreakRule);
			_snprintf(img, 256, "F:\\test_%d_%d.jpg", it->mID, it->mBreakRule);
			if(it->mBreakRule != VSD_BR_NONE)
			{
				pMediaConverter->imgs2media((it->mImage), it->mNumOfImage, buf);
			}
			else
			{
				pSubtitleOverlay->overlaySubtitle((it->mImage[0]), img, "123445km");
			}
		}
	}

		*/


	return APP_OK;
}

EventAPP::~EventAPP()
{
	int** pValue = (int**)mObject;
	EventAPPParam* pAPPParam = (EventAPPParam*)(*pValue);
	ImagePool* pPool = (ImagePool*)(*(pValue + 1));
	VSDRatioLine* laneMark = (VSDRatioLine*)(*(pValue + 2));
	int* pPoolLength =(int*)(*(pValue + 3));
	VirtualLoopImagePool* pVirtualLoopImage = (VirtualLoopImagePool*)(*(pValue + 4));
	StatusMap* pStatusMap = (StatusMap*)(*(pValue + 5));
	StatusMap* pRemoveStatusMap = (StatusMap*)(*(pValue + 6));
	MediaConverter* pMediaConverter = (MediaConverter*)(*(pValue + 7));
	LPR* pLPR = (LPR*)(*(pValue + 9));
	PlateMap* pPlateMap = (PlateMap*)(*(pValue + 10));
	VirtualLoopImagePool* pVirtualLoopLeaveImage = (VirtualLoopImagePool*)(*(pValue + 11));
	int* pImageWidth = (int*)(*(pValue + 12));
	int* pImageHeight = (int*)(*(pValue + 13));
	ImageSynthesis* pImageSynthesis = (ImageSynthesis*)(*(pValue + 14));
	

	delete pAPPParam;

	while(!pPool->empty())
	{
		LPRReleaseImage(pPool->front().mpImage);
		pPool->pop_front();
	}
	delete pPool;

	delete[] laneMark;

	delete pPoolLength;

	for(VirtualLoopImagePool::iterator it = pVirtualLoopImage->begin(); it != pVirtualLoopImage->end(); ++it)
		LPRReleaseImage(it->second);
	delete pVirtualLoopImage;

	for(VirtualLoopImagePool::iterator it = pVirtualLoopLeaveImage->begin(); it != pVirtualLoopLeaveImage->end(); ++it)
		LPRReleaseImage(it->second);
	delete pVirtualLoopLeaveImage;

	delete pStatusMap;

	delete pRemoveStatusMap;

	delete pMediaConverter;

	pLPR->Fini();
	delete pLPR;

	for (PlateMap::iterator it = pPlateMap->begin(); it != pPlateMap->end(); ++it)
	{
		delete[] it->second;
	}
	delete pPlateMap;

	delete pImageHeight;
	delete pImageWidth;
	delete pImageSynthesis;
}

APPRESULT EventAPP::AddSubTitle(LPRImage* ipImage, const wchar_t* ipString, LPRImage** oppImage)
{
	if (ipImage == NULL || ipString == NULL || oppImage == NULL)
	{
#ifdef __DEBUG
		TRACE("EventAPP::AddSubTitle input null pointer");
#endif
		return APP_INPUT_NULL_POINTER;
	}
	int** pValue = (int**)mObject;
	EventAPPParam* pEventParam = (EventAPPParam*)(*pValue);
	SubtitleOverlay* pSubtitleOverlay;
	*oppImage = pSubtitleOverlay->overlaySubtitle(ipImage, ipString, pEventParam->mFont);
	if(*oppImage == NULL)
		return APP_FAIL;
	return APP_OK;
}

APPRESULT EventAPP::SynthesisImages(LPRImage** ipImage, int iNumOfImages, LPRImage** oppImage)
{
	if (ipImage == NULL || oppImage == NULL)
	{
#ifdef __DEBUG
		TRACE("EventAPP::SynthesisImages input null pointer");
#endif
		return APP_INPUT_NULL_POINTER;
	}

	int** pValue = (int**)mObject;
	EventAPPParam* pEventParam = (EventAPPParam*)(*pValue);
	ImageSynthesis* pImageSynthesis = (ImageSynthesis*)(*(pValue + 14));
	*oppImage = pImageSynthesis->synthesis(ipImage, pEventParam->mImageSynthesis);
	if (*oppImage == NULL)
		return APP_FAIL;
	return APP_OK;
}

APPRESULT EventAPP::Convert2Media(LPRImage** ipImage, int iNumOfImages, EventMedia& orMedia)
{
	if (ipImage == NULL)
	{
#ifdef __DEBUG 
		TRACE("EventAPP::Convert2Media input null pointer");
#endif
	}
	int** pValue = (int**)mObject;
	MediaConverter* pMediaConverter = (MediaConverter*)(*(pValue + 7));
	bool ret = pMediaConverter->imgs2media(ipImage, iNumOfImages, orMedia);
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

	for(int i = 0; i < ipAPPResult->mNumOfImage; ++i)
		LPRReleaseImage(ipAPPResult->mImage[i]);
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
	std::string::size_type nPosition;
	std::string keyString;
	std:;string valueString;
	while(::getline(ifs,lineString))
	{
		if(lineString.empty())
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
	// 初始化EventAPPParam.mVSDEventParam
	KeyValue::iterator it = keyValue.find("VSDEvent_Param");
	if (it == keyValue.end())
	{
#ifdef __DEBUG
			TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
			return APP_CONFIG_PARAM_FAULT;
	}
	else
	{
		VSDEventParam lVSDEventParam;
		lResult = VSDEvent_LoadParam(it->second.c_str(), &lVSDEventParam);
		if (lResult != LPR_OK)
		{
#ifdef __DEBUG
			TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
			return APP_VSD_PARAM_FAULT;
		}
		ipEventParam->mVSDParam = lVSDEventParam;
	}

	it = keyValue.find("Charactors");
	if(it == keyValue.end())
	{
#ifdef __DEBUG
		TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
		return APP_CONFIG_PARAM_FAULT;
	}
	else
	{
		if(it->second.size() > MAX_CHARACTOR_NUM)
		{
#ifdef __DEBUG
			TRACE("EventAPP_LoadParam 输入参数文件格式有误");
#endif
			return APP_CONFIG_PARAM_FAULT;
		}
		str2wstr(ipEventParam->mFont.mCharactors, it->second.c_str(), it->second.size());
		//ipEventParam->mFont.mCharactors[it->second.size() + 1] = L'\0';
	}
	
	// 初始化左转线p1点的x坐标
	APPRESULT lAPPResult = CheckAndSetValue(keyValue, "TurnLeft.pt1.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mleftTurnLine.pt1.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//初始化左转线p1点的y坐标
	lAPPResult = CheckAndSetValue(keyValue, "TurnLeft.pt1.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mleftTurnLine.pt1.y);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "TurnLeft.pt2.x", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mleftTurnLine.pt2.x);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	//
	lAPPResult = CheckAndSetValue(keyValue, "TurnLeft.pt2.y", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mleftTurnLine.pt2.y);
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

	// 初始化压线阈值
	lAPPResult = CheckAndSetValue(keyValue, "CrossRatio", 0.0, 1.0, ipEventParam->mRatioToCrossLine);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	// 初始化各种rule的录制帧范围
	lAPPResult = CheckAndSetValue(keyValue, "RecordTurnLeftFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[0]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordTurnLeftFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[0]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordTurnRightFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[1]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordTurnRightFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[1]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordStraightFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[2]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordStraightFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[2]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordCrossLineFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[3]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordCrossLineFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[3]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordReverseFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[4]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordReverseFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[4]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordRedLightFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[5]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordRedLightFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[5]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordStopFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[6]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordStopFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[6]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordHighSpeedFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[7]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordHighSpeedFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[7]);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	//
	lAPPResult = CheckAndSetValue(keyValue, "RecordLowSpeedFramAhead", 0, MAX_FRAM_AHEAD, ipEventParam->mRecordParam.mBreakRuleAhead[8]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	lAPPResult = CheckAndSetValue(keyValue, "RecordLowSpeedFramBehind", 0, MAX_FRAM_BHEIND, ipEventParam->mRecordParam.mBreakRuleBehind[8]);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	lAPPResult = CheckAndSetValue(keyValue, "ViedoFormat", 0, 2, ipEventParam->mRecordParam.mViedoFormat);
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	lAPPResult = CheckAndSetValue(keyValue, "ViedoBitFrequent", 0, 1000000000, ipEventParam->mRecordParam.mBitFrequent);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "FontSize", 0, 10000, ipEventParam->mFont.mFontSize);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "FontFamily", 0, 2, ipEventParam->mFont.mFontFamily);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	int RGB_R;
	lAPPResult = CheckAndSetValue(keyValue, "FontRGB_R", 0, 256, RGB_R); 
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	int RGB_G;
	lAPPResult = CheckAndSetValue(keyValue, "FontRGB_G", 0, 256, RGB_G); 
	if (lAPPResult != APP_OK)
		return lAPPResult;
	
	int RGB_B;
	lAPPResult = CheckAndSetValue(keyValue, "FontRGB_B", 0, 256, RGB_B); 
	if (lAPPResult != APP_OK)
		return lAPPResult;
	ipEventParam->mFont.mFontColor = RGB(RGB_R, RGB_G, RGB_B);


	lAPPResult = CheckAndSetValue(keyValue, "FontOrientation", 0, 2, ipEventParam->mFont.mFontOrientation);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "FontX", 0, ipEventParam->mVSDParam.nWidthBase, ipEventParam->mFont.mFontX);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "FontY", 0, ipEventParam->mVSDParam.nHeightBase, ipEventParam->mFont.mFontY);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "ImageSynthesisNum", 0, 10, ipEventParam->mImageSynthesis.mNumberofImage);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "ImageSynthesisOrientation", 0, 3, ipEventParam->mImageSynthesis.mPicOrientation);
	if (lAPPResult != APP_OK)
		return lAPPResult;

	lAPPResult = CheckAndSetValue(keyValue, "ImageSynthesisZoon", 0.0, 1.0, ipEventParam->mImageSynthesis.mZoonRatio);
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
	VSDRatioLine line = {{2,1},{2,3}};
	VSDRatioRECT rect = {1,3,3,1};

	double length = GetCrossRatio(line, rect);
	std::cout << length << std::endl;

	VSDEventParam lParam;	
	VSDEvent_LoadParam("F:\\EventData\\images\\120327\\VSDEvent.ini", &lParam);
	VSDEvent lEvent;
	LPRRESULT l = lEvent.Init(lParam);
	EventAPP lEventApp;
	EventAPPParam lAPPParam;
	//////////////////////////////////////////////////////////////////////////
	EventAPP_LoadParam("F:\\EvenAPP_Param.ini", &lAPPParam);
	lEventApp.Init(lAPPParam);
	std::wstring fileDir(L"F:\\EventData\\images\\120327");
	std::vector<wstring> lFiles;
	EmumAllJPGFileInFolder(fileDir, lFiles);
	wstring fileName;
	time_t tBegin;
	tBegin = time(NULL);
	//////////////////////////////////////////////////////////////////////////
	for(vector<wstring>::iterator it = lFiles.begin(); it != lFiles.end(); ++it)
	{
		fileName = *it;
		lCurrentPicName = *it;
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
		/*
		LPRImage* lpImage = NULL;
		LPRRESULT lResult = LPRDecodeImage(&lpImage, (const unsigned char*)pJpgBuf, nJpgLen, LPR_ENCODE_FORMAT_JPG, 0);
		*/
		LPRImage imgJPG;
		imgJPG.nColorMode = CS_JPEG;
		imgJPG.pData = (unsigned char *)pJpgBuf;
		imgJPG.imageSize = nJpgLen;
		imgJPG.info.nCamID = 0;
		//imgJPG.timeStamp = sysTime;
		//LPRImage* lEncodeImage = LPRCreateImage(lpImage->width, lpImage->height, lpImage->depth, lpImage->nChannels);
	/*	char* pData = new char[5*nJpgLen];
		int length = 5*nJpgLen;
		lResult = LPREncodeImage(lpImage, (unsigned char*)pData, &length, LPR_ENCODE_FORMAT_JPG, 80);*/
		EventMultiAPPResult lAPPResult;
		VSDObjectMulti lObjectMulti;
		LPRRESULT lResult = lEvent.ProcessFrame(&imgJPG, &lObjectMulti);
		int lLights[MAX_VIRTUAL_LOOPS] = {0, 0, 0, 0};
		lEventApp.ProcessFram(&imgJPG,&lObjectMulti, lLights, &lAPPResult);
		delete[] pJpgBuf;
		FreeMultiAPPResult(&lAPPResult);
	}
	time_t tEnd = time(NULL);
	cout << "运行" << (tEnd - tBegin) / 60 << "分" << (tEnd - tBegin)%60 << "秒" << endl;

	int i;
	std::cin >> i;
	return 0;
}
