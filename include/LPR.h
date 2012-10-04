#ifndef __LPR_H__
#define __LPR_H__

#include "LPRStructure.h"
#include "LPRResult.h"

#if defined(WIN32)
	#ifdef LPR_EXPORTS
		#define LPR_API __declspec(dllexport)
	#else
		#define LPR_API __declspec(dllimport)
	#endif
#elif defined(LPR_LINUX)
	#ifdef LPR_EXPORTS
		#define LPR_API __attribute__ ((visibility("default")))
	#else
		#define LPR_API
	#endif
#else
	#error "No platform specified!"
#endif



#ifdef __cplusplus
extern "C" {
#endif


#define LPR_MAX_REGIONS_PER_FRAME		32

/// 输入图像的长宽比例格式
/// 不指定，内部自动判断
#define LPR_IMAGE_TYPE_AUTO				0
/// 输入图像为帧模式，图像长宽比和实际环境一致
#define LPR_IMAGE_TYPE_FRAME			1
/// 输入图像为场模式，高度比实际环境减半
#define LPR_IMAGE_TYPE_FIELD			2

// 局部参数
// 用于描述图像中的一个待识别的矩形区域
struct LPRParamLocal
{
	RECT		m_rectRegion;		// 用一个矩形框表示的识别区域，描述图像中的需要识别识别区域，该区域外的部分将不做识别，
									// 矩形框的上下左右边界值都以像素为单位。
	int			m_nMinPlateWidth;	// 最小车牌宽度，小于该宽度的车牌将不做识别。
	int			m_nMaxPlateWidth;	// 最大车牌宽度，大于该宽度的车牌将不做识别。

	float		m_fltReserved0;		// 保留参数，设为0。
	float		m_fltReserved1;		// 保留参数，设为0。
	float		m_fltReserved2;		// 保留参数，设为0。
	float		m_fltReserved3;		// 保留参数，设为0。
};

struct LPRParamMulti
{
	LPRParamLocal	m_params[LPR_MAX_REGIONS_PER_FRAME];	// 多个局部参数，一般情况下使用一个局部参数即可。
	int				m_nNumParams;							// m_params数组中局部参数的实际数目。
};

// 辅助函数
LPR_API void*	  __stdcall LPR_Create();
LPR_API void	  __stdcall LPR_Destroy( void* pObject );
LPR_API LPRRESULT __stdcall LPR_Init( void* pObject, const LPRParam& param, int width, int height );
LPR_API LPRRESULT __stdcall LPR_Init2( void* pObject, const LPRParam& param, int width, int height, bool bPicVersion );
LPR_API LPRRESULT __stdcall LPR_Fini( void* pObject );
LPR_API bool	  __stdcall LPR_IsInited( void* pObject );
LPR_API LPRRESULT __stdcall LPR_ProcessImage( void* pObject, LPRImage* pInput, LPROutputMulti* pOutput, LPRParamMulti& params, LPRImage* pMask );
LPR_API LPRRESULT __stdcall LPR_ProcessImage2( void* pObject, LPRImage* pInput, LPROutputMulti* pOutput, LPRParamMulti& params, LPRImage* pMask, int imageType );

// 初始化LPROutput结构
LPR_API void	  __stdcall LPROutput_Init( LPROutput* pOutput );
// 初始化LPROutputMulti结构
LPR_API void	  __stdcall LPROutputMulti_Init( LPROutputMulti* pOutputM );
// 向LPROutputMulti结构中添加一个识别结果
LPR_API bool	  __stdcall LPROutputMulti_Add( LPROutputMulti* pOutputM, const LPROutput& record );
// 从LPROutputMulti结构中获取最佳识别结果
LPR_API bool	  __stdcall LPROutputMulti_GetBest( const LPROutputMulti* pOutputM, LPROutput* pOutput );
LPR_API bool	  __stdcall LPROutputMulti_GetBest2( const LPROutputMulti* pOutputM, LPROutputMulti* pOutputUser, int nMaxPlate );
// 初始化LPRParamLocal结构
LPR_API void	  __stdcall LPRParamLocal_Init( LPRParamLocal* pParamL );
// 初始化LPRParamMulti结构
LPR_API void	  __stdcall LPRParamMulti_Init( LPRParamMulti* pParamM );
// 向LPRParamMulti结构中添加一个局部参数
LPR_API bool	  __stdcall LPRParamMulti_Add( LPRParamMulti* pParamM, const LPRParamLocal& param );


// LPR类，提供单张车牌识别的功能。
class LPR
{
public:
	LPR() { m_pObject = LPR_Create(); }
	~LPR() { LPR_Destroy( m_pObject ); }

	// 描述：初始化函数，应用程序需要在调用其他函数之前调用此函数进行初始化，
	//       否则其他函数调用会失败。
	// 参数：param - 车牌识别的参数，具体结构说明见LPRParam结构注释。
	//		 width - 需要处理的图片的宽度。
	//		 height - 需要处理的图片的高度。
	// 返回值： 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	// 说明：Init函数把LPR初始化为视频模式，如果需要使用单张模式，请调用Init2函数做初始化。
	LPRRESULT Init( const LPRParam& param, int width, int height )
	{
		return LPR_Init( m_pObject, param, width, height );
	}

	// 描述：初始化函数，应用程序需要在调用其他函数之前调用此函数进行初始化，
	//       否则其他函数调用会失败。
	// 参数：param - 车牌识别的参数，具体结构说明见LPRParam结构注释。
	//		 width - 需要处理的图片的宽度。
	//		 height - 需要处理的图片的高度。
	//		 bPicMode - 单张模式还是视频模式,true为单张模式,false为视频模式
	// 返回值： 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT Init2( const LPRParam& param, int width, int height, bool bPicMode )
	{
		return LPR_Init2( m_pObject, param, width, height, bPicMode );
	}

	// 描述：清理函数，当应用程序不再需要进行车牌识别时，应调用此函数以释放LPR所
	//       占用的资源。
	// 参数：无
	// 返回值： 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT Fini()
	{
		return LPR_Fini( m_pObject );
	}

	bool IsInited() { return LPR_IsInited( m_pObject ); }

	// 描述：识别函数
	// 参数：pImage - 车牌识别的输入图像数据，应用程序已经为该数据分配好内存。图像必须为RGB24格式，单场尺寸。
	//                LPRImage的说明参见LPRStructure.h头文件。
	//       pOutput - 指向LPROutput的指针，如果函数执行成功，该LPROutput会被填充为
	//                 车牌识别的结果。注意传入的LPROutputMulti结构必须已经调用LPROutputMulti_Init初始化。
	//		 params - 需要识别的矩形区域。
	//		 pImageMask - 留白掩码，一般情况下设为NULL即可。
	// 返回值： 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT ProcessImage( LPRImage* pImage, LPROutputMulti* pOutput, LPRParamMulti& params, LPRImage* pImageMask = NULL )
	{
		return LPR_ProcessImage( m_pObject, pImage, pOutput, params, pImageMask );
	}

	LPRRESULT ProcessImage2( LPRImage* pImage, LPROutputMulti* pOutput, LPRParamMulti params, LPRImage* pImageMask, int imageType )
	{
		return LPR_ProcessImage2( m_pObject, pImage, pOutput, params, pImageMask, imageType );
	}
private:
	LPR( const LPR& );				// not implemented
	LPR& operator= ( const LPR& );	// not implemented
	void*	m_pObject;
};

//
// 加密狗相关函数
//

// 获得加密狗序列号，功能和LPREngine.h中的LPRGetKeySerial函数相同，为完整起见在这里一起提供
LPR_API LPRRESULT  __stdcall LPRGetKeySerialNumber( unsigned long* pSerial );
// 读取加密狗中的用户数据
LPR_API LPRRESULT  __stdcall LPRGetUserData( LPRUserPasswd* pPassword, LPRUserData* pData );
// 把用户数据写入加密狗
LPR_API LPRRESULT  __stdcall LPRSetUserData( LPRUserPasswd* pPassword, LPRUserData* pData );
// 修改加密狗的用户密码
LPR_API LPRRESULT  __stdcall LPRSetUserPassword( LPRUserPasswd* pOldPassword, LPRUserPasswd* pNewPassword );


#ifdef __cplusplus
}
#endif


#endif // __LPR_H__

