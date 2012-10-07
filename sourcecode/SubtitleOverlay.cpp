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
	// ���м��
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
	/// TODO: ��Ҫ�������ָ���Ƿ�Ϊ�գ�
	///       ��Ҫ���ͼ���С�Ƿ����foregroundҪСһЩ�����Ƿ�Խ�������
	///		  Ҫ������ͼ��Ϊ��ͨ�� pImForeground->nChannels == 3, pImBackGround->nChannels == 3


	int w = pImForeground->width;
	int h = pImForeground->height;

	for (int i = 0; i < h; i++)
	{
		unsigned char* pFG = pImForeground->pData + i * pImForeground->step;
		unsigned char* pBG = pImBackGround->pData + (i+y) * pImBackGround->step + x * 3;
		for (int j = 0; j < w; j++)
		{
			int j3 = j*3;
			int weightFG = (pFG[j3]+pFG[j3+1]+pFG[j3+2]) / 3;	/// ������ΪȨ��


			if(weightFG <= 3)
			{
				weightFG = 0;
			}
			else if(weightFG >= 30)
			{
				/// ԭͼ�㹻���������һ����ǰ������
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
	int bmRowCount = ((bm.bmWidth * wBitCount + 31) / 32) * 4; // һ����ռ���ֽ����������ֽڶ���
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
	SetTextColor(memDC, RGB(255, 255, 255));	// �ı���ɫ
	SetBkMode(memDC, TRANSPARENT);				// ����ģʽ
	TextOut(memDC, (fontSize - fontPixSize.cx) / 2, (fontSize - fontPixSize.cy) / 2, &ch, 1);
	return LPRHBitmapToLPRImage(memDC, hbm);
}

EventSubtitleImages* __stdcall LPRGenerateCharacterImagesDat(wchar_t *subtitle, int *fontFamilys, int fontFamilysCount, int maxFontSize)
{
	EventSubtitleImages *pImages = NULL;
	// �õ�������
	wchar_t **fontFamilyNames = new wchar_t*[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
	{
		switch (fontFamilys[i])
		{
		case EVENT_APP_FONT_SONG:
			fontFamilyNames[i] = L"����";
			break;
		case EVENT_APP_FONT_HEI:
			fontFamilyNames[i] = L"����";
			break;
		default:
			printf("Unsupported font family: %d.\n", fontFamilys[i]);
			return NULL;
		}
	}
	// ���ṹ��
	pImages = new EventSubtitleImages;
	// ���������
	size_t subtitleCount = wcslen(subtitle);
	pImages->mSubtitle = new wchar_t[subtitleCount];
	wcscpy(pImages->mSubtitle, subtitle);
	// �����������
	pImages->mFontFamilysCount = fontFamilysCount;
	pImages->mFontFamilys = new int[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
		pImages->mFontFamilys[i] = fontFamilys[i];
	// ��������
	pImages->mImages = new LPRImage **[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
		pImages->mImages[i] = new LPRImage *[subtitleCount];
	
	// ����������ƻ���
	HDC memDC = CreateCompatibleDC(NULL);
	// ��������
	HBITMAP hbm = CreateBitmap(maxFontSize, maxFontSize, 1, 32, NULL);
	HFONT *hfonts = new HFONT[fontFamilysCount];
	for (int i = 0; i < fontFamilysCount; ++ i)
	{
		hfonts[i] = CreateFont(maxFontSize, 0, 0, 0, FW_THIN, false, false, false, 
				DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, 
				DEFAULT_QUALITY, FF_MODERN, fontFamilyNames[i]);
	}
	// ��ʼ����ͼƬ
	//char savePath[256];
	for (int i = 0; i < fontFamilysCount; ++ i)
	{
		SelectObject(memDC, hbm);	// ѡ�񻭲�
		SelectObject(memDC, hfonts[i]);	// ѡ������
		for (int j = 0; j < subtitleCount; ++ j)
		{
			LPRImage *pImage = LPRGenerateCharacterImage(subtitle[j], memDC, hbm, maxFontSize);
			//_snprintf(savePath, 256, "d:\\gen_img_%d_%d.jpg", i, j);
			//LPRSaveImage(pImage, savePath);
			pImages->mImages[i][j] = pImage;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// �ͷ���Դ
	for (int i = 0; i < fontFamilysCount; ++ i)
		DeleteObject((HGDIOBJ)(hfonts[i]));
	DeleteObject((HGDIOBJ)hbm);
	DeleteDC(memDC);

	return pImages;
}

#endif

/**
 * ����ָ���Ŀ��ַ�����������õ����ַ�ͼƬ��
 *
 * param wch - ���ַ�
 * param fontParam - �������
 *
 * return ����ɹ������ַ���ͼƬ������NULL
 */
static LPRImage* __stdcall LPRGetCharacterImage(const EventSubtitleImages *pImages, wchar_t wch, const EventFont &fontParam)
{
	LPRImage *pResultImage = NULL;
	// �õ������������
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
	// �õ���������
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

	// ��������
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

	// �ı�ͼƬ��ɫ
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
				if (totalRGB >= 3*10)	// �����Ǻڵװ��� BGR24
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
		// ������е��ַ��Ƿ����ֿ���
		int wstrSize = wcslen(subtitle);
		for (int i = 0; i < wstrSize; ++ i)
		{
			if (wcschr(pImages->mSubtitle, subtitle[i]) == NULL)
			{
				printf("Could not find wchar %lc in subtitle.\n", subtitle[i]);
				return false;
			}
		}

		// ��JPG�����н�ѹ��BGR24
		LPRImage *pImBackground = NULL;
		LPRDecodeImage(&pImBackground, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		//////////////////////////////////////////////////////////////////////////
		//wchar_t *wstr = new wchar_t[subtitle.size()];
		LPRImage *fontImg = LPRGetCharacterImage(pImages, subtitle[0], fontParam);
		int imgWidth = fontImg->width;		// ��ʵҲ����fontSize
		int imgHeight = fontImg->height;		// ��ʵҲ����fontSize
		int imgDepth = fontImg->depth;
		int imgChannels = fontImg->nChannels;
		LPRImage *subtitleImg = NULL;
		switch (fontParam.mFontOrientation)
		{
		case EVENT_APP_FONT_HORIZONTAL:
			{
				// ����Ƿ��Խ��
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
				// ����Ƿ��Խ��
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
