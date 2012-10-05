#ifndef SUBTITLEOVERLAY_H
#define SUBTITLEOVERLAY_H

#include "EventAPPStructure.h"
#include <LPRVideo.h>
#include <string>
#include <map>

using std::map;
using std::string;

bool LPRCopySubImageToLarge(const LPRImage* pImSub, LPRImage* pImLarge, RECT rect);
bool LPROverlay(LPRImage* pImForeground, LPRImage* pImBackGround, int x, int y);

class SubtitleOverlay
{
public:
	virtual ~SubtitleOverlay(void);
	static SubtitleOverlay& getInstance();
	/**
	 * 将指定字库中的字按照给定的参数生成图片，并缓存。为了效率起见，
	 * 请确保字库中的字已经覆盖了以后将要用到的所有字。
	 *
	 * param str - 字库
	 * param fontParam - 字幕参数，具体含义请参考EventFont结构体的定义
	 */
	void initialize(const wchar_t *wstr, const EventFont &fontParam);
	/**
	 * 给指定的图片在指定的位置添加指定的字幕，并另存为一张新的图片，该方法不会改变原来图片的内容。 
	 *
	 * param pRawImage - 以JPG格式编码过的图像（目前仅支持JPG码流输入）
	 * param subtitle - 字幕内容
	 * param fontParam - 字幕参数，具体含义请参考EventFont结构体的定义
	 *
	 * return 成功，则返回生成的图像，图像内存需要使用者自己释放，失败则返回NULL。
	 */
	LPRImage* overlaySubtitle(LPRImage *pRawImage, const wchar_t* subtitle, const EventFont &fontParam);
private:
	SubtitleOverlay(void) {}
	SubtitleOverlay(const SubtitleOverlay &other) {}
	SubtitleOverlay& operator=(const SubtitleOverlay &other) { return *this; }
	bool bitmapToLPRImage(HDC hdc, HBITMAP hbm, LPRImage **pImagePtr);
	LPRImage* characterImage(wchar_t ch);
private:
	map<wchar_t, LPRImage*> mCharImageMap;	// 字符到图片的映射

	// 字幕参数
	EventFont mFontParam;
};

#endif //SUBTITLEOVERLAY_H
