#include "MediaConverter.h"
#include <stdio.h>
#include <cassert>

MediaConverter::MediaConverter(EventAPPVideoFormat encoderType, int outputFrameRate, int bitRate) 
	: mInputFramePtr(NULL), 
	mInputPixFmt(PIX_FMT_NONE), 
	mPTS(0), 
	mOutputFrameRate(outputFrameRate),
	mOutputBitRate(bitRate),
	mOutputWidth(-1),
	mOutputHeight(-1), 
	//mOutputCodecId(AVCodecID(encoderType)), 
	mOutputPixFmt(PIX_FMT_NONE), 
	mOutputFormatCtxPtr(NULL),
	mOutputStreamPtr(NULL), 
	mOutputCodecCtxPtr(NULL), 
	mOutputFramePtr(NULL), 
	mEncodeBuff(NULL),
	mEncodeBuffSize(-1),
	mSwsContextPtr(NULL),
	isOutputBuffered(false)
{
	static bool libavInited = false;
	if (!libavInited)
	{
		av_register_all();
		libavInited = false;
	}
	switch (encoderType)
	{
	case EVENT_APP_VIDEO_MJPG:
		mOutputCodecId = AV_CODEC_ID_MJPEG;
		break;
	case EVENT_APP_VIDEO_H264:
		mOutputCodecId = AV_CODEC_ID_H264;
		break;
	default:
		break;
	}
}

MediaConverter::~MediaConverter(void)
{
	uninitialize();
}

