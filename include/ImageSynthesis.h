#ifndef IMAGESYNTHESIS_H
#define IMAGESYNTHESIS_H

#include "EventAPPStructure.h"
#include <LPRVideo.h>

class ImageSynthesis
{
public:
	/**
	 * 根据图片序列和合成参数合成一幅大图片。
	 *
	 * param pRawIamges - 图片序列
	 * param synthesisParam - 合成参数，参数具体含义请参考EventImageSynthesis
	 *
	 * return 成功，返回合成的图像指针，这块内存需要调用者自己释放，失败，则返回NULL。
	 */
	LPRImage* synthesis(LPRImage *pRawImages[], const EventImageSynthesis &synthesisParam);
};

#endif
