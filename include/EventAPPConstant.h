#ifndef __EVENT_APP_CONSTANT_H_
#define __EVENT_APP_CONSTANT_H_

enum EventAPPFontFamily
{
	EVENT_APP_FONT_SONG			= 0,           // ����
	EVENT_APP_FONT_HEI			= 1,           // ����
};

enum EventAPPFontOrientation
{
	EVENT_APP_FONT_VERTICAL		= 0,		   // ���崹ֱ�ڷ�
	EVENT_APP_FONT_HORIZONTAL	= 1,		   // ����ˮƽ�ڷ�
};

enum EventAPPPicOrientation
{
	EVENT_APP_PIC_VERTICAL		= 0,		   // ԭʼͼƬ��ֱ�źϳ��µ�ͼƬ
	EVENT_APP_PIC_HORIZONTAL	= 1,		   // ˮƽ�źϳ��µ�ͼƬ
	EVENT_APP_PIC_GRID			= 2,		   // �����͵ĺϳ��µ�ͼƬ
};

enum EventAPPLightStatus
{
	EVENT_APP_LIGHT_RED			= 0,			// ���
	EVENT_APP_LIGHT_GREEN		= 1,			// �̵�
	EVENT_APP_LIGHT_YELLOW		= 2,			// �Ƶ�
};

enum EventAPPVideoFormat
{
	EVENT_APP_VIDEO_MJPG		= 0,			// �ϳ���Ƶ�ĸ�ʽΪMJPG
	EVENT_APP_VIDEO_H264		= 1,			// �ϳ���Ƶ�ĸ�ʽΪH264
};

enum EventAPPRuleSwitch
{
	EVENT_APP_RULE_SWITCH_OFF         = 0,
	EVENT_APP_RULE_SWITCH_ON		= 1,
};

enum EventAPPPlateCaptureSwitch
{
	EVENT_APP_PLATE_SWITCH_OFF      = 0,
	EVENT_APP_PLATE_SWITCH_ON     = 1,
};

typedef int APPRESULT;

#define APP_OK						     0		// �ɹ�

#define APP_FAIL						-1		// ʧ�ܣ�δ֪ԭ��
#define APP_INPUT_NULL_POINTER			-2		// ʧ�ܣ�ԭ������Ϊ��ָ��
#define APP_OUT_OF_MEMORY				-3		// ʧ�ܣ�ԭ���ڴ����ʧ��
#define APP_FILE_NOT_FOUND				-4		// ʧ�ܣ�ԭ����û�ҵ������ļ�
#define APP_CONFIG_PARAM_FAULT			-5		// ʧ�ܣ�ԭ���������ļ��������ʽ����
#define APP_VSD_PARAM_FAULT				-6		// ʧ�ܣ�ԭ����VSD���ò�������
#define APP_IMAGE_FORMAT_FAULT			-7		// ʧ�ܣ�ԭ���������ļ��ĸ�ʽ����
#define APP_LPR_NOT_INITED				-8      // ʧ�ܣ�ԭ����LPR��δ��ʼ��


#define RULE_TYPES						10		// Υ������ĸ�����������VSD_BR_NONE
#define MAX_FRAME_AHEAD					40		// Υ��ǰ�����¼��֡��
#define MAX_FRAME_BEHIND					20		// Υ�������¼��֡��
#define DEFAULT_PLATE_MIN_WIDTH			70		// ���Ƶ���С��ȣ�������ʶ����
#define DEFAULT_PLATE_MAX_WIDTH			300		// ���Ƶ�����ȣ�������ʶ����
#define DEFAULT_NUMBER_PLATE			1		// ���Ƶ���Ŀ��������ʶ����
#define MAX_CHARACTOR_NUM				100     // ��Ļ�������ַ���Ŀ
#define MAX_SYNTHESIS_NUM				3		// �ϳɴ�ͼƬ��ԭͼƬ�����Ŀ
#define DEFAULT_EXTRA_IMAGE_BUFFER		0	

#endif