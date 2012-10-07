#ifndef IMAGESYNTHESIS_H
#define IMAGESYNTHESIS_H

#include "EventAPPStructure.h"
#include <LPRVideo.h>

class ImageSynthesis
{
public:
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
};

#endif
