#ifndef SUBTITLEOVERLAY_H
#define SUBTITLEOVERLAY_H

#if defined(WIN32)
#ifdef EVENTAPP_EXPORTS
#define EVENTAPP_API __declspec(dllexport)
#else
#define EVENTAPP_API __declspec(dllimport)
#endif
#elif defined(LINUX)
#ifdef EVENTAPP_EXPORTS
#define EVENTAPP_API __attribute__ ((visibility("default")))
#else
#define EVENTAPP_API
#endif
#else
#error "No platform specified!"
#endif


#include "EventAPPStructure.h"
#include <LPRVideo.h>
#include <string>
#include <map>

using std::map;
using std::string;


/**
 * ��pImSubͼ���Ƶ�pImLargeͼ���rect����
 */
bool __stdcall LPRCopySubImageToLarge(const LPRImage* pImSub, LPRImage* pImLarge, const RECT &rect);
/**
 * ��pImLargeͼ���rect���򿽱���pImSubͼ��
 */
bool __stdcall LPRCopyLargeRegionToSub(const LPRImage *pImLarge, LPRImage *pImSub, const RECT &rect);
/**
 * ��pImForegroundͼ����ӵ�pImBackGroundͼ��(x, y)��ʼ������
 */
bool __stdcall LPROverlay(LPRImage* pImForeground, LPRImage* pImBackGround, int x, int y);

#ifdef WIN32

/**
 * ��ָ�����ֿⰴ�ո����Ĳ�������ͼƬ�����������������صĽ�������ɵ������Լ��ͷ�
 *
 * param subtitle - �ֿ�����
 * param fontFamilys - �����������
 * param fontFamilysCount - �����������Ĵ�С
 * param fontSizes - �����С����
 * param fontSizesCount - �����С����Ĵ�С
 * param savePath - dat�ļ��ı���·��
 *
 * return �ɹ��򷵻ػ����ͼƬ�����򷵻�NULL
 */
EVENTAPP_API EventSubtitleImages*  __stdcall LPRGenerateCharacterImagesDat(const wchar_t *subtitle, int *fontFamilys, int fontFamilysCount, int maxFontSize);

#endif//WIN32

/**
 * �ͷ���ĻͼƬ���档
 */
EVENTAPP_API void LPRReleaseSubtitleImages(EventSubtitleImages *pImages);

/**
  * ��ָ����ͼƬ��ָ����λ�����ָ������Ļ�������Ϊһ���µ�ͼƬ���÷�������ı�ԭ��ͼƬ�����ݡ�
  * 
  *
  * param pRawImage - ��JPG��ʽ�������ͼ��Ŀǰ��֧��JPG�������룩
  * param subtitles - ��Ļ���ݣ����庬��ο�EventSubtitleOverlay�ṹ���塣
  * param pImages - ͼƬ��
  *
  * return �ɹ����򷵻����ɵ�ͼ�񣬷��ص�ͼ���ڴ���Ҫ�������Լ��ͷţ�ʧ���򷵻�NULL��
  */
LPRImage* __stdcall LPROverlaySubtitle(const LPRImage *pRawImage, const EventSubtitleOverlay &subtitles, const EventSubtitleImages *pImages);

#endif //SUBTITLEOVERLAY_H