#include "SubtitleOverlay.h"
#include <Windows.h>


bool LPRCopySubImageToLarge( const LPRImage* pImSub, LPRImage* pImLarge, RECT rect )
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

bool LPROverlay(LPRImage* pImForeground, LPRImage* pImBackGround, int x, int y)
{
	/// TODO: ��Ҫ�������ָ���Ƿ�Ϊ�գ�
	///       ��Ҫ���ͼ���С�Ƿ����foregroundҪСһЩ�����Ƿ�Խ�������
	///		  Ҫ������ͼ��Ϊ��ͨ�� pImForeground->nChannels == 3, pImBackGround->nChannels == 3


	int w = pImForeground->width;
	int h = pImForeground->height;

	for (int i = 0; i < h; i++)
	{
		unsigned char* pFG = pImForeground->pData + i * pImForeground->step;
		unsigned char* pBG = pImBackGround->pData + (i+y) * pImBackGround->step + x * 3;
		for (int i = 0; i < w; i++)
		{
			int i3 = i*3;
			int weightFG = (pFG[i3]+pFG[i3+1]+pFG[i3+2]) / 3;	/// ������ΪȨ��


			if(weightFG <= 3)
			{
				weightFG = 0;
			}
			else if(weightFG >= 30)
			{
				/// ԭͼ�㹻���������һ����ǰ������
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

void SubtitleOverlay::initialize(const string &str, int fontSize, int fontImageWidth, int fontImageHeight)
{
	// ȷ����͸���4�ı���
	fontImageWidth = (fontImageWidth + 3) / 4 * 4;
	fontImageHeight = (fontImageHeight + 3) / 4 * 4;
	//////////////////////////////////////////////////////////////////////////
	mFontSize = fontSize;
	mFontImgWidth = fontImageWidth;
	mFontImgHeight = fontImageHeight;
	//////////////////////////////////////////////////////////////////////////
	wchar_t *wstr = new wchar_t[str.size()];	// ���ַ��϶����ᳬ��str.size()
	int wstrSize = str2wstr(wstr, str.c_str(), str.size());
	if (wstrSize < 0)
	{
		printf("��Ļ����ģ���ʼ��ʧ��.\n");
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	HDC memDC = CreateCompatibleDC(NULL);
	HBITMAP hbm = CreateBitmap(fontImageWidth, fontImageHeight, 1, 32, NULL);	// Ĭ�Ϻڵ�
	HGDIOBJ hbmOld = SelectObject(memDC, hbm);
	int fontHeight = fontSize;//-MulDiv(fontSize, GetDeviceCaps(memDC, LOGPIXELSY), 72);
	HFONT hfont = CreateFont(fontHeight, 0, 0, 0, FW_THIN, false, false, false,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, L"����");					// ������壬������Ϊ��������
	HGDIOBJ hfontOld = SelectObject(memDC, hfont);
	COLORREF colorOld = SetTextColor(memDC, RGB(255, 0, 0));	// ������ɫ��������Ϊ��������
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
	delete []wstr;
}

bool SubtitleOverlay::overlaySubtitle(const string &imageFilePath, const string &subtitle, int startX, int startY)
{
	return overlaySubtitle(imageFilePath, imageFilePath, subtitle, startX, startY);
}

bool SubtitleOverlay::overlaySubtitle(const string &imageFilePath, const string &saveAsPath, const string &subtitle, int startX, int startY)
{
	if (subtitle.size() > 0)
	{
		wchar_t *wstr = new wchar_t[subtitle.size()];
		int wstrSize = str2wstr(wstr, subtitle.c_str(), subtitle.size());
		LPRImage *fontImg = characterImage(wstr[0]);
		int imgWidth = fontImg->width;
		int imgHeight = fontImg->height;
		int imgDepth = fontImg->depth;
		int imgChannels = fontImg->nChannels;
		LPRImage *subtitleImg = LPRCreateImage(wstrSize*imgWidth, imgHeight, imgDepth, imgChannels);
		for (int i = 0;i < wstrSize; ++ i)
		{
			RECT r;
			r.left = i * imgWidth;
			r.top  = 0; 
			r.right = i * imgWidth + imgWidth;
			r.bottom = imgHeight;
			fontImg = characterImage(wstr[i]);
			LPRCopySubImageToLarge(fontImg, subtitleImg, r);
		}
		LPRImage* pImBackground = NULL;
		LPRLoadImage(imageFilePath.c_str(), &pImBackground);
		LPROverlay(subtitleImg, pImBackground, startX, startY);
		LPRSaveImage(pImBackground, saveAsPath.c_str());
		//////////////////////////////////////////////////////////////////////////
		LPRReleaseImage(subtitleImg);
		LPRReleaseImage(pImBackground);
		delete []wstr;
		return true;
	}
	return false;
}

bool SubtitleOverlay::overlaySubtitle(LPRImage *pRawImage, const string &saveAsPath, const string &subtitle, int startX, int startY)
{
	if (subtitle.size() > 0)
	{
		wchar_t *wstr = new wchar_t[subtitle.size()];
		int wstrSize = str2wstr(wstr, subtitle.c_str(), subtitle.size());
		LPRImage *fontImg = characterImage(wstr[0]);
		int imgWidth = fontImg->width;
		int imgHeight = fontImg->height;
		int imgDepth = fontImg->depth;
		int imgChannels = fontImg->nChannels;
		LPRImage *subtitleImg = LPRCreateImage(wstrSize*imgWidth, imgHeight, imgDepth, imgChannels);
		for (int i = 0;i < wstrSize; ++ i)
		{
			RECT r;
			r.left = i * imgWidth;
			r.top  = 0; 
			r.right = i * imgWidth + imgWidth;
			r.bottom = imgHeight;
			fontImg = characterImage(wstr[i]);
			LPRCopySubImageToLarge(fontImg, subtitleImg, r);
		}
		// ��JPG�����н�ѹ��BGR24
		LPRImage *pImBackground = NULL;
		LPRDecodeImage(&pImBackground, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		LPRReleaseImage(pRawImage);	// �ͷ�JPG����
		//////////////////////////////////////////////////////////////////////////
		LPROverlay(subtitleImg, pImBackground, startX, startY);
		LPRSaveImage(pImBackground, saveAsPath.c_str());
		//////////////////////////////////////////////////////////////////////////
		LPRReleaseImage(subtitleImg);
		LPRReleaseImage(pImBackground);
		delete []wstr;
		return true;
	}
	return false;
}

bool SubtitleOverlay::bitmapToLPRImage(HDC hdc, HBITMAP hbm, LPRImage **pImagePtr)
{
	//////////////////////////////////////////////////////////////////////////
	// �õ�hbm��ÿ��������ռ��λ��
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
	int bmByteCount =  bmRowCount * bm.bmHeight;	// ���ֽڶ���
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
		//////////////////////////////////////////////////////////////////////////
		HDC memDC = CreateCompatibleDC(NULL);
		HBITMAP hbm = CreateBitmap(mFontImgWidth, mFontImgHeight, 1, 32, NULL);	// Ĭ�Ϻڵ�
		HGDIOBJ hbmOld = SelectObject(memDC, hbm);
		int fontHeight = mFontSize;//-MulDiv(fontSize, GetDeviceCaps(memDC, LOGPIXELSY), 72);
		HFONT hfont = CreateFont(fontHeight, 0, 0, 0, FW_THIN, false, false, false,
			DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, L"����");					// ������壬������Ϊ��������
		HGDIOBJ hfontOld = SelectObject(memDC, hfont);
		COLORREF colorOld = SetTextColor(memDC, RGB(255, 0, 0));	// ������ɫ��������Ϊ��������
		int bkmodeOld = SetBkMode(memDC, TRANSPARENT);
		//////////////////////////////////////////////////////////////////////////
		RECT imgRect = {0, 0, mFontImgWidth, mFontImgHeight};
		SIZE fontPixSize;
		LPRImage *pFontImage;
		//char savePath[256];
		//////////////////////////////////////////////////////////////////////////
		FillRect(memDC, &imgRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		GetTextExtentPoint32(memDC, &ch, 1, &fontPixSize);
		TextOut(memDC, (mFontImgWidth - fontPixSize.cx) / 2, (mFontImgHeight - fontPixSize.cy) / 2, &ch, 1);
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