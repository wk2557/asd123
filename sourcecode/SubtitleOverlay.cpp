#include "SubtitleOverlay.h"


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

EventSubtitleImages* __stdcall LPRGenerateCharacterImagesDat(wchar_t *subtitle, int *fontFamilys, int fontFamilysCount, int maxFontSize)
{
	EventSubtitleImages *pImages = NULL;
	// 得到字体名
	wchar_t **fontFamilyNames = new wchar_t*[fontFamilysCount];
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
	pImages = new EventSubtitleImages;
	// 复制字体库
	size_t subtitleCount = wcslen(subtitle);
	pImages->mSubtitle = new wchar_t[subtitleCount];
	wcscpy(pImages->mSubtitle, subtitle);
	// 复制字体家族
	pImages->mFontFamilysCount = fontFamilysCount;
	pImages->mFontFamilys = new int[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
		pImages->mFontFamilys[i] = fontFamilys[i];
	// 生成数组
	pImages->mImages = new LPRImage **[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
		pImages->mImages[i] = new LPRImage *[subtitleCount];
	
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
		SelectObject(memDC, hbm);	// 选择画布
		SelectObject(memDC, hfonts[i]);	// 选择字体
		for (int j = 0; j < subtitleCount; ++ j)
		{
			LPRImage *pImage = LPRGenerateCharacterImage(subtitle[j], memDC, hbm, maxFontSize);
			//_snprintf(savePath, 256, "d:\\gen_img_%d_%d.jpg", i, j);
			//LPRSaveImage(pImage, savePath);
			pImages->mImages[i][j] = pImage;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 释放资源
	for (int i = 0; i < fontFamilysCount; ++ i)
		DeleteObject((HGDIOBJ)(hfonts[i]));
	DeleteObject((HGDIOBJ)hbm);
	DeleteDC(memDC);

	return pImages;
}

#endif

/**
 * 根据指定的宽字符和字体参数得到该字符图片。
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
	// 得到字体索引
	int subtitleIndex = -1;
	int subtitleCount = wcslen(pImages->mSubtitle);
	for (int i = 0; i < subtitleCount; ++ i)
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

LPRImage* __stdcall LPROverlaySubtitle(LPRImage *pRawImage, const wchar_t* subtitle, const EventFont &fontParam, const EventSubtitleImages *pImages)
{
	if (wcslen(subtitle) > 0)
	{
		// 检查所有的字符是否都在字库中
		int wstrSize = wcslen(subtitle);
		for (int i = 0; i < wstrSize; ++ i)
		{
			if (wcschr(pImages->mSubtitle, subtitle[i]) == NULL)
			{
				printf("Could not find wchar %lc in subtitle.\n", subtitle[i]);
				return false;
			}
		}

		// 从JPG码流中解压出BGR24
		LPRImage *pImBackground = NULL;
		LPRDecodeImage(&pImBackground, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		//////////////////////////////////////////////////////////////////////////
		//wchar_t *wstr = new wchar_t[subtitle.size()];
		LPRImage *fontImg = LPRGetCharacterImage(pImages, subtitle[0], fontParam);
		int imgWidth = fontImg->width;		// 其实也就是fontSize
		int imgHeight = fontImg->height;		// 其实也就是fontSize
		int imgDepth = fontImg->depth;
		int imgChannels = fontImg->nChannels;
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
					return NULL;
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
					fontImg = LPRGetCharacterImage(pImages, subtitle[i], fontParam);
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
					return NULL;
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
					fontImg = LPRGetCharacterImage(pImages, subtitle[i], fontParam);
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
			return NULL;
		}
		LPROverlay(subtitleImg, pImBackground, fontParam.mFontX, fontParam.mFontY);
		//////////////////////////////////////////////////////////////////////////
		LPRReleaseImage(subtitleImg);

		return pImBackground;
	}
	return NULL;
}
