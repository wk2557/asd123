#ifndef IMAGESYNTHESIS_H
#define IMAGESYNTHESIS_H

#include "EventAPPStructure.h"
#include "LPRImageDecoder.h"
//#include "LPRMutex.h"
#include <LPRVideo.h>

class ImageSynthesis
{
public:
	ImageSynthesis();
	~ImageSynthesis();
	/**
	 * ����ͼƬ���кͺϳɲ����ϳ�һ����ͼƬ��
	 *
	 * param pRawIamges - ͼƬ����
	 * param synthesisParam - �ϳɲ������������庬����ο�EventImageSynthesis
	 * param vsdRect - ��һ��ͼƬ�Ĳ�������
	 *
	 * return �ɹ������غϳɵ�ͼ��ָ�룬����ڴ���Ҫ�������Լ��ͷţ�ʧ�ܣ��򷵻�NULL��
	 */
	LPRImage* synthesis(LPRImage *pRawImages[], const EventImageSynthesis &synthesisParam, const VSDRect &vsdRect);
private:
	/**
	 * ��pSrcImageͼ���vsdRect��������һ����pSrcImageһ����С��ͼƬ��
	 *
	 * param pSrcImage - ����ͼ��BGR24��ʽ
	 * param vsdRect - ����ͼ��ľ��β���
	 *
	 * return �ɹ��򷵻ؾ��β��ַŴ��ͼ�񣬷��򷵻�NULL
	 */
	LPRImage* genFocusImage(const LPRImage *pSrcImage, const VSDRect &vsdRect);
	/**
	 * ��ѹpRawImage�������ѹ֮�������뻺������ͼ����һ�£������û�������
	 * ��Ϊ����������ϵͳά���ģ����Բ�Ҫ��ͼ�ͷŸ÷������ص�ͼ��
	 *
	 * param pRawImage - ����ѹ��ͼ��
	 *
	 * return �ɹ��򷵻ؽ�ѹ֮���ͼ��
	 */
	LPRImage* decodeImage(const LPRImage *pRawImage);
	/**
	 * ����ͼ��Ĳ��֡���ߺ������õ�������
	 *
	 * param rect - ͼ���������ڷ��ؽ��
	 * param orien - ͼ�񲼾֣����庬��ο�EventAPPPicOrientation
	 * param w - ͼ���
	 * param h - ͼ���
	 * param imgIndex - ͼ������
	 */
	void calculateRect(RECT &rect, int orien, int w, int h, int imgIndex);
private:
	LPRImageDecoder mImageDecoder;	// ����������ͼ�������
	//LPRMutex mMutex;		// �������ƶ��̰߳�ȫ
};

#endif
