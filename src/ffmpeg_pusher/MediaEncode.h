#pragma once

struct AVFrame;
struct AVCodecContext;
struct AVPacket;

enum SampleFmt
{
	S16 = 1,
	FLATP = 8
};

class MediaEncode
{
public:
	//��Ƶ������������
	AVCodecContext *vc = 0;
	//��Ƶ������������
	AVCodecContext *ac = 0;

	//�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;
	int inChannels = 2;
	SampleFmt inSampleFmt = S16;
	int inSampleRate = 44100;


	//�������
	int outWidth = 1280;
	int outHeight = 720;
	int fps = 25;
	int bitrate = 4000000;
	int nbSample = 1024;
	SampleFmt outSmapleFmt = FLATP;

	//������������
	static MediaEncode *Get(unsigned char index = 0);

	virtual AVFrame* RgbToYuv(char *rgb) = 0;

	//�ر���Դ
	virtual void Close() = 0;

	//��ʼ�����ظ�ʽת����������
	virtual bool InitScale() = 0;

	//��Ƶ�ز��������ĳ�ʼ��
	virtual bool InitResample() = 0;

	//�ز��� 
	virtual AVFrame *Resample(char *data) = 0;

	//��ʼ����Ƶ������
	virtual bool InitVideoCodec() = 0;

	//��Ƶ�����ʼ��
	virtual bool InitAudioCodec() = 0;

	//��Ƶ����
	virtual AVPacket *EncodeVideo(AVFrame *frame) = 0;

	//��Ƶ���� 
	virtual AVPacket * EncodeAudio(AVFrame* frame) = 0;

	virtual ~MediaEncode();
protected:
	MediaEncode();
};