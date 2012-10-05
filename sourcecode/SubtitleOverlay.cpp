#include "SubtitleOverlay.h"
#include <Windows.h>


bool LPRCopySubImageToLarge( const LPRImage* pImSub, LPRImage* pImLarge, const RECT &rect )
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

bool LPRCopyLargeRegionToSub(const LPRImage *pImLarge, LPRImage *pImSub, const RECT &rect)
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

bool LPROverlay(LPRImage* pImForeground, LPRImage* pImBackGround, int x, int y)
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
		for (int i = 0; i < w; i++)
		{
			int i3 = i*3;
			int weightFG = (pFG[i3]+pFG[i3+1]+pFG[i3+2]) / 3;	/// 用来作为权重


			if(weightFG <= 3)
			{
				weightFG = 0;
			}
			else if(weightFG >= 30)
			{
				/// 原图足够亮的情况下一定是前景文字
				weightFG = 255;
			}

			pBG[i3]   = ((255-weightFG) * pBG[i3]   + weightFG * pFG[i3]  +127)/255;
			pBG[i3+1] = ((255-weightFG) * pBG[i3+1] + weightFG * pFG[i3+1]+127)/255;
			pBG[i3+2] = ((255-weightFG) * pBG[i3+2] + weightFG * pFG[i3+2]+127)/255;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static int str2wstr(wchar_t *dst, const char *src, int srcSize)
{
	int cchWideChar = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)src, srcSize, 0, 0);
	if(cchWideChar < 0) return false;
	return MultiByteToWideChar(CP_ACP, 0,(LPCSTR)src, srcSize, dst, cchWideChar);
}

//////////////////////////////////////////////////////////////////////////

SubtitleOverlay::~SubtitleOverlay(void)
{
	for(map<wchar_t, LPRImage*>::iterator it = mCharImageMap.begin(), e = mCharImageMap.end(); it != e; ++ it)
	{
		LPRReleaseImage(it->second);
	}
	mCharImageMap.clear();
}

SubtitleOverlay& SubtitleOverlay::getInstance()
{
	static SubtitleOverlay so;
	return so;
}

