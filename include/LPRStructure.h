#ifndef __LPR_STRUCTURE_H__
#define __LPR_STRUCTURE_H__

#include "LPRResult.h"
#include "LPRDefines.h"

struct LPRLine
{
	POINT	from;
	POINT	to;
};

struct LPRRatioRect
{
	int		leftRatio;
	int		rightRatio;
	int		topRatio;
	int		bottomRatio;
};

struct LPRRatioPoint
{
	int		xRatio;
	int		yRatio;
};

struct LPRRatioLine
{
	LPRRatioPoint	from;
	LPRRatioPoint	to;
};

/// 暂时没有开放使用
struct LPRImageInfo
{
	int nCamID;				/// 摄像头ID
	int nRoadID;			/// 路段ID
	int nDirID;				/// 方向ID
	int pReserved[61];
};

/// 暂时没有开放使用
struct LPROutputInfo
{
	int nMotionDir;			/// 运动方向
	int pReserved[63];		/// 保留字段
};

struct LPRImage
{
	int  nSize;         // LPRImage 结构大小
	int  nChannels;     // 通道数目，1、2、3、4
	int  depth;         // 颜色深度，8、16
	int  nColorMode;	// 取值为 LPRColorSpace 中某一个
	int  dataOrder;     // 0
	int  origin;        // 0
	int  width;         // 宽度
	int  height;        // 高度
	int  imageSize;     // 数据buffer大小
	unsigned char *pData;// 数据buffer指针
	int  step;			// 每一行的长度（不一定和宽度相等）
	SYSTEMTIME	timeStamp;  // 时间戳
	LPRImageInfo info;
};

/// 车牌检测的输出信息
struct LPROutput
{
	LPRPlateColor	nColor;						// 车牌颜色
	LPRPlateType	nType;						// 车牌类型
	RECT			rectPlate;					// 车牌位置矩形
	RECT			charRect[LPR_MAX_NUM_CHAR];	// 车牌各位字符的位置矩形，在全图中的坐标
	wchar_t			wszRec[LPR_PLATE_STR_LEN];	// 车牌号
	int				pRec[LPR_MAX_NUM_CHAR];		// 保留
	float			pConfid[LPR_MAX_NUM_CHAR];	// 车牌各位字符的置信度
	float			fltConfid;					// 整个车牌的置信度
	float			currConfid;					// 保留
	int				nNumChar;					// 车牌中字符数目
	bool			bNewCar;					// 车辆是否第一次在视频中出现
#if ENABLE_SEPARATOR_POS_FIELD
	int				nSeparatorPos;				// 车牌文字间隔的位置
#endif
#if REC_VEHICLE_BRAND
	int				nBrand;						// 车标识别结果，启用车标识别需要在LPR.ini中设置
	RECT			rectBrand;					// RecogBrand参数为1，并且需要加密狗支持车标识别功能。
												// rectBrand为车标的矩形框
#endif
	int				numOfFaces;					// 人脸检测结果，faces字段存放检测到人脸矩形区域，
	RECT			faces[LPR_MAX_FACES];		// numOfFaces表示faces数组中人脸检测结果的个数。
												// 如果检测出多个人脸结果，那么第一个结果为驾驶员的脸
												// 启用车人脸检测需要在LPR.ini中设置DetectFace参数为1，
												// 并且需要加密狗支持人脸检测功能。
	RECT			rectWindow;					// 车辆前车窗的大致位置

	LPROutputInfo	info;						// 其他输出信息
};

/// LPR 可以返回多个结果
/// 多输出的接口
struct LPROutputMulti
{
	LPROutput	m_outputs[LPR_MAX_OUTPUT_NUM];	// 多个输出结果。
	int			m_nNumOutputs;					// 输出结果的数目。
	RECT  		m_motionRegion;					// 运动区域
};


// 车牌模型
struct LPRPlateModel
{
	int		plateType;
	int		plateCharType[LPR_MAX_NUM_CHAR];	// CharType，每一位的类型
};

#define		LPR_MAX_PLATE_NUM		12

// LPR参数
struct LPRParam
{
	int				size;
	int				nMaxPlate;
	int				nMinPlateWidth;
	int				nMaxPlateWidth;
	LPRPlateModel	plateModel;
	int				pReserved[64];	/// 保留字段，便于将来版本无缝升级
};

// LPREngine参数
struct LPREngineParam
{
	int				size;			// 结构大小
	LPRParam		lprParam;		// LPR参数
	LPRRatioRect	recogRegion;	// 矩形识别区域

	LPRRatioLine	line1;			// 双线识别区域
	LPRRatioLine	line2;

	POINT			tlvRegion[4];	

	int				maxSkip;		// 理解为前后车辆之间的最小间隔，
									// 识别一辆车之后，如果maxSkip帧没有结果，
									// 就输出这一辆车的结果
									// 这个参数的目的是：容忍视频识别时间或
									// 检测不到车牌

	float			minValid;		// 投票结果满足这个置信度之后可以输出
	float			minConfid;		// 进入投票的单个识别结果的最低置信度

	int				framePercent;			// 识别比率
	bool			bAdaptiveFP;			// 保留
	int 			motionDetectionMode;	// 运动检测模式，0-不做运动检测，1-在运动检测基础上才做识别识别
	bool			bCapture;				// 是否打开车辆抓拍功能
	int				minCaptureDiff;			// 
	LPRRatioRect	preferCapRegion;		// 优先抓图区域
	LPRRatioRect	rectEntrance;			// 跟踪模式为3时，进入区域的设置
	int				pReserved[512];	/// 保留字段，便于将来版本无缝升级
};

// LPREngine输入
struct LPREngineInput
{
	int				size;		// 结构大小
	LPRImage*		pInput;		// 待识别图像的指针
	bool			bForce;		// 保留，置为false
	bool			bRedLight;
	long			context;	// 自定义字段
};

class LPREngine;

// 抓图
struct LPRCarItem
{
	LPRImage*		pCapture;	// 车辆抓拍图片
	LPROutput		output;		// 车辆信息
};

// TLV for "Traffic Light Violation"
#define LPR_MAX_TLV_NUM			16
struct LPRTLVItem
{
	LPRCarItem		captures[3];
};

// LPREngine输出结果
struct LPREngineOutput
{
	int				size;							// 结构大小
	LPREngine*		pEngine;						// LPREngine的指针
	LPRImage*		pInput;							// 完成识别的图片
	bool			bForce;							// 保留，为false
	long			context;						// 自定义字段
	LPRRESULT		lr;								// 识别的错误值
	LPROutputMulti	outputs;						// 识别结果
	int				numOfCarItems;					// 车辆结果数目
	LPRCarItem		carItems[LPR_MAX_OUTPUT_NUM];	// 车辆结果数组
	int				numOfTlvItems;
	LPRTLVItem		tlvItems[LPR_MAX_TLV_NUM];
};	


struct LPRUserPasswd
{
	unsigned long	data1;
	unsigned long	data2;
};
struct LPRUserData
{
	unsigned long	data1;
	unsigned long	data2;
	unsigned long	data3;
	unsigned long	data4;
};

#endif //__LPR_STRUCTURE_H__

