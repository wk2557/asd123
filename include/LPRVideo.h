#ifndef __LPR_VIDEO_H__
#define __LPR_VIDEO_H__

#include "LPRStructure.h"
#include "LPRResult.h"

#if defined(WIN32)
	#ifdef LPRVIDEO_EXPORTS
		#define LPRVIDEO_API __declspec(dllexport)
	#else
		#define LPRVIDEO_API __declspec(dllimport)
	#endif
#elif defined(LPR_LINUX)
	#ifdef LPRVIDEO_EXPORTS
		#define LPRVIDEO_API __attribute__ ((visibility("default")))
	#else
		#define LPRVIDEO_API
	#endif
#else
	#error "No platform specified!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LPR_VIDEOSRC_INVALID		0
#define LPR_VIDEOSRC_IMAGEFILE		1
#define LPR_VIDEOSRC_VIDEOFILE		2
#define LPR_VIDEOSRC_CAPTURE		3

#define LPR_CAPTURE_WIDTH_DEFAULT			-1
#define LPR_CAPTURE_HEIGHT_DEFAULT			-1

#define LPR_CAPTURE_COLOR_FORMAT_DEFAULT	-1
#define LPR_CAPTURE_COLOR_FORMAT_RGB24		0
#define LPR_CAPTURE_COLOR_FORMAT_RGB555		1
#define LPR_CAPTURE_COLOR_FORMAT_YUV422		2
#define LPR_CAPTURE_COLOR_FORMAT_YV12		3
#define LPR_CAPTURE_COLOR_FORMAT_MJPG		4
#define LPR_CAPTURE_COLOR_FORMAT_MAX		4

#define LPR_CAPTURE_STANDARD_DEFAULT		-1
#define LPR_CAPTURE_STANDARD_PAL			0
#define	LPR_CAPTURE_STANDARD_NTSC			1
#define	LPR_CAPTURE_STANDARD_MAX			1

#define LPR_CAPTURE_SCANMODE_DEFAULT		-1
#define LPR_CAPTURE_SCANMODE_FIELD			0
#define	LPR_CAPTURE_SCANMODE_FRAME			1
#define	LPR_CAPTURE_SCANMODE_MAX			1

#define LPR_CAPTURE_INPUT_TYPE_DEFAULT		-1
#define LPR_CAPTURE_INPUT_TYPE_COMPOSITE	0
#define	LPR_CAPTURE_INPUT_TYPE_SVIDEO		1
#define	LPR_CAPTURE_INPUT_TYPE_MAX			1

#define LPR_CAPTURE_BRIGHTNESS_DEFAULT		-1
#define LPR_CAPTURE_BRIGHTNESS_MAX			255
#define LPR_CAPTURE_CONTRAST_DEFAULT		-1
#define LPR_CAPTURE_CONTRAST_MAX			255
#define LPR_CAPTURE_HUE_DEFAULT				-1
#define LPR_CAPTURE_HUE_MAX					255
#define LPR_CAPTURE_SATURATION_DEFAULT		-1
#define LPR_CAPTURE_SATURATION_MAX			255

#define LPR_CAPTURE_OSC_DEFAULT				-1
#define LPR_CAPTURE_OSC_28M					0
#define	LPR_CAPTURE_OSC_35M					1
#define	LPR_CAPTURE_OSC_MAX					1

#define LPR_CAPTURE_CHANNEL_DEFAULT			-1
#define LPR_CAPTURE_CHANNEL_MAX				8

#if defined(WIN32)

#define LPR_VIDEO_SRC_NAME_MAX_LEN		512
struct LPRVideoSrcItem
{
	int		type;
	char	name[LPR_VIDEO_SRC_NAME_MAX_LEN];
};


