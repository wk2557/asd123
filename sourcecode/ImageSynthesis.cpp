#include "ImageSynthesis.h"
#include "SubtitleOverlay.h"
#include <cassert>

LPRImage* ImageSynthesis::synthesis(LPRImage *pRawImages[], const EventImageSynthesis &synthesisParam)
{
	assert(synthesisParam.mNumberofImage > 0);
	// TODO 检查图像宽高一致
	// 先解码JPG流得到BGR24的图像
	LPRImage **pImages = new LPRImage *[synthesisParam.mNumberofImage];
	for (int i = 0; i < synthesisParam.mNumberofImage; ++ i)
	{
		LPRImage *pImage = NULL;
		LPRDecodeImage(&pImage, pRawImages[i]->pData, pRawImages[i]->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		pImages[i] = pImage;
		LPRReleaseImage(pRawImages[i]);
	}
	
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
			largeImgHeight = synthesisParam.mNumberofImage * littleImgHeight;
			pLargeImg = LPRCreateImage(largeImgWidth, largeImgHeight, littleImgDepth, littleImgChannels);
			for (int i = 0; i < synthesisParam.mNumberofImage; ++ i)
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
			largeImgWidth = synthesisParam.mNumberofImage * littleImgWidth;
			largeImgHeight = littleImgHeight;
			pLargeImg = LPRCreateImage(largeImgWidth, largeImgHeight, littleImgDepth, littleImgChannels);
			for (int i = 0; i < synthesisParam.mNumberofImage; ++ i)
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
			if (synthesisParam.mNumberofImage % 2 == 0)
			{
				largeImgWidth = 2 * littleImgWidth;
				largeImgHeight = synthesisParam.mNumberofImage/2 * littleImgHeight;
				pLargeImg = LPRCreateImage(largeImgWidth, largeImgHeight, littleImgDepth, littleImgChannels);
				for (int i = 0; i < synthesisParam.mNumberofImage; ++ i)
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
	// 释放小图
	for (int i = 0; i < synthesisParam.mNumberofImage; ++ i)
		LPRReleaseImage(pImages[i]);

	return pLargeImg;
}