void SubtitleOverlay::initialize(const wchar_t *wstr, const EventFont &fontParam)
{
	mFontParam = fontParam;
	//////////////////////////////////////////////////////////////////////////
	// 确定图片的大小，确保宽和高是4的倍数
	int fontImageWidth = (mFontParam.mFontSize + 3) / 4 * 4;
	int fontImageHeight = (mFontParam.mFontSize + 3) / 4 * 4;
	// 确定字体家族
	const wchar_t *fontFamilyName;
	switch (fontParam.mFontFamily)
	{
	case EVENT_APP_FONT_SONG:
		fontFamilyName = L"宋体";
		break;
	case EVENT_APP_FONT_HEI:
		fontFamilyName = L"黑体";
		break;
	default:
		printf("Unsupported font family: %d.\n", fontParam.mFontFamily);
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	int wstrSize = wcslen(wstr);
	if (wstrSize < 0)
	{
		printf("字幕叠加模块初始化失败.\n");
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	HDC memDC = CreateCompatibleDC(NULL);
	HBITMAP hbm = CreateBitmap(fontImageWidth, fontImageHeight, 1, 32, NULL);	// 默认黑底
	HGDIOBJ hbmOld = SelectObject(memDC, hbm);
	int fontHeight = mFontParam.mFontSize;//-MulDiv(fontSize, GetDeviceCaps(memDC, LOGPIXELSY), 72);
	HFONT hfont = CreateFont(fontHeight, 0, 0, 0, FW_THIN, false, false, false,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, fontFamilyName);
	HGDIOBJ hfontOld = SelectObject(memDC, hfont);
	COLORREF colorOld = SetTextColor(memDC, mFontParam.mFontColor);
	int bkmodeOld = SetBkMode(memDC, TRANSPARENT);
	//////////////////////////////////////////////////////////////////////////
	RECT imgRect = {0, 0, fontImageWidth, fontImageHeight};
	SIZE fontPixSize;
	LPRImage *pFontImage;
	//char savePath[256];
	for (int i = 0; i < wstrSize; ++ i)
	{
		map<wchar_t, LPRImage*>::const_iterator it = mCharImageMap.find(wstr[i]);
		if (it == mCharImageMap.end())
		{
			FillRect(memDC, &imgRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
			GetTextExtentPoint32(memDC, &wstr[i], 1, &fontPixSize);
			TextOut(memDC, (fontImageWidth - fontPixSize.cx) / 2, (fontImageHeight - fontPixSize.cy) / 2, &wstr[i], 1);
			bitmapToLPRImage(memDC, hbm, &pFontImage);
			//_snprintf(savePath, 256, "d:\\font_image_%d.jpg", i);
			//LPRSaveImage(pFontImage, savePath);
			mCharImageMap[wstr[i]] = pFontImage;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SetBkMode(memDC, bkmodeOld);
	SetTextColor(memDC, colorOld);
	SelectObject(memDC, hfontOld);
	SelectObject(memDC, hbmOld);
	DeleteObject((HGDIOBJ)hbm);
	DeleteObject((HGDIOBJ)hfont);
	DeleteDC(memDC);
	//////////////////////////////////////////////////////////////////////////
}

LPRImage* SubtitleOverlay::overlaySubtitle(LPRImage *pRawImage, const wchar_t* subtitle, const EventFont &fontParam)
{
	if (wcslen(subtitle) > 0)
	{
		//wchar_t *wstr = new wchar_t[subtitle.size()];
		int wstrSize = wcslen(subtitle);
		LPRImage *fontImg = characterImage(subtitle[0]);
		int imgWidth = fontImg->width;
		int imgHeight = fontImg->height;
		int imgDepth = fontImg->depth;
		int imgChannels = fontImg->nChannels;
		LPRImage *subtitleImg = NULL;
		switch (fontParam.mFontOrientation)
		{
		case EVENT_APP_FONT_HORIZONTAL:
			{
				subtitleImg = LPRCreateImage(wstrSize*imgWidth, imgHeight, imgDepth, imgChannels);
				for (int i = 0;i < wstrSize; ++ i)
				{
					RECT r;
					r.left = i * imgWidth;
					r.top  = 0; 
					r.right = i * imgWidth + imgWidth;
					r.bottom = imgHeight;
					fontImg = characterImage(subtitle[i]);
					LPRCopySubImageToLarge(fontImg, subtitleImg, r);
				}
			}
			break;
		case EVENT_APP_FONT_VERTICAL:
			{
				subtitleImg = LPRCreateImage(imgWidth, wstrSize*imgHeight, imgDepth, imgChannels);
				for (int i = 0;i < wstrSize; ++ i)
				{
					RECT r;
					r.left = 0;
					r.top  = i * imgHeight; 
					r.right = imgWidth;
					r.bottom = i * imgHeight + imgHeight;
					fontImg = characterImage(subtitle[i]);
					LPRCopySubImageToLarge(fontImg, subtitleImg, r);
				}
			}
			break;
		default:
			printf("Unsupported font orientation.\n");
			return NULL;
		}
			
		
		// 从JPG码流中解压出BGR24
		LPRImage *pImBackground = NULL;
		LPRDecodeImage(&pImBackground, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		//LPRReleaseImage(pRawImage);	// 释放JPG码流
		//////////////////////////////////////////////////////////////////////////
		LPROverlay(subtitleImg, pImBackground, fontParam.mFontX, fontParam.mFontY);
		//LPRSaveImage(pImBackground, saveAsPath.c_str());
		//////////////////////////////////////////////////////////////////////////
		LPRReleaseImage(subtitleImg);
		//LPRReleaseImage(pImBackground);
		return pImBackground;
	}
	return NULL;
}

bool SubtitleOverlay::bitmapToLPRImage(HDC hdc, HBITMAP hbm, LPRImage **pImagePtr)
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
	int bmRowCount = ((bm.bmWidth * wBitCount + 31) / 32) * 4;
	int bmByteCount =  bmRowCount * bm.bmHeight;	// 四字节对齐
	*pImagePtr = LPRCreateImage(bm.bmWidth, bm.bmHeight, 8, 3);
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
	int lprRowByteCount = bm.bmWidth * 3;
	for (int i = 0; i < bm.bmHeight; ++ i)
	{
		for (int j = 0; j < bm.bmWidth; ++ j)
		{
			(*pImagePtr)->pData[(bm.bmHeight - i - 1)*bmRowCount + j*3] = buf[i*lprRowByteCount + j*3];
			(*pImagePtr)->pData[(bm.bmHeight - i - 1)*bmRowCount + j*3 + 1] = buf[i*lprRowByteCount + j*3 + 1];
			(*pImagePtr)->pData[(bm.bmHeight - i - 1)*bmRowCount + j*3 + 2] = buf[i*lprRowByteCount + j*3 + 2];
		}
	}
	delete []buf;
	return true;
}

LPRImage* SubtitleOverlay::characterImage(wchar_t ch)
{
	map<wchar_t, LPRImage*>::iterator it = mCharImageMap.find(ch);
	if (it != mCharImageMap.end())
		return it->second;
	else
	{
		// 确定图片的大小，确保宽和高是4的倍数
		int fontImageWidth = (mFontParam.mFontSize + 3) / 4 * 4;
		int fontImageHeight = (mFontParam.mFontSize + 3) / 4 * 4;
		// 确定字体家族
		const wchar_t *fontFamilyName;
		switch (mFontParam.mFontFamily)
		{
		case EVENT_APP_FONT_SONG:
			fontFamilyName = L"宋体";
			break;
		case EVENT_APP_FONT_HEI:
			fontFamilyName = L"黑体";
			break;
		default:
			printf("Unsupported font family: %d, using default.\n", mFontParam.mFontFamily);
			fontFamilyName = L"宋体";
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		HDC memDC = CreateCompatibleDC(NULL);
		HBITMAP hbm = CreateBitmap(fontImageWidth, fontImageHeight, 1, 32, NULL);	// 默认黑底
		HGDIOBJ hbmOld = SelectObject(memDC, hbm);
		int fontHeight = mFontParam.mFontSize;//-MulDiv(fontSize, GetDeviceCaps(memDC, LOGPIXELSY), 72);
		HFONT hfont = CreateFont(fontHeight, 0, 0, 0, FW_THIN, false, false, false,
			DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, fontFamilyName);
		HGDIOBJ hfontOld = SelectObject(memDC, hfont);
		COLORREF colorOld = SetTextColor(memDC, mFontParam.mFontColor);
		int bkmodeOld = SetBkMode(memDC, TRANSPARENT);
		//////////////////////////////////////////////////////////////////////////
		RECT imgRect = {0, 0, fontImageWidth, fontImageHeight};
		SIZE fontPixSize;
		LPRImage *pFontImage;
		//char savePath[256];
		//////////////////////////////////////////////////////////////////////////
		FillRect(memDC, &imgRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		GetTextExtentPoint32(memDC, &ch, 1, &fontPixSize);
		TextOut(memDC, (fontImageWidth - fontPixSize.cx) / 2, (fontImageHeight - fontPixSize.cy) / 2, &ch, 1);
		bitmapToLPRImage(memDC, hbm, &pFontImage);
		//_snprintf(savePath, 256, "d:\\font_image_%d.jpg", 1111);
		//LPRSaveImage(pFontImage, savePath);
		mCharImageMap[ch] = pFontImage;
		
		//////////////////////////////////////////////////////////////////////////
		SetBkMode(memDC, bkmodeOld);
		SetTextColor(memDC, colorOld);
		SelectObject(memDC, hfontOld);
		SelectObject(memDC, hbmOld);
		DeleteObject((HGDIOBJ)hbm);
		DeleteObject((HGDIOBJ)hfont);
		DeleteDC(memDC);
		//////////////////////////////////////////////////////////////////////////
		return pFontImage;
	}
}