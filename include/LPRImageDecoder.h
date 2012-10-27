#ifndef LPRIMAGEDECODER_H
#define LPRIMAGEDECODER_H

#include <LPRVideo.h>

class LPRImageDecoder
{
public:
	LPRImageDecoder();
	~LPRImageDecoder();
	/**
	 * ��ѹpRawImage�������ѹ֮�������뻺������ͼ����һ�£������û�������
	 * ��Ϊ����������ϵͳά���ģ����Բ�Ҫ��ͼ�ͷŸ÷������ص�ͼ��
	 *
	 * param pRawImage - ����ѹ��ͼ��
	 *
	 * return �ɹ��򷵻ؽ�ѹ֮���ͼ��
	 */
	LPRImage *decode(const LPRImage *pRawImage);
private:
	LPRImage *mImageBuffer;
};

#endif