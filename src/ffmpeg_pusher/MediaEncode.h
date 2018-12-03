#pragma once

struct AVFrame;
struct AVCodecContext;
struct AVPacket;

class MediaEncode
{
public:
	//������������
	AVCodecContext *vc = 0;	

	//�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;

	//�������
	int outWidth = 1280;
	int outHeight = 720;
	int fps = 25;
	//ѹ����ÿ����Ƶ��bitλ��С 50kB
	int bitrate = 4000000;

	//������������
	static MediaEncode *Get(unsigned char index = 0);

	virtual AVFrame* RgbToYuv(char *rgb) = 0;

	//�ر���Դ
	virtual void Close() = 0;

	//��ʼ�����ظ�ʽת����������
	virtual bool InitScale() = 0;

	//��ʼ����Ƶ������
	virtual bool InitVideoCodec() = 0;

	//��Ƶ����
	virtual AVPacket *EncodeVideo(AVFrame *frame) = 0;

	virtual ~MediaEncode();
protected:
	MediaEncode();
};