#ifndef __LPR_EX_H__
#define __LPR_EX_H__

#include "LPR.h"
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


// 辅助函数
/************************************************************************/
/* 获取车身颜色                                                    */
/************************************************************************/
// 输入：
// pImageFull - 抓拍到的图像，一般从LPRInput结构获得（使用ImageFromInput）函数。
// lpr		  - 对应于抓拍图像的识别结果
// 
// 输出：
// r, g, b	  - 车身颜色的rgb值
// 
//
LPR_API void __stdcall LPRGetCarRGBEx( const LPRImage* pImageFull, const LPROutput& lpr, int* r, int* g, int* b);

/************************************************************************/
/* 根据车牌识别结果，检测车标、人脸；暂只支持RGB格式                    */
/************************************************************************/
// 输入：
// pInput     - 抓拍到的图像，一般从LPRInput结构获得（使用ImageFromInput）函数。
// nImageType - 抓拍图像是场模式（LPR_IMAGE_TYPE_FIELD）或是帧模式（LPR_IMAGE_TYPE_FRAME），
// bRecBrand  - 是否识别车标
// bDetFace   - 是否检测人脸
// pLPROutput - 对应于pInput的车牌识别结果
// 
// 输出：
// pLPROutput - 该函数会将车标、人脸检测结果填充到pLPROutput
// 
//
LPR_API LPRRESULT __stdcall LPRGetBrandFaceEx( const LPRImage* pInput, int nImageType, LPROutput* pLPROutput, bool bRecBrand, bool bDetFace );

#ifdef __cplusplus
}
#endif

#endif //__LPR_EX_H__
