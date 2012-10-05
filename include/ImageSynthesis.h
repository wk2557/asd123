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
	 *
	 * return �ɹ������غϳɵ�ͼ��ָ�룬����ڴ���Ҫ�������Լ��ͷţ�ʧ�ܣ��򷵻�NULL��
	 */
	LPRImage* synthesis(LPRImage *pRawImages[], const EventImageSynthesis &synthesisParam);
};

#endif
