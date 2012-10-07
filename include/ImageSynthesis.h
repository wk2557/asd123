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
	 * param vsdRect - 第一张图片的部分区域
	 *
	 * return 成功，返回合成的图像指针，这块内存需要调用者自己释放，失败，则返回NULL。
	 */
	LPRImage* synthesis(LPRImage *pRawImages[], const EventImageSynthesis &synthesisParam, const VSDRect &vsdRect);
private:
	/**
	 * 将pSrcImage图像的vsdRect部分生成一个与pSrcImage一样大小的图片。
	 *
	 * param pSrcImage - 输入图像BGR24格式
	 * param vsdRect - 输入图像的矩形部分
	 *
	 * return 成功则返回矩形部分放大的图像，否则返回NULL
	 */
	LPRImage* genFocusImage(const LPRImage *pSrcImage, const VSDRect &vsdRect);
};

#endif
