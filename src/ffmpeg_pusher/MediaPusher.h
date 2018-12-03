#pragma once

struct AVCodecContext;
struct AVPacket;

class MediaPusher {
public:

	//������������
	static MediaPusher *Get(unsigned char index = 0);

	//��ʼ����װ��������
	virtual bool Init(const char *url) = 0;

	//�����
	virtual bool AddStream(const AVCodecContext *actx) = 0;

	//��rtmp����IO�����ͷ�װͷ
	virtual bool OpenIO() = 0;

	//����
	virtual bool SendPacket(AVPacket *pkt) = 0;

	//�ر���Դ
	virtual void Close() = 0;

	virtual ~MediaPusher();
protected:
	MediaPusher();
};