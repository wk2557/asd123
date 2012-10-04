#ifndef SUBTITLEOVERLAY_H
#define SUBTITLEOVERLAY_H

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
	 * 将指定字库中的字按照给定大小生成在指定大小的图片中，并缓存在图片中。为了效率起见，
	 * 请确保字库中的字已经覆盖了以后将要用到的所有字。
	 *
	 * param str - 字库
	 * param fontSize - 字体大小，以像素为单位
	 * param fontImageWidth - 生成图片的宽度，以像素为单位
	 * param fontImageHeight - 生成图片的高度，以像素为单位
	 */
	void initialize(const string &str, int fontSize, int fontImageWidth, int fontImageHeight);
	/**
	 * 给指定的图片在指定的位置添加指定的字幕。 
	 *
	 * param imageFilePath - 图片路径（支持LPRImage支持的所有图片类型）
	 * param subtitle - 字幕内容
	 * param startX - 字幕开始的x坐标，采用屏幕坐标系
	 * param startY - 字幕开始的y坐标，采用屏幕坐标系
	 */
	bool overlaySubtitle(const string &imageFilePath, const string &subtitle, int startX = 0, int startY = 0);
	/**
	 * 给指定的图片在指定的位置添加指定的字幕，并另存为一张新的图片，该方法不会改变原来图片的内容。 
	 *
	 * param imageFilePath - 图片路径（支持LPRImage支持的所有图片类型）
	 * param saveAsPath - 图片另存为路径
	 * param subtitle - 字幕内容
	 * param startX - 字幕开始的x坐标，采用屏幕坐标系
	 * param startY - 字幕开始的y坐标，采用屏幕坐标系
	 */
	bool overlaySubtitle(const string &imageFilePath, const string &saveAsPath, const string &subtitle, int startX = 0, int startY = 0);
	/**
	 * 给指定的图片在指定的位置添加指定的字幕，并另存为一张新的图片，该方法不会改变原来图片的内容。 
	 *
	 * param pRawImage - 以JPG格式编码过的图像（目前仅支持JPG码流输入）
	 * param saveAsPath - 图片另存为路径
	 * param subtitle - 字幕内容
	 * param startX - 字幕开始的x坐标，采用屏幕坐标系
	 * param startY - 字幕开始的y坐标，采用屏幕坐标系
	 */
	bool overlaySubtitle(LPRImage *pRawImage, const string &saveAsPath, const string &subtitle, int startX = 0, int startY = 0);
private:
	SubtitleOverlay(void) {}
	SubtitleOverlay(const SubtitleOverlay &other) {}
	SubtitleOverlay& operator=(const SubtitleOverlay &other) { return *this; }
	bool bitmapToLPRImage(HDC hdc, HBITMAP hbm, LPRImage **pImagePtr);
	LPRImage* characterImage(wchar_t ch);
private:
	map<wchar_t, LPRImage*> mCharImageMap;	// 字符到图片的映射

	// 字体相关
	int mFontSize;
	int mFontImgWidth;
	int mFontImgHeight;
};

#endif //SUBTITLEOVERLAY_H
