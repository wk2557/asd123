#ifndef __LPR_DEFINES_H__
#define __LPR_DEFINES_H__

#define LPR_REGION_TYPE					0

#define ENABLE_SEPARATOR_POS_FIELD		1
#define REC_VEHICLE_BRAND				1

// 基本的常量

// 车牌字符串长度
#define LPR_PLATE_STR_LEN		11

// 车牌字符数目
#define LPR_NUM_CHAR			7
#define LPR_MAX_NUM_CHAR		7

// 最大的输出结果数目，预留
#define LPR_MAX_OUTPUT_NUM		32
#define LPR_MAX_FACES			4

#ifdef __BORLANDC__
#pragma option push -b  // treat enum as integer
#endif

// 每一位字符的识别类型
enum LPRRecType
{
	LR_INVALID		= -1,
	LR_DigitAlpha	= 0,	// 数字及字母
	LR_Digit		= 1,	// 数字
	LR_Alpha		= 2,	// 字母
	// 汉字识别的地区类型	  
	LR_China		= 20,	// 全国通用
	LR_HeiLongJiang	= 21,	// 黑龙江
	LR_ShangHai		= 22,	// 上海
	LR_LiaoNing		= 23,	// 辽宁
	LR_ShanDong		= 24,	// 山东
	LR_GuangDong	= 25,	// 广东
	LR_GuangXi		= 26,	// 广西
	LR_TianJin		= 27,	// 天津
	LR_BeiJing		= 28,	// 北京
	LR_JiLin		= 29,	// 吉林
	LR_HeNan		= 30,	// 河南
	LR_HuBei		= 31,	// 湖北
	LR_HuNan		= 32,	// 湖南
	LR_JiangXi		= 33,	// 江西
	LR_GanSu		= 34,	// 甘肃
	LR_ZheJiang		= 35,	// 浙江
	LR_JiangSu		= 36,	// 江苏
	LR_HuaDong		= 37,	// 华东
	LR_XiNan		= 38,	// 西南
	LR_XiBei		= 39,	// 西北
	LR_DongBei		= 40,	// 东北
	LR_HuaBei		= 41,	// 华北
	LR_HuaNan		= 42,	// 华南
	LR_HuaZhong		= 43	// 华中
};							

// 车牌颜色
enum LPRPlateColor
{
	PC_UNKNOWN	= 0,	// 未知
	PC_BLUE		= 1,	// 蓝底白字
	PC_YELLOW	= 2,	// 黄底黑字
	PC_WHITE	= 3,	// 白底黑字
	PC_BLACK	= 4,	// 黑底白字	
	PC_GREEN	= 5		// 绿底白字
};

// 车牌类型
enum LPRPlateType
{
	PT_UNKOWN		= 0,	// 未知
	PT_CAR_NORMAL	= 1,	// 普通小车，私家车，蓝色，黑色
	PT_BIG_NORMAL	= 2,	// 普通大车，卡车，黄色
	PT_POLICE		= 4,    // 警车（分割符在第一位和第二位字符之间），白色
	PT_WJ			= 8,	// 武警车辆
	PT_HK			= 16,	// 港澳车辆
	PT_DOUBLELINE	= 32,	// 双层黄牌
	PT_MILITARY		= 64,	// 军牌
	PT_AMBASSADOR	= 128,	// 使馆车辆
	PT_POLICE_TYPE2	= 512,  // 第二类警车（分割符在第二位和第三位字符之间），白色
	PT_XUE			= 1024,	// “学”车牌
	PT_MOTOR		= 2048,	// 摩托车
	PT_LIAOTEMP		= 4096, // 临字车牌
	PT_GREEN		= 8192,	// 双层绿牌，农用车
	PT_GUA			= 16384	// “挂”车牌
};

enum LPRVersion
{
	LV_VIDEO	= 0,		/// 视频版本，只适用于LPREngine调用
	LV_FAST_VID = 1,		/// 预留
	LV_ACC_VIDEO = 2,		/// 比普通视频版本可能更慢，但是更精确
	LV_PICTURE	= 8,		/// 单张版本
	LV_FAST_PIC = 9,		/// 快速单张版本
	LV_FAST_PIC2= 10,		/// 更快速单张版本
	LV_FAST_PIC3= 11,		/// 非常快速单张版本
	LV_FAST_PIC4= 12,		/// 超快速单张版本
};

enum LPRVehicleBrand
{
	BRAND_UNKOWN	= -1,	/// 未知
	BRAND_CSVW		= 0,	/// 大众
	BRAND_AUDI		= 1,	/// 奥迪
	BRAND_HONDA		= 2,	/// 本田
	BRAND_BUICK		= 3,	/// 别克
	BRAND_TOYOTA	= 4,	/// 丰田
	BRAND_CITROEN	= 5,	/// 雪铁龙/富康
	BRAND_MAZDA		= 6,	/// 马自达
	BRAND_HYUNDAI	= 7,	/// 现代
};

// 输入图像的颜色空间
enum LPRColorSpace
{
	CS_RGB24	= 0,
	CS_RGB565	= 1,
	CS_RGB555	= 2,
	CS_RGB32	= 3,
	CS_JPEG		= 4,	/// 输入jpeg码流
	CS_YUYV		= 5,
	CS_UYVY		= 6,
	CS_GRAY		= 4,	/// 以下暂不支持
	CS_YV12		= 8,
	CS_YV16		= 16,
	CS_YUY2		= 32,
};


#if defined( LPR_LINUX )

typedef long				BOOL;
typedef unsigned short     	WORD;
typedef int					INT;
typedef long				LONG;
typedef unsigned long		DWORD;
typedef unsigned long		ULONG;
#ifndef USE_ICC
typedef long				__int64;
#endif
typedef char				TCHAR;
typedef char				_TCHAR;
typedef unsigned char		UCHAR;


#define TRUE	1
#define FALSE   0

#ifndef NULL
#define NULL 0
#endif /* NULL */

typedef void *HANDLE;
typedef void *HPEN;
typedef void *HFONT;
typedef void *HWND;
typedef DWORD COLORREF;
typedef char  TCHAR;

#ifdef RTM
#undef assert
#define assert(exp)		(true)
#endif

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, *LPPOINT;

typedef struct  tagSIZE
{
    LONG cx;
    LONG cy;
} SIZE, *PSIZE, *LPSIZE;

typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT, *LPRECT;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#define  __stdcall
#define _MAX_PATH   260
#define _snprintf	snprintf
#define _stricmp	strcasecmp
#define stricmp	strcasecmp
#define _cprintf	printf
#define wcscpy_s(a,b,c)  wcscpy(a,c)
#define _countof(array)  (sizeof(array)/sizeof(array[0]))

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))


#elif defined(WIN32)
#include <windows.h>
#endif

#ifdef __BORLANDC__
#pragma option pop
#endif

#endif //__LPR_DEFINES_H__