bool MediaConverter::imgs2media(LPRImage *pRawImages[], size_t imgCount, EventMedia &eventMedia)
{
	if (imgCount == 0)
	{
		printf("Input is empty.\n");
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	if(!initialize(pRawImages[0], "temp.avi"))
	{
		printf("Failed to initialize.\n");
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	if (!(mOutputFormatCtxPtr->flags & AVFMT_NOFILE))
	{
		/*if (avio_open(&mOutputFormatCtxPtr->pb, mediaName.c_str(), AVIO_FLAG_WRITE) < 0)
		{
		printf("Could not open %s.\n", mediaName.c_str());
		return false;
		}*/
		if (avio_open_dyn_buf(&mOutputFormatCtxPtr->pb) < 0)
		{
			printf("Could not open avio buff.\n");
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// Output
	avformat_write_header(mOutputFormatCtxPtr, NULL);
	for (size_t i = 0; i < imgCount; ++ i)
		outputFrame(pRawImages[i]);
	flushFrames();
	av_write_trailer(mOutputFormatCtxPtr);
	//////////////////////////////////////////////////////////////////////////
	if (!(mOutputFormatCtxPtr->flags & AVFMT_NOFILE))
	{
		//avio_close(mOutputFormatCtxPtr->pb);
		eventMedia.mBufferSize = avio_close_dyn_buf(mOutputFormatCtxPtr->pb, &eventMedia.mBufferPtr);
	}
	//////////////////////////////////////////////////////////////////////////
	// 清理环境
	uninitialize();

	return true;
}

bool MediaConverter::initialize(const LPRImage *pRawImage, const char *outputMediaName)
{
	return initializeInput(pRawImage) && initializeOutput(outputMediaName);
}

bool MediaConverter::initializeInput(const LPRImage *pRawImage)
{
	LPRImage *pImage = NULL;
	LPRDecodeImage(&pImage, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
	mOutputWidth = pImage->width;
	mOutputHeight = pImage->height;
	LPRReleaseImage(pImage);
	// pRawImage现在还没有输出到视频，还不能释放

	mInputFramePtr = avcodec_alloc_frame();
	return true;
}

bool MediaConverter::initializeOutput(const char *outputMediaName)
{
	// 初始化输出格式
	AVOutputFormat *pOutputFormat = av_guess_format(NULL, outputMediaName, NULL);
	if (NULL == pOutputFormat)
	{
		printf("Could not deduce output format from file extension: using AVI.\n");
		pOutputFormat = av_guess_format("avi", NULL, NULL);
	}
	if (NULL == pOutputFormat)
	{
		printf("Could not find suitable output format.\n");
		return false;
	}
	pOutputFormat->video_codec = mOutputCodecId;
	// Allocate the output media context
	mOutputFormatCtxPtr = avformat_alloc_context();
	if (NULL == mOutputFormatCtxPtr)
	{
		printf("Could not allocate format context.\n");
		return false;
	}
	mOutputFormatCtxPtr->oformat = pOutputFormat;
	strcpy_s(mOutputFormatCtxPtr->filename, sizeof(mOutputFormatCtxPtr->filename), outputMediaName);
	// Find the video encoder
	AVCodec *pOutputCodec = avcodec_find_encoder(pOutputFormat->video_codec);
	if (NULL == pOutputCodec)
	{
		printf("Could not find encoder.\n");
		return false;
	}
	// Open the output stream
	mOutputStreamPtr = avformat_new_stream(mOutputFormatCtxPtr, pOutputCodec);
	if (NULL == mOutputStreamPtr)
	{
		printf("Could not allocate stream.\n");
		return false;
	}
	// Set codec context params
	mOutputCodecCtxPtr = mOutputStreamPtr->codec;
	mOutputCodecCtxPtr->bit_rate = mOutputBitRate;
	mOutputCodecCtxPtr->width = mOutputWidth;
	mOutputCodecCtxPtr->height = mOutputHeight;
	mOutputCodecCtxPtr->time_base.den = mOutputFrameRate;
	mOutputCodecCtxPtr->time_base.num = 1;
	mOutputCodecCtxPtr->gop_size = 12;
	if (NULL != pOutputCodec->pix_fmts)
	{
		mOutputPixFmt = pOutputCodec->pix_fmts[0];
		mOutputCodecCtxPtr->pix_fmt = mOutputPixFmt;
	}
	else
	{
		mOutputPixFmt = DEFAULT_PIX_FMT;
		mOutputCodecCtxPtr->pix_fmt = mOutputPixFmt;
	}
	if (mOutputCodecCtxPtr->codec_id == AV_CODEC_ID_MPEG1VIDEO) 
	{
        /* Needed to avoid using macroblocks in which some coeffs overflow.
         * This does not happen with normal video, it just happens here as
         * the motion of the chroma plane does not match the luma plane. */
        mOutputCodecCtxPtr->mb_decision = 2;
    }
    /* Some formats want stream headers to be separate. */
    if (mOutputFormatCtxPtr->oformat->flags & AVFMT_GLOBALHEADER)
        mOutputFormatCtxPtr->flags |= CODEC_FLAG_GLOBAL_HEADER;

	// Open the video encoder
	if (avcodec_open2(mOutputCodecCtxPtr, NULL, NULL) < 0)
	{
		printf("Could not open encoder.\n");
		return false;
	}
	// Allocate output frame
	mOutputFramePtr = avcodec_alloc_frame();
	int frameBuffSize = avpicture_get_size(mOutputPixFmt, mOutputWidth, mOutputHeight);
	uint8_t *frameBuff = (uint8_t*)av_malloc_array(frameBuffSize, sizeof(uint8_t));
	avpicture_fill((AVPicture*)mOutputFramePtr, frameBuff, mOutputPixFmt, mOutputWidth, mOutputHeight);
	// Allocate encode buff
	mEncodeBuffSize = 400000;
	mEncodeBuff = (uint8_t *)av_malloc_array(mEncodeBuffSize, sizeof(uint8_t));

	return true;
}

void MediaConverter::uninitialize()
{
	uninitializeInput();
	uninitializeOutput();
}

void MediaConverter::uninitializeInput()
{
	if (NULL != mInputFramePtr)
	{
		//av_free(mInputFramePtr);
		avcodec_free_frame(&mInputFramePtr);
		mInputFramePtr = NULL;
	}
}

void MediaConverter::uninitializeOutput()
{
	if (NULL != mOutputCodecCtxPtr)
	{
		avcodec_close(mOutputCodecCtxPtr);
		mOutputCodecCtxPtr = NULL;
	}
	if (NULL != mOutputFramePtr)
	{
		av_free(mOutputFramePtr->data[0]);
		//av_free(mOutputFramePtr);
		avcodec_free_frame(&mOutputFramePtr);
		mOutputFramePtr = NULL;
	}
	if (NULL != mOutputStreamPtr)
	{
		av_freep(&mOutputStreamPtr->codec);
		av_freep(&mOutputStreamPtr);
		mOutputStreamPtr = NULL;
	}
	if (NULL != mOutputFormatCtxPtr)
	{
		av_free(mOutputFormatCtxPtr);
		//avformat_free_context(mOutputFormatCtxPtr);
		mOutputFormatCtxPtr = NULL;
	}
	if (NULL != mEncodeBuff)
	{
		av_free(mEncodeBuff);
		mEncodeBuff = NULL;
	}
	if (NULL != mSwsContextPtr)
	{
		sws_freeContext(mSwsContextPtr);
		mSwsContextPtr = NULL;
	}
	mPTS = 0;
	isOutputBuffered = false;
}

bool MediaConverter::outputFrame(LPRImage *pRawImage)
{
	// 先解码JPG码流
	LPRImage *pImage = NULL;
	LPRDecodeImage(&pImage, pRawImage->pData, pRawImage->imageSize, LPR_ENCODE_FORMAT_JPG, 0);
	//LPRReleaseImage(pRawImage);
	//////////////////////////////////////////////////////////////////////////
	// 从pImage中拷贝数据
	mSwsContextPtr = sws_getCachedContext(mSwsContextPtr, pImage->width, pImage->height, PIX_FMT_BGR24, 
		mOutputWidth, mOutputHeight, mOutputPixFmt, SWS_BICUBIC, NULL, NULL, NULL);
	if (NULL == mSwsContextPtr)
	{
		printf("Could not initialize the conversion context.\n");
		return false;
	}
	avpicture_fill((AVPicture*)mInputFramePtr, pImage->pData, PIX_FMT_BGR24, pImage->width, pImage->height);
	int ret = sws_scale(mSwsContextPtr, mInputFramePtr->data, mInputFramePtr->linesize, 0, mOutputHeight, mOutputFramePtr->data, mOutputFramePtr->linesize);
	assert(ret != 0);
	LPRReleaseImage(pImage);	// 释放BGR24码流
	mOutputFramePtr->pts = nextPTS();
	//////////////////////////////////////////////////////////////////////////
	int got_packet = 0;
	AVPacket packet;
	av_init_packet(&packet);
	packet.data = mEncodeBuff;
	packet.size = mEncodeBuffSize;
	ret = avcodec_encode_video2(mOutputCodecCtxPtr, &packet, mOutputFramePtr, &got_packet);
	//printf("outsize=%d getpacket=%d packetsize=%d stream_index=%d\n", ret, got_packet, packet.size, packet.stream_index);
	if (got_packet == 1)
	{
		if (packet.pts != AV_NOPTS_VALUE)
			packet.pts = av_rescale_q(packet.pts, mOutputCodecCtxPtr->time_base, mOutputStreamPtr->time_base);
		if (packet.dts != AV_NOPTS_VALUE)
			packet.dts = av_rescale_q(packet.dts, mOutputCodecCtxPtr->time_base, mOutputStreamPtr->time_base);
		if (mOutputCodecCtxPtr->coded_frame->key_frame)
			packet.pts |= AV_PKT_FLAG_KEY;

		int ret = av_interleaved_write_frame(mOutputFormatCtxPtr, &packet);
		//printf("after pts=%d dts=%d ret=%d\n", (int)packet.pts, (int)packet.dts, ret);
		return 0 == ret;
	}
	else
		isOutputBuffered = true;
	return true;
}

bool MediaConverter::flushFrames()
{
	if (isOutputBuffered)
	{
		int got_packet = 0;
		AVPacket packet;
		av_init_packet(&packet);
		while(true)
		{
			packet.data = mEncodeBuff;
			packet.size = mEncodeBuffSize;
			int ret = avcodec_encode_video2(mOutputCodecCtxPtr, &packet, NULL, &got_packet);
			//printf("flush outsize=%d getpacket=%d packetsize=%d stream_index=%d\n", ret, got_packet, packet.size, packet.stream_index);
			if (got_packet == 1)
			{
				if (packet.pts != AV_NOPTS_VALUE)
					packet.pts = av_rescale_q(packet.pts, mOutputCodecCtxPtr->time_base, mOutputStreamPtr->time_base);
				if (packet.dts != AV_NOPTS_VALUE)
					packet.dts = av_rescale_q(packet.dts, mOutputCodecCtxPtr->time_base, mOutputStreamPtr->time_base);
				if (mOutputCodecCtxPtr->coded_frame->key_frame)
					packet.pts |= AV_PKT_FLAG_KEY;
				ret = av_interleaved_write_frame(mOutputFormatCtxPtr, &packet);
				//printf("after pts=%d dts=%d ret=%d\n", (int)packet.pts, (int)packet.dts, ret);
				if (ret != 0)
					return false;
			}
			else
				break;
		}
	}
	return true;
}