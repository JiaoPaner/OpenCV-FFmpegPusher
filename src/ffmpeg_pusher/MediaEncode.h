#pragma once

struct AVFrame;

class MediaEncode
{
public:
	//�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;

	//�������
	int outWidth = 1280;
	int outHeight = 720;

	//������������
	static MediaEncode *Get(unsigned char index = 0);

	virtual AVFrame* RgbToYuv(char *rgb) = 0;

	//��ʼ�����ظ�ʽת����������
	virtual bool InitScale() = 0;

	//�ر�������
	virtual void Close() = 0;

	virtual ~MediaEncode();
protected:
	MediaEncode();
};