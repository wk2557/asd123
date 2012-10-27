#ifndef MEDIACONVERTER_H
#define MEDIACONVERTER_H

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
	 * 根据指定参数构造一个媒体转换器。
	 * 
	 * param encoderType - 输出编码类型，取值为ECoderID中的一个，目前仅支持MJPEG和H264
	 * param outputFrameRate - 输出帧率
	 * param bitRate - 输出比特率
	 */
	//MediaConverter(EventAPPVideoFormat encoderType, int outputFrameRate = 10, int bitRate = 1000000);
	MediaConverter(int outputFrameRate = 10, int bitRate = 1000000);
	APPRESULT Init(EventAPPVideoFormat encoderType, int outputFrameRate, int bitRate);
	virtual ~MediaConverter(void);
	/**
	 * 将指定的JPG码流序列，转换成流媒体（支持所有libav支持的流媒体格式，libav支持的流媒体格
	 * 式请参考libav文档），默认的流媒体格式是avi。并保存在eventMedia中，eventMedia中的空
	 * 间调用者必须自己释放。
	 *
	 * param imgNames - 图片路径序列
	 * param imgCount - 图片张数
	 * param eventMedia - 流媒体缓冲区
	 *
	 * param 成功则返回true，这时候eventMedia可用，否则返回false，eventMedia不可用。
	 */
	bool imgs2media(LPRImage *pRawImages[], size_t imgCount, EventMedia &eventMedia);
private:
	/* 根据输入和输出媒体初始化上下文，该方法必须在调用任何方法之前被调用过 */
	bool initialize(const LPRImage *pRawImage, const char *outputMediaName);
	bool initializeInput(const LPRImage *pRawImage);
	bool initializeOutput(const char *outputMediaName);
	void uninitialize();
	void uninitializeInput();
	void uninitializeOutput();
	/* 输出帧 */
	bool outputFrame(LPRImage *pRawImage);
	bool outputFrame();
	bool flushFrames();
	int64_t nextPTS()
	{
		return mPTS ++;
	}
private:
	// Input related
	AVFrame *mInputFramePtr;
	PixelFormat mInputPixFmt;
	AVInputFormat *mInputFmtPtr;

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