#ifndef SUBTITLEOVERLAY_H
#define SUBTITLEOVERLAY_H

#include "EventAPPStructure.h"
#include <LPRVideo.h>
#include <string>
#include <map>

using std::map;
using std::string;

bool LPRCopySubImageToLarge(const LPRImage* pImSub, LPRImage* pImLarge, RECT rect);
bool LPROverlay(LPRImage* pImForeground, LPRImage* pImBackGround, int x, int y);

class SubtitleOverlay
{
public:
	virtual ~SubtitleOverlay(void);
	static SubtitleOverlay& getInstance();
	/**
	 * ��ָ���ֿ��е��ְ��ո����Ĳ�������ͼƬ�������档Ϊ��Ч�������
	 * ��ȷ���ֿ��е����Ѿ��������Ժ�Ҫ�õ��������֡�
	 *
	 * param str - �ֿ�
	 * param fontParam - ��Ļ���������庬����ο�EventFont�ṹ��Ķ���
	 */
	void initialize(const string &str, const EventFont &fontParam);
	/**
	 * ��ָ����ͼƬ��ָ����λ�����ָ������Ļ�������Ϊһ���µ�ͼƬ���÷�������ı�ԭ��ͼƬ�����ݡ� 
	 *
	 * param pRawImage - ��JPG��ʽ�������ͼ��Ŀǰ��֧��JPG�������룩
	 * param subtitle - ��Ļ����
	 * param fontParam - ��Ļ���������庬����ο�EventFont�ṹ��Ķ���
	 *
	 * return �ɹ����򷵻����ɵ�ͼ��ͼ���ڴ���Ҫʹ�����Լ��ͷţ�ʧ���򷵻�NULL��
	 */
	LPRImage* overlaySubtitle(LPRImage *pRawImage, const string &subtitle, const EventFont &fontParam);
private:
	SubtitleOverlay(void) {}
	SubtitleOverlay(const SubtitleOverlay &other) {}
	SubtitleOverlay& operator=(const SubtitleOverlay &other) { return *this; }
	bool bitmapToLPRImage(HDC hdc, HBITMAP hbm, LPRImage **pImagePtr);
	LPRImage* characterImage(wchar_t ch);
private:
	map<wchar_t, LPRImage*> mCharImageMap;	// �ַ���ͼƬ��ӳ��

	// ��Ļ����
	EventFont mFontParam;
};

#endif //SUBTITLEOVERLAY_H
