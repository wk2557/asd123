#include "SubtitleOverlay.h"
//#include "LPRMutex.h"
//#include "LPRImageDecoder.h"
#include <cassert>

#ifdef WIN32
#include <Windows.h>
#endif

bool __stdcall LPRCopySubImageToLarge( const LPRImage* pImSub, LPRImage* pImLarge, const RECT &rect )
{
	if (!pImLarge || !pImSub)
	{
		return false;
	}
	if (pImSub->nChannels != pImLarge->nChannels ||
		pImSub->depth != pImLarge->depth )
	{
		return false;
	}
	int x = rect.left;
	int y = rect.top;
	int w = rect.right-rect.left;
	int h = rect.bottom-rect.top;

	if( rect.left < 0 || rect.top < 0 || 
		w <= 0 || h <= 0 ||
		w != pImSub->width ||
		h != pImSub->height ||
		rect.right > pImLarge->width ||
		rect.bottom > pImLarge->height )
	{
		return false;
	}

	const unsigned char* pSrc = pImSub->pData;
	unsigned char* pDst = pImLarge->pData + y*pImLarge->step + x * pImLarge->nChannels;
	for (int i = 0; i < h; i++)
	{
		int w3 = w * pImLarge->nChannels;
		for (int j = 0; j < w3; j++)
		{
			pDst[j] = pSrc[j];
		}
		pSrc += pImSub->step;
		pDst += pImLarge->step;
	}

	return true;
}

bool __stdcall LPRCopyLargeRegionToSub(const LPRImage *pImLarge, LPRImage *pImSub, const RECT &rect)
{
	// 进行检查
	if (NULL == pImLarge || NULL == pImSub)
	{
		return false;
	}
	if (pImLarge->depth != pImSub->depth || pImLarge->nChannels != pImLarge->nChannels)
	{
		return false;
	}
	int x = rect.left;
	int y = rect.top;
	int w = rect.right-rect.left;
	int h = rect.bottom-rect.top;
	int nChannels = pImLarge->nChannels;

	if( rect.left < 0 || rect.top < 0 || 
		w <= 0 || h <= 0 ||
		w != pImSub->width ||
		h != pImSub->height ||
		rect.right > pImLarge->width ||
		rect.bottom > pImLarge->height )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const unsigned char* pSrc = pImLarge->pData + y*pImLarge->step + x * nChannels;
	unsigned char* pDst = pImSub->pData;
	for (int i = 0; i < h; ++ i)
	{
		int bytesWidth = w * nChannels;
		for (int j = 0; j < bytesWidth; ++ j)
		{
			pDst[j] = pSrc[j];
		}
		pSrc += pImLarge->step;
		pDst += pImSub->step;
	}

	return true;
}

