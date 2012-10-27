#include "LPRImageDecoder.h"
#include <cassert>

LPRImageDecoder::LPRImageDecoder() : mImageBuffer(NULL)
{
}

LPRImageDecoder::~LPRImageDecoder()
{
	if (NULL != mImageBuffer)
	{
		LPRReleaseImage(mImageBuffer);
		mImageBuffer = NULL;
	}
}

LPRImage* LPRImageDecoder::decode(const LPRImage *pRawImage)
{
	assert(NULL != pRawImage);

	if (mImageBuffer != NULL)
	{
		int oldImageSize = mImageBuffer->imageSize;
		LPRDecodeImage(&mImageBuffer, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		if (oldImageSize != mImageBuffer->imageSize)
		{
			LPRReleaseImage(mImageBuffer);
			mImageBuffer = NULL;
			LPRDecodeImage(&mImageBuffer, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
		}
	}
	else
	{
		LPRDecodeImage(&mImageBuffer, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
	}
	return mImageBuffer;
}