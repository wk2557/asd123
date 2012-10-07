#include "ImageSynthesis.h"
#include "SubtitleOverlay.h"
#include <cassert>

LPRImage* ImageSynthesis::synthesis(LPRImage *pRawImages[], const EventImageSynthesis &synthesisParam, const VSDRect &vsrRect)
{
	assert(synthesisParam.mNumberofImage > 0);
	// TODO ���ͼ����һ��
	// �Ƚ���JPG���õ�BGR24��ͼ��
	LPRImage **pImages = new LPRImage *[synthesisParam.mNumberofImage + 1];
	for (int i = 0; i < synthesisParam.mNumberofImage; ++ i)
	{
		LPRImage *pImage = NULL;
		LPRDecodeImage(&pImage, pRawImages[i]->pData, pRawImages[i]->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		pImages[i] = pImage;
		//LPRReleaseImage(pRawImages[i]);
	}
	pImages[synthesisParam.mNumberofImage] = genFocusImage(pImages[0], vsrRect);
	//////////////////////////////////////////////////////////////////////////
	// ȡ�þֲ�ͼ��֮��ͼ�������
	int imageCount = synthesisParam.mNumberofImage + 1;
	int littleImgWidth = pImages[0]->width;
	int littleImgHeight = pImages[0]->height;
	int littleImgDepth = pImages[0]->depth;
	int littleImgChannels = pImages[0]->nChannels;
	int largeImgWidth = 0;
	int largeImgHeight = 0;
	LPRImage *pLargeImg = NULL;
	switch (synthesisParam.mPicOrientation)
	{
	case EVENT_APP_PIC_VERTICAL:
		{
			largeImgWidth = littleImgWidth;
			largeImgHeight = imageCount * littleImgHeight;
			pLargeImg = LPRCreateImage(largeImgWidth, largeImgHeight, littleImgDepth, littleImgChannels);
			for (int i = 0; i < imageCount; ++ i)
			{
				RECT r;
				r.left = 0;
				r.top  = i * littleImgHeight; 
				r.right = r.left + littleImgWidth;
				r.bottom = r.top + littleImgHeight;
				LPRCopySubImageToLarge(pImages[i], pLargeImg, r);
			}
			//char savePath[256];
			//_snprintf(savePath, 256, "d:\\syn_image_v.jpg");
			//LPRSaveImage(pLargeImg, savePath);
		}
		break;
	case EVENT_APP_PIC_HORIZONTAL:
		{
			largeImgWidth = imageCount * littleImgWidth;
			largeImgHeight = littleImgHeight;
			pLargeImg = LPRCreateImage(largeImgWidth, largeImgHeight, littleImgDepth, littleImgChannels);
			for (int i = 0; i < imageCount; ++ i)
			{
				RECT r;
				r.left = i * littleImgWidth;
				r.top  = 0; 
				r.right = r.left + littleImgWidth;
				r.bottom = r.top + littleImgHeight;
				LPRCopySubImageToLarge(pImages[i], pLargeImg, r);
			}
			//char savePath[256];
			//_snprintf(savePath, 256, "d:\\syn_image_h.jpg");
			//LPRSaveImage(pLargeImg, savePath);
		}
		break;
	case EVENT_APP_PIC_GRID:
		{
			if (imageCount % 2 == 0)
			{
				largeImgWidth = 2 * littleImgWidth;
				largeImgHeight = imageCount/2 * littleImgHeight;
				pLargeImg = LPRCreateImage(largeImgWidth, largeImgHeight, littleImgDepth, littleImgChannels);
				for (int i = 0; i < imageCount; ++ i)
				{
					RECT r;
					r.left = (i%2) * littleImgWidth;
					r.top  = (i/2) *littleImgHeight; 
					r.right = r.left + littleImgWidth;
					r.bottom = r.top + littleImgHeight;
					LPRCopySubImageToLarge(pImages[i], pLargeImg, r);
				}
				//char savePath[256];
				//_snprintf(savePath, 256, "d:\\syn_image_g.jpg");
				//LPRSaveImage(pLargeImg, savePath);
			}
			else
			{
				printf("Unsupported synthesis option grid with the odd number of images.\n");
				return NULL;
			}
		}
		break;
	default:
		printf("Unsupported image orientation.\n");
		return NULL;
	}
	// ��������˴�ͼ��������Ҫ��������
	if (NULL != pLargeImg && synthesisParam.mZoomRatio != 1.0)
	{
		int nTargetWidth = (int)(pLargeImg->width * synthesisParam.mZoomRatio + 0.5f);
		int nTargetHeight = (int)(pLargeImg->height * synthesisParam.mZoomRatio + 0.5f);
		LPRImage* pImTextResized = LPRCreateImage(nTargetWidth, nTargetHeight, pLargeImg->depth, pLargeImg->nChannels);
		LPRResizeImage(pLargeImg, pImTextResized);
		LPRReleaseImage(pLargeImg);
		pLargeImg = pImTextResized;
	}
	// �ͷ�Сͼ
	for (int i = 0; i < imageCount; ++ i)
		LPRReleaseImage(pImages[i]);

	return pLargeImg;
}

LPRImage* ImageSynthesis::genFocusImage(const LPRImage *pSrcImage, const VSDRect &vsdRect)
{
	assert(pSrcImage != NULL);
	int depth = pSrcImage->depth;
	int nChannels = pSrcImage->nChannels;
	// �ȴ���һ����ԭͼ����ͬ��С��ͼ��
	LPRImage *pResultImage = LPRCreateImage(pSrcImage->width, pSrcImage->height, depth, nChannels);
	memset(pResultImage->pData, 0, pResultImage->imageSize);
	// ����focus���ֵ�ͼ��
	LPRImage *pFocusImage = LPRCreateImage(vsdRect.width, vsdRect.height, depth, nChannels);
	RECT focusRect;
	focusRect.left = vsdRect.x;
	focusRect.top = vsdRect.y;
	focusRect.right = vsdRect.x + vsdRect.width;
	focusRect.bottom = vsdRect.y + vsdRect.height;
	LPRCopyLargeRegionToSub(pSrcImage, pFocusImage, focusRect);
	//LPRSaveImage(pFocusImage, "d:\\focus.jpg");
	//////////////////////////////////////////////////////////////////////////
	// ��ʼ�Ŵ�focusͼ��
	// ���ȼ����߱� w1/h1 = w2/h2 => w1*h2 = w2*h1��Ϊ�˱����������64λ����
	int srcFocusDelta = int64_t(pSrcImage->width)*int64_t(pFocusImage->height) - int64_t(pSrcImage->height)*int64_t(pFocusImage->width);
	if (srcFocusDelta != 0)	// ��߱Ȳ����ʱ����Ҫ�Ŵ�Ȼ����
	{
		int startCopyX = 0;
		int startCopyY = 0;
		int resizedImageWidth = 0;
		int resizedImageHeight = 0;
		LPRImage *pResizedImage = NULL;
		if (srcFocusDelta > 0)	// ԭͼ��Ŀ�߱ȸ������ո����Ŵ�ͼ��
		{
			resizedImageWidth = int(pFocusImage->width*(double(pSrcImage->height)/double(pFocusImage->height)));
			resizedImageHeight = pSrcImage->height;
			startCopyX = (pSrcImage->width - resizedImageWidth)/2;
			startCopyY = 0;
			pResizedImage = LPRCreateImage(resizedImageWidth, resizedImageHeight, depth, nChannels);
			LPRResizeImage(pFocusImage, pResizedImage);
			//LPRSaveImage(pResizedImage, "d:\\resized.jpg");
		}
		else if (srcFocusDelta < 0)	// ԭͼ��Ŀ�߱ȸ�С�����տ����Ŵ�ͼ��
		{
			resizedImageWidth = pSrcImage->width;
			resizedImageHeight = int(pFocusImage->height*(double(pSrcImage->width)/double(pFocusImage->width)));
			startCopyX = 0;
			startCopyY = (pSrcImage->height - resizedImageHeight)/2;
			pResizedImage = LPRCreateImage(resizedImageWidth, resizedImageHeight, depth, nChannels);
			LPRResizeImage(pFocusImage, pResizedImage);
		}
		// ��ʼ����
		RECT copyRect;
		copyRect.left = startCopyX;
		copyRect.top = startCopyY;
		copyRect.right = startCopyX + resizedImageWidth;
		copyRect.bottom = startCopyY + resizedImageHeight;
		LPRCopySubImageToLarge(pResizedImage, pResultImage, copyRect);
		LPRReleaseImage(pResizedImage);
	}
	else // ��߱���ȣ���ֱ�������ͼ������
	{
		LPRResizeImage(pFocusImage, pResultImage);
	}
	//LPRSaveImage(pResultImage, "d:\\res.jpg");

	LPRReleaseImage(pFocusImage);

	return pResultImage;
}