bool __stdcall LPROverlay(LPRImage* pImForeground, LPRImage* pImBackGround, int x, int y)
{
	/// TODO: 需要检查输入指针是否为空；
	///       需要检查图像大小是否合理（foreground要小一些）、是否越界等问题
	///		  要求输入图像为三通道 pImForeground->nChannels == 3, pImBackGround->nChannels == 3


	int w = pImForeground->width;
	int h = pImForeground->height;

	for (int i = 0; i < h; i++)
	{
		unsigned char* pFG = pImForeground->pData + i * pImForeground->step;
		unsigned char* pBG = pImBackGround->pData + (i+y) * pImBackGround->step + x * 3;
		for (int j = 0; j < w; j++)
		{
			int j3 = j*3;
			int weightFG = (pFG[j3]+pFG[j3+1]+pFG[j3+2]) / 3;	/// 用来作为权重


			if(weightFG <= 3)
			{
				weightFG = 0;
			}
			else if(weightFG >= 30)
			{
				/// 原图足够亮的情况下一定是前景文字
				weightFG = 255;
			}

			pBG[j3]   = ((255-weightFG) * pBG[j3]   + weightFG * pFG[j3]  +127)/255;
			pBG[j3+1] = ((255-weightFG) * pBG[j3+1] + weightFG * pFG[j3+1]+127)/255;
			pBG[j3+2] = ((255-weightFG) * pBG[j3+2] + weightFG * pFG[j3+2]+127)/255;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

#ifdef WIN32 

static LPRImage* __stdcall LPRHBitmapToLPRImage(HDC hdc, HBITMAP hbm)
{
	//////////////////////////////////////////////////////////////////////////
	// 得到hbm中每个像素所占的位数
	int bmPixBitCount = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
	int wBitCount = 0;
	if(bmPixBitCount <=  1)                                                 
		wBitCount = 1;           
	else if(bmPixBitCount <=  4)                             
		wBitCount = 4;           
	else if(bmPixBitCount <=  8)                             
		wBitCount = 8;           
	else                                                                                                                           
		wBitCount = 24;
	//////////////////////////////////////////////////////////////////////////
	BITMAP bm;
	GetObject(hbm, sizeof(bm), &bm);
	int bmRowCount = ((bm.bmWidth * wBitCount + 31) / 32) * 4; // 一行所占的字节数，按四字节对齐
	int bmByteCount =  bmRowCount * bm.bmHeight;
	LPRImage *pImage = LPRCreateImage(bm.bmWidth, bm.bmHeight, 8, 3);
	char *buf = new char[bmByteCount];
	BITMAPINFOHEADER     bi;
	bi.biSize= sizeof(BITMAPINFOHEADER);       
	bi.biWidth = bm.bmWidth;       
	bi.biHeight =  bm.bmHeight;       
	bi.biPlanes =  1;       
	bi.biBitCount = wBitCount;       
	bi.biCompression= BI_RGB;       
	bi.biSizeImage=0;       
	bi.biXPelsPerMeter = 0;       
	bi.biYPelsPerMeter = 0;       
	bi.biClrImportant = 0;       
	bi.biClrUsed =  0;
	GetDIBits(hdc, hbm, 0, bm.bmHeight, buf, (BITMAPINFO*)(&bi), DIB_RGB_COLORS);
	for (int i = 0; i < bm.bmHeight; ++ i)
	{
		for (int j = 0; j < bm.bmWidth; ++ j)
		{
			(pImage)->pData[(bm.bmHeight - i - 1)*bmRowCount + j*3 + 0] = buf[i*bmRowCount + j*3 + 0];
			(pImage)->pData[(bm.bmHeight - i - 1)*bmRowCount + j*3 + 1] = buf[i*bmRowCount + j*3 + 1];
			(pImage)->pData[(bm.bmHeight - i - 1)*bmRowCount + j*3 + 2] = buf[i*bmRowCount + j*3 + 2];
		}

	}
	delete []buf;
	return pImage;
}

static LPRImage* __stdcall LPRGenerateCharacterImage(wchar_t ch, HDC memDC, HBITMAP hbm, int fontSize)
{
	RECT imgRect = {0, 0, fontSize, fontSize};
	SIZE fontPixSize;
	LPRImage *pFontImage = NULL;
	FillRect(memDC, &imgRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	GetTextExtentPoint32(memDC, &ch, 1, &fontPixSize);
	SetTextColor(memDC, RGB(255, 255, 255));	// 文本颜色
	SetBkMode(memDC, TRANSPARENT);				// 背景模式
	TextOut(memDC, (fontSize - fontPixSize.cx) / 2, (fontSize - fontPixSize.cy) / 2, &ch, 1);
	return LPRHBitmapToLPRImage(memDC, hbm);
}


static const wchar_t *DEFAULT_SUBTITLE = L"？0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ京津黑吉辽蒙冀豫鲁鄂沪苏浙皖湘赣粤闽桂琼云贵川渝藏晋陕甘宁青新军空海沈南广成兰济北使警港澳学挂临";
EventSubtitleImages* __stdcall LPRGenerateCharacterImagesDat(const wchar_t *subtitle, int *fontFamilys, int fontFamilysCount, int maxFontSize)
{
	if (fontFamilysCount <= 0)
	{
		printf("You have to supply with at least one font family.\n");
		return NULL;
	}
	// 得到字体名
	wchar_t **fontFamilyNames = new wchar_t*[fontFamilysCount];	// 字符串数组，需要释放数组，常量字符串，不需要释放
	for (int i = 0; i < fontFamilysCount; ++ i)
	{
		switch (fontFamilys[i])
		{
		case EVENT_APP_FONT_SONG:
			fontFamilyNames[i] = L"宋体";
			break;
		case EVENT_APP_FONT_HEI:
			fontFamilyNames[i] = L"黑体";
			break;
		default:
			printf("Unsupported font family: %d.\n", fontFamilys[i]);
			return NULL;
		}
	}
	// 填充结构体
	EventSubtitleImages *pImages = new EventSubtitleImages;	// 返回给调用者，由调用者自己释放
	// 复制字体库
	size_t defaultSubtitleCount = wcslen(DEFAULT_SUBTITLE);
	size_t subtitleCount = wcslen(subtitle);
	size_t upperSubtitleCount = defaultSubtitleCount + subtitleCount;
	wchar_t *upperSubtitle = new wchar_t[upperSubtitleCount + 1];
	wcscpy(upperSubtitle, DEFAULT_SUBTITLE);
	wcscpy(upperSubtitle + defaultSubtitleCount, subtitle);
	wcsupr(upperSubtitle);
	
	// 复制字体家族
	pImages->mFontFamilysCount = fontFamilysCount;
	pImages->mFontFamilys = new int[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
		pImages->mFontFamilys[i] = fontFamilys[i];
	// 生成数组
	//pImages->mWCharImageMapArray = new std::map<wchar_t, LPRImage*>[fontFamilysCount];
	std::map<wchar_t, LPRImage*> *wcharImageMapArray = new std::map<wchar_t, LPRImage*>[fontFamilysCount];

	// 创建字体绘制环境
	HDC memDC = CreateCompatibleDC(NULL);
	// 创建画布
	HBITMAP hbm = CreateBitmap(maxFontSize, maxFontSize, 1, 32, NULL);
	HFONT *hfonts = new HFONT[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
	{

		hfonts[i] = CreateFont(maxFontSize, 0, 0, 0, FW_THIN, false, false, false, 
				DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, 
				DEFAULT_QUALITY, FF_MODERN, fontFamilyNames[i]);
	}
	// 开始生成图片
	//char savePath[256];
	for (int i = 0; i < fontFamilysCount; ++ i)
	{
		std::map<wchar_t, LPRImage*> &wchar2image = wcharImageMapArray[i];
		SelectObject(memDC, hbm);	// 选择画布
		SelectObject(memDC, hfonts[i]);	// 选择字体
		for (int j = 0; j < upperSubtitleCount; ++ j)
		{
			std::map<wchar_t, LPRImage*>::iterator it = wchar2image.find(upperSubtitle[j]);
			if (it == wchar2image.end())
			{
				LPRImage *pImage = LPRGenerateCharacterImage(upperSubtitle[j], memDC, hbm, maxFontSize);
				wchar2image[upperSubtitle[j]] = pImage;
				//if (i == 0)
				/*{
					_snprintf(savePath, 256, "d:\\test_img\\gen_img_%d_%d_%lc.jpg", i, j, upperSubtitle[j]);
					LPRSaveImage(pImage, savePath);
				}*/
			}
		}
	}
	// 将图片和字幕复制
	pImages->mSubtitleCount = wcharImageMapArray[0].size();
	pImages->mSubtitle = new wchar_t[pImages->mSubtitleCount + 1];
	memset(pImages->mSubtitle, 0, (pImages->mFontFamilysCount + 1)*sizeof(wchar_t));
	pImages->mImages = new LPRImage **[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
	{
		pImages->mImages[i] = new LPRImage *[pImages->mSubtitleCount];
		std::map<wchar_t, LPRImage*> &wchar2image = wcharImageMapArray[i];
		int j = 0;
		for (std::map<wchar_t, LPRImage*>::const_iterator it = wchar2image.begin(), e = wchar2image.end(); it != e; ++ it)
		{
			pImages->mSubtitle[j] = it->first;
			pImages->mImages[i][j] = it->second;
			++ j;
		}
		pImages->mSubtitle[pImages->mSubtitleCount] = L'\0';
	}
	//////////////////////////////////////////////////////////////////////////
	// 释放资源
	for (int i = 0; i < fontFamilysCount; ++ i)
		DeleteObject((HGDIOBJ)(hfonts[i]));
	DeleteObject((HGDIOBJ)hbm);
	DeleteDC(memDC);
	delete []wcharImageMapArray;
	delete []upperSubtitle;
	delete []fontFamilyNames;

	return pImages;
}

#endif

 void LPRReleaseSubtitleImages(EventSubtitleImages *pImages)
 {
	 assert(NULL != pImages);
	 /*for (int i = 0; i < pImages->mFontFamilysCount; ++ i)
	 {
		 std::map<wchar_t, LPRImage*> &wchar2image = pImages->mWCharImageMapArray[i];
		 for (std::map<wchar_t, LPRImage*>::iterator it = wchar2image.begin(), e = wchar2image.end(); it != e; ++ it)
			 LPRReleaseImage(it->second);
		 wchar2image.clear();
	 }
	 if (pImages->mWCharImageMapArray != NULL)
	 {
		 delete []pImages->mWCharImageMapArray;
		 pImages->mWCharImageMapArray = NULL;
	 }*/
	 if (pImages->mImages != NULL)
	 {
		 for (int i = 0; i < pImages->mFontFamilysCount; ++ i)
		 {
			 for (int j = 0; j < pImages->mSubtitleCount; ++ j)
				 LPRReleaseImage(pImages->mImages[i][j]);
			 delete []pImages->mImages[i];
		 }
		 delete []pImages->mImages;
		 pImages->mImages = NULL;
	 }
	 if (pImages->mSubtitle != NULL)
	 {
		 delete []pImages->mSubtitle;
		 pImages->mSubtitle = NULL;
	 }
	 if (pImages->mFontFamilys != NULL)
	 {
		 delete []pImages->mFontFamilys;
		 pImages->mFontFamilys = NULL;
	 }
	 pImages->mSubtitleCount = 0;
	 pImages->mFontFamilysCount = 0;
 }

/**
 * 根据指定的宽字符和字体参数得到该字符图片。注意返回的图像必须由调用者释放。
 *
 * param wch - 宽字符
 * param fontParam - 字体参数
 *
 * return 如果成功，宽字符的图片，否则NULL
 */
static LPRImage* __stdcall LPRGetCharacterImage(const EventSubtitleImages *pImages, wchar_t wch, const EventFont &fontParam)
{
	LPRImage *pResultImage = NULL;
	// 得到字体家族索引
	int fontFamilyIndex = -1;
	for (int i = 0; i < pImages->mFontFamilysCount; ++ i)
	{
		if (fontParam.mFontFamily == pImages->mFontFamilys[i])
		{
			fontFamilyIndex = i;
			break;
		}
	}
	if (-1 == fontFamilyIndex)
	{
		printf("Could not find font family %d.\n", fontParam.mFontFamily);
		return NULL;
	}
	// 得到字库索引
	int subtitleIndex = -1;
	for (int i = 0; i < pImages->mSubtitleCount; ++ i)
	{
		if (wch == pImages->mSubtitle[i])
		{
			subtitleIndex = i;
			break;
		}
	}
	if (-1 == subtitleIndex)
	{
		printf("Could not find wchar %lc.\n", wch);
		return NULL;
	}
	//LPRImage *pWCharImage = pImages->mWCharImageMapArray[fontFamilyIndex].find(wch)->second;
	LPRImage *pWCharImage = pImages->mImages[fontFamilyIndex][subtitleIndex];

	// 进行缩放
	float zoomRatio = fontParam.mFontSize/100.0f;
	if (zoomRatio != 1.0)
	{
		int nTargetWidth = (int)(pWCharImage->width * zoomRatio + 0.5f);
		int nTargetHeight = (int)(pWCharImage->height * zoomRatio + 0.5f);
		pResultImage = LPRCreateImage(nTargetWidth, nTargetHeight, pWCharImage->depth, pWCharImage->nChannels);
		LPRResizeImage(pWCharImage, pResultImage);
	}
	else
		pResultImage = LPRCloneImage(pWCharImage);

	// 改变图片颜色
	if (NULL != pResultImage)
	{
		int w = pResultImage->width;
		int h = pResultImage->height;
		unsigned char *pResult = pResultImage->pData;
		for (int i = 0; i < h; ++ i)
		{
			for (int j = 0; j < w; ++ j)
			{
				int j3 = 3*j;
				int totalRGB = pResult[j3 + 0] + pResult[j3 + 1] + pResult[j3 + 2];
				if (totalRGB >= 3*10)	// 现在是黑底白字 BGR24
				{
					pResult[j3 + 0] = fontParam.mFontRGB_B;
					pResult[j3 + 1] = fontParam.mFontRGB_G;
					pResult[j3 + 2] = fontParam.mFontRGB_R;
				}
			}
			pResult += pResultImage->step;
		}
	}

	return pResultImage;
}

/**
  * 给指定的图片在指定的位置添加指定的字幕，并另存为一张新的图片，该方法不会改变原来图片的内容。 
  *
  * param pImBackground - BGR24格式图像
  * param subtitle - 字幕内容，字幕内容中的宽字符必须都在字库中存在。
  * param fontParam - 字幕参数，具体含义请参考EventFont结构体的定义
  * param pImages - 图片库
  *
  * return 成功，则返回生成的图像，图像内存需要使用者自己释放，失败则返回NULL。
  */
static bool __stdcall LPROverlaySubtitle(LPRImage *pImBackground, const wchar_t* subtitle, const EventFont &fontParam, const EventSubtitleImages *pImages)
{
	if (wcslen(subtitle) > 0)
	{
		// 检查所有的字符是否都在字库中
		int wstrSize = wcslen(subtitle);
		wchar_t *upperSubtitle = new wchar_t[wstrSize + 1];
		wcscpy(upperSubtitle, subtitle);
		wcsupr(upperSubtitle);

		// 前提是有字幕图片生成
		/*const std::map<wchar_t, LPRImage*> &wchar2image = pImages->mWCharImageMapArray[0];
		for (int i = 0; i < wstrSize; ++ i)
		{
		if (wchar2image.find(upperSubtitle[i]) == wchar2image.end())
		{
		printf("There is not corresponding image for %lc\n", upperSubtitle[i]);
		return false;
		}
		}*/
		for (int i = 0; i <wstrSize; ++ i)
		{
			if (wcschr(pImages->mSubtitle, upperSubtitle[i]) == NULL)
			{
				printf("There is not corresponding image for %lc\n", upperSubtitle[i]);
				return false;
			}
		}

		/*LPRImage *pImBackground = NULL;
		LPRDecodeImage(&pImBackground, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);*/
		//////////////////////////////////////////////////////////////////////////
		LPRImage *fontImg = LPRGetCharacterImage(pImages, upperSubtitle[0], fontParam);
		int imgWidth = fontImg->width;		// 其实也就是fontSize
		int imgHeight = fontImg->height;		// 其实也就是fontSize
		int imgDepth = fontImg->depth;
		int imgChannels = fontImg->nChannels;
		LPRReleaseImage(fontImg);
		LPRImage *subtitleImg = NULL;
		switch (fontParam.mFontOrientation)
		{
		case EVENT_APP_FONT_HORIZONTAL:
			{
				// 检查是否会越界
				if ((fontParam.mFontX + wstrSize*imgWidth) >= pImBackground->width || (fontParam.mFontY + imgHeight) >= pImBackground->height)
				{
					printf("Out of band.\n");
					LPRReleaseImage(pImBackground);
					return false;
				}
				//////////////////////////////////////////////////////////////////////////
				subtitleImg = LPRCreateImage(wstrSize*imgWidth, imgHeight, imgDepth, imgChannels);
				for (int i = 0;i < wstrSize; ++ i)
				{
					RECT r;
					r.left = i * imgWidth;
					r.top  = 0; 
					r.right = i * imgWidth + imgWidth;
					r.bottom = imgHeight;
					fontImg = LPRGetCharacterImage(pImages, upperSubtitle[i], fontParam);
					LPRCopySubImageToLarge(fontImg, subtitleImg, r);
					LPRReleaseImage(fontImg);
				}
			}
			break;
		case EVENT_APP_FONT_VERTICAL:
			{
				// 检查是否会越界
				if ((fontParam.mFontX + imgWidth) >= pImBackground->width || (fontParam.mFontY + wstrSize*imgHeight) >= pImBackground->height)
				{
					printf("Out of band.\n");
					LPRReleaseImage(pImBackground);
					return false;
				}
				//////////////////////////////////////////////////////////////////////////
				subtitleImg = LPRCreateImage(imgWidth, wstrSize*imgHeight, imgDepth, imgChannels);
				for (int i = 0;i < wstrSize; ++ i)
				{
					RECT r;
					r.left = 0;
					r.top  = i * imgHeight; 
					r.right = imgWidth;
					r.bottom = i * imgHeight + imgHeight;
					fontImg = LPRGetCharacterImage(pImages, upperSubtitle[i], fontParam);
					LPRCopySubImageToLarge(fontImg, subtitleImg, r);
					LPRReleaseImage(fontImg);
				}
			}
			break;
		default:
			{
				printf("Unsupported font orientation.\n");
				LPRReleaseImage(pImBackground);
			}
			return false;
		}
		LPROverlay(subtitleImg, pImBackground, fontParam.mFontX, fontParam.mFontY);
		//////////////////////////////////////////////////////////////////////////
		LPRReleaseImage(subtitleImg);
		delete []upperSubtitle;

		return true;
	}
	return false;
}

//static LPRMutex globalMutex;
//static LPRImageDecoder globalImageDecoder;

LPRImage* __stdcall LPROverlaySubtitle(const LPRImage *pRawImage, const EventSubtitleOverlay &subtitles, const EventSubtitleImages *pImages)
{
	//LPRMutexLocker locker(&globalMutex);	// 加锁，因为多个线程之间共享缓冲区
	assert(subtitles.mSubtitleSize == subtitles.mFontSize);
	LPRImage *pImBackground = NULL;//globalImageDecoder.decode(pRawImage);
	LPRDecodeImage(&pImBackground, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);

	for (int i = 0; i < subtitles.mSubtitleSize; ++ i)
		LPROverlaySubtitle(pImBackground, subtitles.mSubtitles[i], subtitles.mFonts[i], pImages);
	
	return pImBackground;
}