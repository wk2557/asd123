#ifndef SUBTITLEOVERLAY_H
#define SUBTITLEOVERLAY_H

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


#include "EventAPPStructure.h"
#include <LPRVideo.h>
#include <string>
#include <map>

using std::map;
using std::string;


/**
 * 将pImSub图像复制到pImLarge图像的rect区域。
 */
bool __stdcall LPRCopySubImageToLarge(const LPRImage* pImSub, LPRImage* pImLarge, const RECT &rect);
/**
 * 将pImLarge图像的rect区域拷贝到pImSub图像。
 */
bool __stdcall LPRCopyLargeRegionToSub(const LPRImage *pImLarge, LPRImage *pImSub, const RECT &rect);
/**
 * 将pImForeground图像叠加到pImBackGround图像(x, y)开始的区域。
 */
bool __stdcall LPROverlay(LPRImage* pImForeground, LPRImage* pImBackGround, int x, int y);

#ifdef WIN32

/**
 * 将指定的字库按照给定的参数生成图片，并缓存起来。
 *
 * param subtitle - 字库内容
 * param fontFamilys - 字体家族数组
 * param fontFamilysCount - 字体家族数组的大小
 * param fontSizes - 字体大小数组
 * param fontSizesCount - 字体大小数组的大小
 * param savePath - dat文件的保存路径
 *
 * return 成功则返回缓存的图片，否则返回NULL
 */
EVENTAPP_API EventSubtitleImages*  __stdcall LPRGenerateCharacterImagesDat(wchar_t *subtitle, int *fontFamilys, int fontFamilysCount, int maxFontSize);

#endif//WIN32

/**
  * 给指定的图片在指定的位置添加指定的字幕，并另存为一张新的图片，该方法不会改变原来图片的内容。 
  *
  * param pRawImage - 以JPG格式编码过的图像（目前仅支持JPG码流输入）
  * param subtitle - 字幕内容，字幕内容中的宽字符必须都在字库中存在。
  * param fontParam - 字幕参数，具体含义请参考EventFont结构体的定义
  * param pImages - 图片库
  *
  * return 成功，则返回生成的图像，图像内存需要使用者自己释放，失败则返回NULL。
  */
LPRImage* __stdcall LPROverlaySubtitle(LPRImage *pRawImage, const wchar_t* subtitle, const EventFont &fontParam, const EventSubtitleImages *pImages);

#endif //SUBTITLEOVERLAY_H