LPRVIDEO_API void*		__stdcall LPRVideoSource_Create();
LPRVIDEO_API void		__stdcall LPRVideoSource_Destroy( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_Init( void* pObject, int srcType, const char* pSrcName );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_Fini( void* pObject );
LPRVIDEO_API bool		__stdcall LPRVideoSource_IsInited( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_GetNextImage( void* pObject, LPRImage** ppImage );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_ReleaseImage( void* pObject, LPRImage* pImage );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_GetCaptureSourceList( LPRVideoSrcItem* pItemArray, int* pArraySize );

LPRVIDEO_API int		__stdcall LPRVideoSource_GetWidth( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetWidth( void* pObject, int width );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetHeight( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetHeight( void* pObject, int height );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetColorFormat( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetColorFormat( void* pObject, int format );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetStandard( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetStandard( void* pObject, int standard );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetScanMode( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetScanMode( void* pObject, int scanMode );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetInputType( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetInputType( void* pObject, int inputType );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetBrightness( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetBrightness( void* pObject, int brightness );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetContrast( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetContrast( void* pObject, int contrast );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetHue( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetHue( void* pObject, int hue );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetSaturation( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetSaturation( void* pObject, int saturation );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetOSC( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetOSC( void* pObject, int osc );
LPRVIDEO_API int		__stdcall LPRVideoSource_GetChannel( void* pObject );
LPRVIDEO_API LPRRESULT	__stdcall LPRVideoSource_SetChannel( void* pObject, int channel );


class LPRVideoSource
{
public:
	LPRVideoSource() { m_pObject = LPRVideoSource_Create(); }
	~LPRVideoSource() { LPRVideoSource_Destroy( m_pObject ); }

	// 初始化
	// srcType - 视频源类型
	// pSrcName - 视频源名称
	LPRRESULT Init( int srcType, const char* pSrcName )
	{
		return LPRVideoSource_Init( m_pObject, srcType, pSrcName );
	}
	// 清理
	LPRRESULT Fini()
	{
		return LPRVideoSource_Fini( m_pObject );
	}

	bool IsInited()
	{
		return LPRVideoSource_IsInited( m_pObject );
	}

	// 从视频源读取数据
	// 没有数据则将 *ppImage 置为 NULL
	LPRRESULT GetNextImage( LPRImage** ppImage )
	{
		return LPRVideoSource_GetNextImage( m_pObject, ppImage ); 
	}

	// 释放视频源数据
	LPRRESULT ReleaseImage( LPRImage* pImage )
	{
		return LPRVideoSource_ReleaseImage( m_pObject, pImage );
	}

	static LPRRESULT GetCaptureSourceList( LPRVideoSrcItem* pItemArray, int* pArraySize )
	{
		return LPRVideoSource_GetCaptureSourceList( pItemArray, pArraySize );
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	
	// 获取采集图像宽度
	// 参数：
	// 返回值：
	//   返回采集设备画面的宽度，如果是-1表示缺省宽度，每个设备的缺省宽度不同，具体参见相应设备的说明。
	int GetWidth() { return LPRVideoSource_GetWidth( m_pObject ); }

	// 设置采集图像宽度
	// 参数：
	//   width - 新的图像宽度，-1表示使用缺省宽度。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetWidth( int width ) { return LPRVideoSource_SetWidth( m_pObject, width ); }

	// 获取采集图像高度
	// 参数：
	// 返回值：
	//   返回采集设备画面的高度，如果是-1表示缺省高度，每个设备的缺省高度不同，具体参见相应设备的说明。
	int GetHeight() { return LPRVideoSource_GetHeight( m_pObject ); }

	// 设置采集图像高度
	// 参数：
	//   height - 新的图像高度，-1表示使用缺省高度。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetHeight( int height ) { return LPRVideoSource_SetHeight( m_pObject, height ); }

	// 获取颜色格式
	// 参数：
	// 返回值：
	//   返回采集设备使用的颜色格式，可能的返回值参见此头文件开始处的以LPR_CAPTURE_COLOR_FORMAT开头的宏定义，
	//   如果是-1表示缺省颜色格式，每个设备的缺省颜色格式不同，具体参见相应设备的说明。
	int GetColorFormat() { return LPRVideoSource_GetColorFormat( m_pObject ); }

	// 设置颜色格式
	// 参数：
	//   format - 新的颜色格式，可取值参见此头文件开始处的以LPR_CAPTURE_COLOR_FORMAT开头的宏定义。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetColorFormat( int format ) { return LPRVideoSource_SetColorFormat( m_pObject, format ); }

	// 获取图像制式（PAL或NTSC）
	// 参数：
	// 返回值：
	//   返回采集设备使用的图像制式，可能的返回值参见此头文件开始处的以LPR_CAPTURE_STANDARD开头的宏定义，
	//   如果是-1表示缺省图像制式，每个设备的缺省图像制式不同，具体参见相应设备的说明。
	int GetStandard() { return LPRVideoSource_GetStandard( m_pObject ); }

	// 设置图像制式（PAL或NTSC）
	// 参数：
	//   standard - 新的图像制式，可取值参见此头文件开始处的以LPR_CAPTURE_STANDARD开头的宏定义。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetStandard( int standard ) { return LPRVideoSource_SetStandard( m_pObject, standard ); }

	// 获取扫描模式（帧或场模式）
	// 参数：
	// 返回值：
	//   返回采集设备使用的扫描模式，可能的返回值参见此头文件开始处的以LPR_CAPTURE_SCANMODE开头的宏定义，
	//   如果是-1表示缺省扫描模式，每个设备的缺省扫描模式不同，具体参见相应设备的说明。
	int GetScanMode() { return LPRVideoSource_GetScanMode( m_pObject ); }

	// 设置扫描模式（PAL或NTSC）
	// 参数：
	//   scanMode - 新的扫描模式，可取值参见此头文件开始处的以LPR_CAPTURE_SCANMODE开头的宏定义。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetScanMode( int scanMode ) { return LPRVideoSource_SetScanMode( m_pObject, scanMode ); }

	// 获取输入信号类型（Composite或SVideo）
	// 参数：
	// 返回值：
	//   返回采集设备使用的输入信号类型，可能的返回值参见此头文件开始处的以LPR_CAPTURE_INPUT_TYPE开头的宏定义，
	//   如果是-1表示缺省输入信号类型，每个设备的缺省输入信号类型不同，具体参见相应设备的说明。
	int GetInputType() { return LPRVideoSource_GetInputType( m_pObject ); }

	// 设置输入信号类型（Composite或SVideo）
	// 参数：
	//   inputType - 新的输入信号类型，可取值参见此头文件开始处的以LPR_CAPTURE_INPUT_TYPE开头的宏定义。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetInputType( int inputType ) { return LPRVideoSource_SetInputType( m_pObject, inputType ); }

	// 获取图像亮度
	// 参数：
	// 返回值：
	//   返回采集设备的图像亮度，如果是-1表示缺省亮度，每个设备的缺省亮度不同，具体参见相应设备的说明。
	int GetBrightness() { return LPRVideoSource_GetBrightness( m_pObject ); }

	// 设置图像亮度
	// 参数：
	//   brightness - 新的图像亮度，取值范围0-255，-1表示使用缺省亮度。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetBrightness( int brightness ) { return LPRVideoSource_SetBrightness( m_pObject, brightness ); }

	// 获取图像对比度
	// 参数：
	// 返回值：
	//   返回采集设备的图像对比度，如果是-1表示缺省对比度，每个设备的缺省对比度不同，具体参见相应设备的说明。
	int GetContrast() { return LPRVideoSource_GetContrast( m_pObject ); }

	// 设置图像对比度
	// 参数：
	//   contrast - 新的图像对比度，取值范围0-255，-1表示使用缺省对比度。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetContrast( int contrast ) { return LPRVideoSource_SetContrast( m_pObject, contrast ); }

	// 获取图像色度
	// 参数：
	// 返回值：
	//   返回采集设备的图像色度，如果是-1表示缺省色度，每个设备的缺省色度不同，具体参见相应设备的说明。
	int GetHue() { return LPRVideoSource_GetHue( m_pObject ); }
	
	// 设置图像色度
	// 参数：
	//   hue - 新的图像色度，取值范围0-255，-1表示使用缺省色度。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetHue( int hue ) { return LPRVideoSource_SetHue( m_pObject, hue ); }

	// 获取图像饱和度
	// 参数：
	// 返回值：
	//   返回采集设备的图像饱和度，如果是-1表示缺省饱和度，每个设备的缺省饱和度不同，具体参见相应设备的说明。
	int GetSaturation() { return LPRVideoSource_GetSaturation( m_pObject ); }

	// 设置图像饱和度
	// 参数：
	//   saturation - 新的图像饱和度，取值范围0-255，-1表示使用缺省饱和度。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetSaturation( int saturation ) { return LPRVideoSource_SetSaturation( m_pObject, saturation ); }

	// 获取采集卡晶振频率
	// 参数：
	// 返回值：
	//   返回采集卡晶振频率，可能的返回值参见此头文件开始处的以LPR_CAPTURE_OSC开头的宏定义，
	//   此参数目前仅对大恒卡有效。
	int GetOSC() { return LPRVideoSource_GetOSC( m_pObject ); }

	// 设置采集卡晶振频率
	// 参数：
	//   osc - 新的采集卡晶振频率，可取值参见此头文件开始处的以LPR_CAPTURE_OSC开头的宏定义。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	LPRRESULT SetOSC( int osc ) { return LPRVideoSource_SetOSC( m_pObject, osc ); }

	// 获取采集卡当前通道号
	// 参数：
	// 返回值：
	//   返回采集卡当前通道号，此参数仅对支持通道选择的采集卡有效，具体参见相应设备的说明。
	int GetChannel() { return LPRVideoSource_GetChannel( m_pObject ); }

	// 设置采集卡当前通道号，应用程序可以通过此函数在采集卡的多个通道间切换。
	// 参数：
	//   channel - 新的通道号，可取值参见此头文件开始处的以LPR_CAPTURE_OSC开头的宏定义。
	// 返回值：
	//   大于等于0表示成功，小于0表示失败。错误值含义参见LPRResult.h。
	//   此参数仅对支持通道选择的采集卡有效，具体参见相应设备的说明。
	LPRRESULT SetChannel( int channel ) { return LPRVideoSource_SetChannel( m_pObject, channel ); }

private:
	LPRVideoSource( const LPRVideoSource& );			// not implemented
	LPRVideoSource& operator=( const LPRVideoSource& );	// not implemented
	void*		m_pObject;
};

#endif


/// 拷贝图像的一个矩形区域到另一个图像
/// 输入 pSrcImage --- 原图，大图
/// 输入 rect	   --- 指定的矩形区域
/// 输出 pDstImage --- 目标图，用于存放扣取的图像
LPRVIDEO_API void __stdcall LPRCopySubImage( const LPRImage* pSrcImage, LPRImage* pDstImage, RECT rect );

/// 创建图像
LPRVIDEO_API LPRImage* __stdcall LPRCreateImage( int nWidth, int nHeight, int nDepth, int nChannels );
/// 创建预定义格式的图像，支持CS_GRAY, CS_RGB24, 以及LPRColorSpace中包含的YUV格式
LPRVIDEO_API LPRImage* __stdcall LPRCreateImage2( int nWidth, int nHeight, LPRColorSpace nColorMode );
LPRVIDEO_API void		__stdcall LPRReleaseImage( LPRImage* pImage );
LPRVIDEO_API LPRImage* __stdcall LPRCloneImage( const LPRImage* pImage );

/// YUV 格式转换函数
LPRVIDEO_API LPRRESULT __stdcall LPRConvertYUVToGRAY(const LPRImage *pImYUV, LPRImage *pImGray, bool bFlip = false);
LPRVIDEO_API LPRRESULT __stdcall LPRConvertYUVToRGB(const LPRImage *pImYUV, LPRImage *pImRGB, bool bFlip = false);

/// 注意：以下函数不支持YUV格式图像的处理
LPRVIDEO_API LPRImage* __stdcall LPRCropImage( LPRImage* pImage, RECT rect );
LPRVIDEO_API LPRRESULT __stdcall LPRFlipImage( const LPRImage* pSrcImage, LPRImage* pDstImage );
LPRVIDEO_API void		__stdcall LPRResizeImage( const LPRImage* pSrcImage, LPRImage* pDstImage );
LPRVIDEO_API void		__stdcall LPRResizeImage2( const LPRImage* pSrcImage, LPRImage* pDstImage );
LPRVIDEO_API LPRRESULT __stdcall LPRSaveImage( LPRImage* pImage, const char* pFilePath );
LPRVIDEO_API LPRImage* __stdcall LPRConvertImageToGray( LPRImage* pImage );
LPRVIDEO_API LPRRESULT __stdcall LPRFillImage( LPRImage* pDest, RECT rect, LPRImage* pSrc );


/// 构造一个LPRImage数据结构，表示原图的一个子图，它没有新分配内存，只是调整了指针到大图合适的位置
/// 目前只适用于输入为LPRImage::nColorMode == CS_RGB24，
/// 输入 pSrcImage --- 原图，大图
/// 输入 rect	   --- 指定的矩形区域
/// 输出		   --- 设定了合适指针和内部参数的LPRImage，这个数据结构不要释放内存
LPRVIDEO_API LPRImage __stdcall LPRGetSubImageStruct( const LPRImage* pImage, RECT rect );

/// 以下为压缩、解压缩指定的format参数
#define LPR_ENCODE_FORMAT_NONE			1	// 不做压缩
#define LPR_ENCODE_FORMAT_JPG			2	// 使用JPEG格式压缩
#define LPR_ENCODE_FORMAT_J2K			3	// 使用JPEG2000格式压缩

//
// 压缩图像至内存
//
// 参数：
//	pImage - 需要压缩的图像，目前只支持BGR 3通道的彩色图像
//  pBuffer - 数据缓存区，用于保存压缩后的图像数据。
//  pLen   - 指向一个int型变量，该int变量内保存了数据缓存区的长度。
//  format - 图像的压缩格式，可取的值参见前面LPR_ENCODE_FORMAT_开头的宏。
//  param  - 压缩算法的参数，如果format取值为LPR_ENCODE_FORMAT_JPG，此参数表示图像压缩的质量，取值范围为1-100，值越大文件体积越大，但图像质量越好。
//			 如果format取值为LPR_ENCODE_FORMAT_J2K，那么此参数取值含义如下：
//			 param取值应小于0   ： 指定压缩后的数据大小为param的绝对值，单位为Byte。比如-2000表示图像压缩后大约为2000字节。
// 返回值：
//  大于等于0表示成功，小于0表示失败。如果返回值为LPR_BUFFER_TOO_SMALL，表示提供的缓存区太小，pLen指向的int变量内保存着所需要的最小
//	缓存区长度。
//  
LPRVIDEO_API LPRRESULT __stdcall LPREncodeImage( const LPRImage* pImage, unsigned char* pBuffer, int* pLen, int format, int param );

//
// 从内存解压图像
//
// 参数：
//	ppImage - 用于保存解压后图像的指针地址。
//           注意：如果*ppImage为NULL，LPRDecodeImage内部会分配内存，那么外部需要调用LPRReleaseImage释放内存
//                 如果*ppImage不为NULL，并且和当前需要解码的图像大小等属性相同，那么内部就不重新分配内存，而直接覆盖*ppImage
//                 在这种情况下，可以避免多次内存分配和释放，只需要最后释放一次即可。
//  pBuffer - 数据缓存区，存放着待解压的数据。
//  len     - 数据缓存区的长度。
//  format - 图像的压缩格式，可取的值参见前面LPR_ENCODE_FORMAT_开头的宏。
//  param  - 保留，设为0。
//
// 返回值：
//  大于等于0表示成功，小于0表示失败。
//
LPRVIDEO_API LPRRESULT __stdcall LPRDecodeImage( LPRImage** ppImage, const unsigned char* pBuffer, int len, int format, int param );


#define LPR_ENCODE_PLATE_FORMAT_NONE			1		// 不做压缩
#define LPR_ENCODE_PLATE_FORMAT_J2K_COLOR		2		// 把车牌压缩为JPEG200彩图
#define LPR_ENCODE_PLATE_FORMAT_J2K_GRAY		3		// 把车牌压缩为JPEG200灰度图

//
// 压缩车牌图像
//
// 参数：
//	pImage - 视频图像
//  output - pImage图像的识别结果
//  pBuffer - 数据缓存区，用于保存压缩后的车牌图像数据。
//  pLen   - 指向一个int型变量，该int变量内保存了数据缓存区的长度。
//  width  - 车牌的宽度，LPREncodePlate会自动把车牌校正为指定宽度。
//  height - 车牌的高度，LPREncodePlate会自动把车牌校正为指定高度。
//  format - 车牌图像的压缩格式，可取的值参见前面LPR_ENCODE_PLATE_FORMAT_开头的宏。
//  param  - 车牌压缩算法的参数，如果format取值为LPR_ENCODE_PLATE_FORMAT_J2K_COLOR或LPR_ENCODE_PLATE_FORMAT_J2K_GRAY，
//			 此参数取值含义如下：
//				1-100： 表示指定图像压缩的质量，值越大压缩效果越好。（目前尚未支持，返回LPR_NOT_SUPPORTED）
//				<0   ： 表示指定压缩后的数据大小，所指定的大小为param的绝对值，单位为Byte。比如-400表示车牌压缩后大约为400字节。
// 返回值：
//  大于等于0表示成功，小于0表示失败。如果返回值为LPR_BUFFER_TOO_SMALL，表示提供的缓存区太小，pLen指向的int变量内保存着所需要的最小
//	缓存区长度。
//  
LPRVIDEO_API LPRRESULT __stdcall LPREncodePlate( const LPRImage* pImage, const LPROutput& output, unsigned char* pBuffer, int* pLen, 
												int width, int height, int format, int param );

//
// 保存图像
//
//	pImage - 需要保存的图像
//  pFilePath - 保存的文件路径和文件名，文件名需要包含扩展名以指定图像压缩格式，目前支持如下格式：
//					.bmp - 不做压缩，保存为位图文件。
//					.jpg - 压缩并保存为jpeg文件。
//					.j2k - 压缩并保存为jpeg2000文件。
//  param  - 压缩算法的参数，如果保存为bmp文件，此参数忽略。如果保存为jpg文件，此参数表示图像压缩的质量，取值范围为1-100，值越大压缩效果越好。
//			 如果保存为jpeg2000文件，那么此参数取值含义如下：
//				1-100： 表示指定图像压缩的质量，值越大压缩效果越好。（目前尚未支持，返回LPR_NOT_SUPPORTED）
//				<0   ： 表示指定压缩后的文件大小，所指定的大小为param的绝对值，单位为Byte。比如-2000表示压缩后保存的文件大约为2000字节。
// 返回值：
//  大于等于0表示成功，小于0表示失败。
// 
LPRVIDEO_API LPRRESULT __stdcall LPRSaveImage2( LPRImage* pImage, const char* pFilePath, int param );
LPRVIDEO_API LPRRESULT __stdcall LPRLoadImage( const char* pFilePath, LPRImage** ppImage );
LPRVIDEO_API LPRRESULT __stdcall LPRLoadImage2( const char* pFilePath, LPRImage** ppImage, int param );
LPRVIDEO_API LPRRESULT __stdcall LPRGenerateMask( LPRImage* pImage, LPRRatioLine line1, LPRRatioLine line2 );

// 用于描述任意的四边形，注意pt1,pt2,pt3,pt4必须依次为四边形的顺时针或逆时针的顶点
struct LPRQuadrangle
{
	POINT		pt1;
	POINT		pt2;
	POINT		pt3;
	POINT		pt4;
};

//
// 生成识别区域掩码图像
//
//	ppMask - 用于保存掩码图像的指针地址。
//  width - 生成的掩码图像宽度
//  height - 生成的掩码图像高度
//  pQuadrangles - 一个或多个四边形识别区域，这些四边形识别区域的“或”即为生成的掩码图像中的识别区域。
//  num  - 　pQuadrangles指向的数组中四边形的个数
// 返回值：
//  大于等于0表示成功，小于0表示失败。如果函数执行成功，那么*ppMask内存放着生成的掩码图像，应用程序使用完
//  该图像后需要调用LPRReleaseImage释放该图像．
LPRVIDEO_API LPRRESULT __stdcall LPRGenerateMask2( LPRImage** ppMask, int width, int height, const LPRQuadrangle* pQuadrangles, int num );

/************************************************************************/
/* 统计车牌图像的亮度对比度                                             */
/************************************************************************/
//输入
//
// pImageFull - 抓拍到的图像，一般从LPRInput结构获得（使用ImageFromInput）函数。
// lpr		  - 对应于抓拍图像的识别结果
// 输出：
// mean		- 车牌图象亮度的均值
// var		- 车牌图象亮度的方差
//
LPRVIDEO_API void __stdcall LPRGetPlateLighting( const LPRImage* pImageFull, const LPROutput& lpr, double* mean, double* var);

/************************************************************************/
/* 获取车牌底色                                                         */
/************************************************************************/
//输入
//
// pImageFull - 抓拍到的图像，一般从LPRInput结构获得（使用ImageFromInput）函数。
// lpr		  - 对应于抓拍图像的识别结果
// 输出：
// r, g, b	  - 车牌底色的rgb值
// 
//
LPRVIDEO_API void __stdcall LPRGetPlateRGB( const LPRImage* pImageFull, const LPROutput& lpr, int* r, int* g, int* b);


/************************************************************************/
/* 获取车身颜色                                                         */
/************************************************************************/
//输入
//
// pImageFull - 抓拍到的图像，一般从LPRInput结构获得（使用ImageFromInput）函数。
// lpr		  - 对应于抓拍图像的识别结果
// 输出：
// r, g, b	  - 车身颜色的rgb值
// 
//
LPRVIDEO_API void __stdcall LPRGetCarRGB( const LPRImage* pImageFull, const LPROutput& lpr, int* r, int* g, int* b);

/************************************************************************/
/* 车牌图像矫正以及二值化                                               */
/************************************************************************/
//输入
//
// pImageFull - 抓拍到的图像，一般从LPRInput结构获得（使用ImageFromInput）函数。
// lpr		  - 对应于抓拍图像的识别结果
// width	  - 输出标准宽度，内部做图像缩放。
// height	  - 输出标准高度，内部做图像缩放。
//
// 输出：
// *ppImgPlate	  - 矫正之后的车牌图像，彩色。
//					函数内部分配内存，调用者用LPRReleaseImage释放，原始大小。
// *ppImgBinPlate - 矫正之后的二值车牌图像，用0和255表示，一个象素一个byte（不是一个bit）。
//					该图像可用于后续的JBig压缩。
//					函数内部分配内存，调用者用LPRReleaseImage释放，指定大小(width, height)。
//
LPRVIDEO_API void __stdcall LPRRectifyPlate( const LPRImage* pImageFull, const LPROutput& lpr, LPRImage** ppImgPlate, LPRImage** ppImgBinPlate, int width, int height );

/************************************************************************/
/* JBig 图像压缩编码解码API                                                 */
/************************************************************************/
// LPREncodeJBig, 编码JBig
// 输入：
//	pImage - 用0和255表示的二值图，每一个象素一个byte，
//			 通常是经过LPRRectifyPlate输出的 *ppImgBinPlate，
//	pBuffer - 为编码后的JBig图像缓冲区，需要由调用者分配。
//
// 参数：
//	nBufferLength - pBuffer的长度。
//
// 返回值：
//	> 0  - 编码后JBig数据的实际长度
//	= 0  - 外界提供的pBuffer长度太小，没有编码成功
//  < 0  - 其他错误
//
LPRVIDEO_API int __stdcall LPREncodeJBig( const LPRImage* pImage, unsigned char* pBuffer, int nBufferLength );

// LPRDecodeJBig, 解码JBig
// 输入：
//
//	pBuffer - JBig图像缓冲区，由调用者从文件或外界存储中得到。
//	nBufferLength - pBuffer的长度。
//
// 输出：
//
//	pImage - 用0和255表示的二值图，每一个象素一个byte（不是一个bit一个象素），可以正常显示、保存bmp/jpg文件。
//
// 返回值：
//	NULL - 解码失败。
//	合法的地址 - 成功解码并且创建了一个LPRImage缓冲区，需要调用者使用完之后用LPRReleaseImage()释放
//
LPRVIDEO_API LPRImage* __stdcall LPRDecodeJBig( const unsigned char* pBuffer, int nBufferLength );

/************************************************************************/
/* 点阵图像编码解码API                                                 */
/************************************************************************/
// LPREncodePointArray, 编码成为点阵格式：
// 
// 112 X 20的点阵，每个象素一个bit，一个字符8个象素，共280个字节的字符串。
// 其中点阵排列规则如下：图片中位置上方的行在数组中位于前面，即前14个字符的数据与二值化图第一行的象素对应；
// 一个字符与8位象素对应，其中字节低位的数据位于图形的左方，即若有一个字符值为0x01，
// 则该字符对应的8个象素中，只有最左边一个点为白色，其余点为黑色。
//
// 输入：
//	pImage - 用0和255表示的二值图，每一个象素一个byte，
//			 通常是经过LPRRectifyPlate输出的 *ppImgBinPlate，
//
// 输出：
//	pBuffer - 编码后的点阵图像缓冲区，需要由调用者分配。
//	nBufferLength - pBuffer的长度。
//
// 返回值：
//	> 0  - 编码后点阵数据的实际长度
//	= 0  - 外界提供的pBuffer长度太小，没有编码成功
//  < 0  - 其他错误
//
LPRVIDEO_API int __stdcall LPREncodePointArray( const LPRImage* pImage, unsigned char* pBuffer, int nBufferLength );

// LPRDecodePointArray, 解码点阵格式图像
// 输入：
//
//	pBuffer - 点阵图像缓冲区，由调用者从文件或外界存储中得到。
//	nBufferLength - pBuffer的长度。
//  width	- 希望输出的图像宽度。
//  height  - 希望输出的图像高度。
//  nBufferLength == width * height;
//
// 输出：
//
//	pImage - 用0和255表示的二值图，每一个象素一个byte（不是一个bit一个象素），可以正常显示、保存bmp/jpg文件。
//
// 返回值：
//	NULL - 解码失败。
//	合法的地址 - 成功解码并且创建了一个LPRImage缓冲区，需要调用者使用完之后用LPRReleaseImage()释放
//
LPRVIDEO_API LPRImage* __stdcall LPRDecodePointArray( const unsigned char* pBuffer, int nBufferLength, int width, int height );

#if defined(WIN32)


LPRVIDEO_API HANDLE __stdcall LPRCreateVideoWriter( const char* fileName, int width, int height, int frameRate );
LPRVIDEO_API void __stdcall LPRWriteVideoFrame( HANDLE handle, LPRImage* pImage );
LPRVIDEO_API void __stdcall LPRReleaseVideoWriter( HANDLE handle );


#define LPRRENDER_MAX_LINES				16
#define LPRRENDER_MAX_RECTS				16
#define LPRRENDER_MAX_TEXTS				16

#define LPRRENDER_TEXT_EFFECT_NONE		0x0
#define LPRRENDER_TEXT_EFFECT_OUTLINE	0x1

struct LPRRenderLine
{
	POINT		pt1;
	POINT		pt2;
	HPEN		hPen;
};

struct LPRRenderRect
{
	RECT		rect;
	HPEN		hPen;
};

struct LPRRenderText
{
	char		text[256];
	POINT		pos;
	SIZE		scale;
	HFONT		hFont;
	COLORREF	textColor;
	DWORD		bkMode;
	COLORREF	bkColor;
	COLORREF	outlineColor;
	INT			effect;
};

struct LPRRenderLine2
{
	POINT		pt1;
	POINT		pt2;
	int			penStyle;
	int			penWidth;
	COLORREF	penColor;
};

struct LPRRenderRect2
{
	RECT		rect;
	int			penStyle;
	int			penWidth;
	COLORREF	penColor;
};

struct LPRRenderText2
{
	char		text[256];
	POINT		pos;
	SIZE		scale;
	int			fontSize;
	char		fontName[256];
	COLORREF	textColor;
	DWORD		bkMode;
	COLORREF	bkColor;
	COLORREF	outlineColor;
	INT			effect;	
};

LPRVIDEO_API void* __stdcall LPRRender_Create();
LPRVIDEO_API void  __stdcall LPRRender_Destroy( void* pObject );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_SetDestWindow( void* pObject, HWND hWnd );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_SetDestRect( void* pObject, RECT rect );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_Reset( void* pObject );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_SetImage( void* pObject, LPRImage* pImage, SIZE actualSize, LPRImage** ppOldImage );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_AddLine( void* pObject, const LPRRenderLine& item );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_AddRect( void* pObject, const LPRRenderRect& item );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_AddText( void* pObject, const LPRRenderText& item );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_ClearLines( void* pObject );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_ClearRects( void* pObject );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_ClearTexts( void* pObject );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_Update( void* pObject );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_UpdateToImage( void* pObject, LPRImage* pImage );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_SetLine( void* pObject, int index, const LPRRenderLine2& item );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_SetRect( void* pObject, int index, const LPRRenderRect2& item );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_SetText( void* pObject, int index, const LPRRenderText2& item );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_ClearLine( void* pObject, int index );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_ClearRect( void* pObject, int index );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_ClearText( void* pObject, int index );
LPRVIDEO_API LPRRESULT __stdcall LPRRender_SetBkgrdColor( void* pObject, COLORREF color );


class LPRRender
{
public:
	LPRRender() { m_pObject = LPRRender_Create(); }
	~LPRRender() { LPRRender_Destroy( m_pObject ); }

	LPRRESULT SetDestWindow( HWND hWnd )
	{
		return LPRRender_SetDestWindow( m_pObject, hWnd );
	}
	LPRRESULT SetDestRect( RECT rect )
	{
		return LPRRender_SetDestRect( m_pObject, rect );
	}
	LPRRESULT SetBkgrdColor( COLORREF color )
	{
		return LPRRender_SetBkgrdColor( m_pObject, color );
	}
	LPRRESULT Reset()
	{
		return LPRRender_Reset( m_pObject );
	}
	LPRRESULT SetImage( LPRImage* pImage, SIZE actualSize, LPRImage** ppOldImage )
	{
		return LPRRender_SetImage( m_pObject, pImage, actualSize, ppOldImage );
	}
	LPRRESULT AddLine( const LPRRenderLine& item )
	{
		return LPRRender_AddLine( m_pObject, item );
	}
	LPRRESULT AddRect( const LPRRenderRect& item )
	{
		return LPRRender_AddRect( m_pObject, item );
	}
	LPRRESULT AddText( const LPRRenderText& item )
	{
		return LPRRender_AddText( m_pObject, item );
	}
	LPRRESULT ClearLines()
	{
		return LPRRender_ClearLines( m_pObject );
	}
	LPRRESULT ClearRects()
	{
		return LPRRender_ClearRects( m_pObject );
	}
	LPRRESULT ClearTexts()
	{
		return LPRRender_ClearTexts( m_pObject );
	}
	LPRRESULT Update()
	{
		return LPRRender_Update( m_pObject );
	}
	LPRRESULT UpdateToImage( LPRImage* pImage )
	{
		return LPRRender_UpdateToImage( m_pObject, pImage );
	}
	LPRRESULT SetLine( int index, const LPRRenderLine2& item )
	{
		return LPRRender_SetLine( m_pObject, index, item );
	}
	LPRRESULT SetRect( int index, const LPRRenderRect2& item )
	{
		return LPRRender_SetRect( m_pObject, index, item );
	}
	LPRRESULT SetText( int index, const LPRRenderText2& item )
	{
		return LPRRender_SetText( m_pObject, index, item );
	}
	LPRRESULT ClearLine( int index )
	{
		return LPRRender_ClearLine( m_pObject, index );
	}
	LPRRESULT ClearRect( int index )
	{
		return LPRRender_ClearRect( m_pObject, index );
	}
	LPRRESULT ClearText( int index )
	{
		return LPRRender_ClearText( m_pObject, index );
	}

private:
	void*	m_pObject;
};

LPRVIDEO_API void* __stdcall LPRAviWriter_Create();
LPRVIDEO_API void  __stdcall LPRAviWriter_Destroy( void* pObject );
LPRVIDEO_API LPRRESULT __stdcall LPRAviWriter_Init( void* pObject, const char* pFileName, int frameRate, DWORD codec );
LPRVIDEO_API LPRRESULT __stdcall LPRAviWriter_WriteFrame( void* pObject, LPRImage* pImage, bool bFlip );
LPRVIDEO_API LPRRESULT __stdcall LPRAviWriter_Fini( void* pObject );
LPRVIDEO_API bool	   __stdcall LPRAviWriter_IsInited( void* pObject );

#define LPR_MAKE_FOURCC( ch0, ch1, ch2, ch3 ) \
	( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
	( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

class LPRAviWriter
{
public:
	LPRAviWriter() { m_pObject = LPRAviWriter_Create(); }
	~LPRAviWriter() { LPRAviWriter_Destroy( m_pObject ); }

	LPRRESULT Init( const char* pFileName, int frameRate, DWORD codec )
	{
		return LPRAviWriter_Init( m_pObject, pFileName, frameRate, codec );
	}
	LPRRESULT WriteFrame( LPRImage* pImage, bool bFlip )
	{
		return LPRAviWriter_WriteFrame( m_pObject, pImage, bFlip );
	}	
	LPRRESULT Fini()
	{
		return LPRAviWriter_Fini( m_pObject );
	}
	bool IsInited()
	{
		return LPRAviWriter_IsInited( m_pObject );
	}
private:
	void*	m_pObject;
};

#endif


#ifdef __cplusplus
}
#endif
	
#endif //__LPR_VIDEO_H__

