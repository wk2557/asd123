#ifndef MEDIACONVERTER_H
#define MEDIACONVERTER_H

#define __STDC_CONSTANT_MACROS

#include "EventAPPConstant.h"
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
	 * 根据指定参数构造一个媒体转换器。
	 * 
	 * param encoderType - 输出编码类型，取值为ECoderID中的一个，目前仅支持MJPEG和H264
	 * param outputFrameRate - 输出帧率
	 * param bitRate - 输出比特率
	 */
	MediaConverter(EventAPPViedoFormat encoderType, int outputFrameRate = 10, int bitRate = 1000000);
	virtual ~MediaConverter(void);
	/**
	 * 将指定的图片序列（支持所有libav支持的图片格式，libav支持的图片格式请参考libav文档），
	 * 转换成流媒体（支持所有libav支持的流媒体格式，libav支持的流媒体格式请参考libav文档），
	 * 默认的流媒体格式是avi。
	 *
	 * param imgNames - 图片路径序列
	 * param mediaName - 流媒体路径
	 */
	bool imgs2media(const vector<string> &imgNames, const string &mediaName);
	/**
	 * 将指定的JPG码流序列，转换成流媒体（支持所有libav支持的流媒体格式，libav支持的流媒体格
	 * 式请参考libav文档），默认的流媒体格式是avi。
	 *
	 * param imgNames - 图片路径序列
	 * param mediaName - 流媒体路径
	 */
	bool imgs2media(LPRImage *pRawImages[], size_t imgCount, const string &mediaName);
private:
	/* 根据输入和输出媒体初始化上下文，该方法必须在调用任何方法之前被调用过 */
	bool initialize(const char *inputMediaName, const char *outputMediaName);
	bool initialize(const LPRImage *pRawImage, const char *outputMediaName);
	bool initializeInput(const LPRImage *pRawImage);
	bool initializeInput(const char *inputMediaName);
	bool initializeOutput(const char *outputMediaName);
	void uninitialize();
	void uninitializeInput();
	void uninitializeOutput();
	/* 转换媒体 */
	bool convertMedia(const char *inputMediaName);
	/* 输出帧 */
	bool outputFrame();
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

	// 输出帧是否被缓存了，H264格式会缓存帧
	bool isOutputBuffered;
};

#endif
