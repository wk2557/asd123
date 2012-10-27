#ifndef IMAGESYNTHESIS_H
#define IMAGESYNTHESIS_H

#include "EventAPPStructure.h"
#include "LPRImageDecoder.h"
#include "LPRMutex.h"
#include <LPRVideo.h>

class ImageSynthesis
{
public:
	ImageSynthesis();
	~ImageSynthesis();
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
	/**
	 * 解压pRawImage，如果解压之后其宽高与缓存区中图像宽高一致，则重用缓冲区，
	 * 因为缓冲区是由系统维护的，所以不要试图释放该方法返回的图像。
	 *
	 * param pRawImage - 待解压的图像
	 *
	 * return 成功则返回解压之后的图像
	 */
	LPRImage* decodeImage(const LPRImage *pRawImage);
	/**
	 * 根据图像的布局、宽高和索引得到其区域。
	 *
	 * param rect - 图像区域，用于返回结果
	 * param orien - 图像布局，具体含义参考EventAPPPicOrientation
	 * param w - 图像宽
	 * param h - 图像高
	 * param imgIndex - 图像索引
	 */
	void calculateRect(RECT &rect, int orien, int w, int h, int imgIndex);
private:
	LPRImageDecoder mImageDecoder;	// 共享缓冲区的图像解码器
	LPRMutex mMutex;		// 锁，控制多线程安全
};

#endif
