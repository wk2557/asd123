#ifndef __EVENT_APP_STRUCTURE_H_
#define __EVENT_APP_STRUCTURE_H_

#include "VSDEventStructure.h"
#include "LPRStructure.h"
#include "EventAPPConstant.h"
#include <inttypes.h>
#include <map>

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

struct EventSubtitleImages
{
	std::map<wchar_t, LPRImage*> *mWCharImageMapArray;	// ���ַ������Ӧͼ��map�����飬����ά�ȵ�������������
	int *mFontFamilys;								// ���е��������
	int mFontFamilysCount;							// ����������
};

// ��¼����Υ��¼��Χ
struct EventRecordParam
{
	int mBreakRuleAhead[RULE_TYPES];   // VSD_BR_TURN_LEFT �� VSD_BR_LOW_SPEED 9��Υ�£������е�ÿһ��ֱ��¼����Υ����Ϊ��Ӧ��ǰ����֡����¼��
	int mBreakRuleBehind[RULE_TYPES];  // VSD_BR_TURN_LEFT �� VSD_BR_LOW_SPEED 9��Υ�£������е�ÿһ��ֱ��¼����Υ����Ϊ��Ӧ¼��Υ����Ϊ��Ķ���֡ 
	//std::map<int, int> mBreakRuleAhead;
	//std::map<int, int> mBreakRuleBehind;
	int mFrameFrequent;
	int mVideoFormat;				   // ��Ƶ��ʽ��ȡֵ��ΧΪΪEventAPPVideoFormat
	int mBitFrequent;				   // ��Ƶ������
};

// ���ص���Ƶ�ṹ
struct EventMedia
{
	uint8_t *mBufferPtr;			  // ָ����Ƶ��buffer
	int mBufferSize;				  // ��Ƶbuffer�Ĵ�С
};

// �ϳ�ͼƬ����Ļ��������Ϣ
struct EventFont 
{
	int mFontSize;					   // �����С
	int mFontFamily;				   // �����ʽ
	int mFontRGB_R;					   // ������ɫ
	int mFontRGB_G;					   // ������ɫ
	int mFontRGB_B;					   // ������ɫ
	int mFontOrientation;			   // �����ŷų���ȡֵΪ��ΧΪEventAPPFontOrientation
	int mFontX;						   // ������ʼλ�õ�X����
	int mFontY;                        // ������ʼλ�õ�Y����
};

struct EventSubtitleOverlay
{
	wchar_t **mSubtitles;				// �ַ������飬��ʾ�����Ļ
	int mSubtitleSize;					// ��Ļ����
	EventFont *mFonts;					// ÿ����Ļ��Ӧ��������Ϣ
	int mFontSize;						// ����Ӧ������Ļ�������
};

// N��ͼƬ�ϳ�һ��
struct EventImageSynthesis
{
	int mNumberofImage;				   // �ϳ�ͼƬ����Ŀ
	int mPicOrientation;               // ͼƬ�İڷţ�ȡֵ��ΧΪEventAPPPicOrientation
	int mZoomRatio;                 // ԭʼͼƬ�����ű���
};

// EventAPP����
struct EventAPPParam
{
	VSDEventParam mVSDParam;			// VSDEvent�Ĳ���
	int mRuleSwitch[RULE_TYPES];		// �Ƿ����Ӧ��Υ���������ץ�ĺ�¼��
	//std::map<int, int> mRuleSwitch;
	VSDRatioLine mStopLine;				// ͣ����λ��
	VSDRatioLine mCentreLine;				// ������λ�ã������������嵽���м�ʱ��ͼƬ
	VSDRatioRECT mStopForbidRect;		// ��ֹͣ������
	VSDRatioLine mLeftTurnLine;			// ��ת�߱�־�ߣ���ֹ���ʱ���ΪΥ��
	VSDRatioLine mRightTurnLine;		// ��ת�߱�־�ߣ���ֹ�ҹ�ʱ���ΪΥ��
	VSDRatioLine mStraightLine;			// ֱ�б�־�ߣ�Խ�����ߺ��ֹ����ת
	EventRecordParam mRecordParam;		// ����Υ�������Υ��¼�����
	int mRatioToCrossLine;			// ����ѹ�߱�����ֵ�����������ֵ�ж�ѹ����ʻ
	EventFont mFont;					// �ϳ�ͼƬ����ĸ��Ϣ
	EventImageSynthesis mImageSynthesis;// ��N��ͼƬ�ϳ�һ��ͼƬ�Ĳ���
	int mRulePriority[RULE_TYPES];
	int mReverseRatio;
	int mStopRatio;
	int mPlateCaptureSwitch;
	//std::map<int, int> mRulePriority;
};

// EventAPP����ͼƬ��������
struct EventAPPResult
{
	int mID;															// �����ID
	int mNumOfVideoImage;												// ���ںϳ�¼���ͼ��ĸ���
	int mNumOfSynthesisImage;											// ���ںϳɴ�ͼƬ��ԭʼͼƬ����
	wchar_t mPlate[LPR_PLATE_STR_LEN];									// ����ĳ��ƺ�
	int mBreakRule;														// ID����Υ���Ĺ���
	int mLoopID;														// ID����ĳ���
	LPRImage* mVideoImage[MAX_FRAME_AHEAD + MAX_FRAME_BEHIND];			// �����ͼƬ���У����mBreakRule��VSD_BR_NONE����ômNumberOfImage��ֵһ����1��������λ��ͣ���߸�������Ƭ
																		// ���mBreakRule����VSD_BR_NONE����ôǰMAX_FRAM_AHEAD + MAX_FRAM_BEHIND�������ϳ�¼�񣬺������ŷֱ��ǽӴ�ͣ���ߣ��뿪ͣ���ߣ�Υ��ʱ������ͼƬ
	LPRImage* mSynthesisImage[MAX_SYNTHESIS_NUM];						
	LPRImage* mPlateImage;
	VSDRect mRect;														// ���ڱ���������mSynthesisImage�ĵ�һ��ͼƬ�е�λ�ã�����ȡ�Ŵ�ͼƬ��λ���� 
};


// ���APPResult
struct EventMultiAPPResult
{
	int mNumOfResult;													// APPResult�ĸ���
	EventAPPResult mppAPPResult[MAX_OBJECTS];							// ָ��APPResultָ������飬��Ϊÿ��ͼƬ�������MAX_OBJECTS�����壬ÿ������������һ��Υ�����
};


// ��EventAPPParam��ֵ֮ǰ�����ȵ���InitEventAPPParam����
EVENTAPP_API void __stdcall InitEventAPPParam(EventAPPParam* opEventAPPParam);
EVENTAPP_API void __stdcall FreeMultiAPPResult(EventMultiAPPResult* ipMultiAPPResult);
EVENTAPP_API void __stdcall FreeAPPResult(EventAPPResult* ipAPPResult);
EVENTAPP_API void __stdcall FreeEventMedia(EventMedia *pMedia);
EVENTAPP_API APPRESULT __stdcall EventAPP_LoadParam(const char* ipFileName, EventAPPParam* ipEventParam);


#endif
