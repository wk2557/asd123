#ifndef MEDIACONVERTER_H
#define MEDIACONVERTER_H

#define __STDC_CONSTANT_MACROS

#include "EventAPPConstant.h"
#include "EventAPPStructure.h"
#include <LPRVideo.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
}
#include <string>
#include <vector>
#include <stdint.h>

using std::string;
using std::vector;

struct AVFrame;

class MediaConverter
{
public:
	static const PixelFormat DEFAULT_PIX_FMT = PIX_FMT_YUV420P; 
public:
	/**
	 * ����ָ����������һ��ý��ת������
	 * 
	 * param encoderType - ����������ͣ�ȡֵΪECoderID�е�һ����Ŀǰ��֧��MJPEG��H264
	 * param outputFrameRate - ���֡��
	 * param bitRate - ���������
	 */
	MediaConverter(EventAPPViedoFormat encoderType, int outputFrameRate = 10, int bitRate = 1000000);
	virtual ~MediaConverter(void);
	/**
	 * ��ָ����JPG�������У�ת������ý�壨֧������libav֧�ֵ���ý���ʽ��libav֧�ֵ���ý���
	 * ʽ��ο�libav�ĵ�����Ĭ�ϵ���ý���ʽ��avi����������eventMedia�У�eventMedia�еĿ�
	 * ������߱����Լ��ͷš�
	 *
	 * param imgNames - ͼƬ·������
	 * param imgCount - ͼƬ����
	 * param eventMedia - ��ý�建����
	 *
	 * param �ɹ��򷵻�true����ʱ��eventMedia���ã����򷵻�false��eventMedia�����á�
	 */
	bool imgs2media(LPRImage *pRawImages[], size_t imgCount, EventMedia &eventMedia);
private:
	/* ������������ý���ʼ�������ģ��÷��������ڵ����κη���֮ǰ�����ù� */
	bool initialize(const LPRImage *pRawImage, const char *outputMediaName);
	bool initializeInput(const LPRImage *pRawImage);
	bool initializeOutput(const char *outputMediaName);
	void uninitialize();
	void uninitializeInput();
	void uninitializeOutput();
	/* ���֡ */
	bool outputFrame(LPRImage *pRawImage);
	bool flushFrames();
	int64_t nextPTS()
	{
		return mPTS ++;
	}
private:
	// Input related
	AVFrame *mInputFramePtr;
	PixelFormat mInputPixFmt;

	// Output related
	int64_t mPTS;
	int mOutputFrameRate;
	int mOutputBitRate;
	int mOutputWidth;
	int mOutputHeight;
	AVCodecID mOutputCodecId;
	PixelFormat mOutputPixFmt;

	AVFormatContext *mOutputFormatCtxPtr;
	AVStream *mOutputStreamPtr;
	AVCodecContext *mOutputCodecCtxPtr;
	AVFrame *mOutputFramePtr;

	uint8_t *mEncodeBuff;
	int mEncodeBuffSize;

	SwsContext *mSwsContextPtr;

	// ���֡�Ƿ񱻻����ˣ�H264��ʽ�Ỻ��֡
	bool isOutputBuffered;
};

#endif
