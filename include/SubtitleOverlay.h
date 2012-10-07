#ifndef SUBTITLEOVERLAY_H
#define SUBTITLEOVERLAY_H

#include "EventAPPStructure.h"
#include <LPRVideo.h>
#include <string>
#include <map>

using std::map;
using std::string;

struct EventSubtitleImages
{
	LPRImage ***mImages;	// ����������ȵĶ�άLPRImage*���飬mImages[i][j]��ʾ�������ΪmFontFamilys[i]���ַ�ΪmSubtitle[j]��ͼƬ
	int *mFontFamilys;		// ���е��������
	int mFontFamilysCount; // ����������
	wchar_t *mSubtitle;		// �����
};

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
 * ��ָ�����ֿⰴ�ո����Ĳ�������ͼƬ��������������
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
EventSubtitleImages* __stdcall LPRGenerateCharacterImagesDat(wchar_t *subtitle, int *fontFamilys, int fontFamilysCount, int maxFontSize);

#endif//WIN32

/**
  * ��ָ����ͼƬ��ָ����λ�����ָ������Ļ�������Ϊһ���µ�ͼƬ���÷�������ı�ԭ��ͼƬ�����ݡ� 
  *
  * param pRawImage - ��JPG��ʽ�������ͼ��Ŀǰ��֧��JPG�������룩
  * param subtitle - ��Ļ���ݣ���Ļ�����еĿ��ַ����붼���ֿ��д��ڡ�
  * param fontParam - ��Ļ���������庬����ο�EventFont�ṹ��Ķ���
  * param pImages - ͼƬ��
  *
  * return �ɹ����򷵻����ɵ�ͼ��ͼ���ڴ���Ҫʹ�����Լ��ͷţ�ʧ���򷵻�NULL��
  */
LPRImage* __stdcall LPROverlaySubtitle(LPRImage *pRawImage, const wchar_t* subtitle, const EventFont &fontParam, const EventSubtitleImages *pImages);

#endif //SUBTITLEOVERLAY_H
