#ifndef LPRIMAGEDECODER_H
#define LPRIMAGEDECODER_H

#include <LPRVideo.h>

class LPRImageDecoder
{
public:
	LPRImageDecoder();
	~LPRImageDecoder();
	/**
	 * 解压pRawImage，如果解压之后其宽高与缓存区中图像宽高一致，则重用缓冲区，
	 * 因为缓冲区是由系统维护的，所以不要试图释放该方法返回的图像。
	 *
	 * param pRawImage - 待解压的图像
	 *
	 * return 成功则返回解压之后的图像
	 */
	LPRImage *decode(const LPRImage *pRawImage);
private:
	LPRImage *mImageBuffer;
};

#endif