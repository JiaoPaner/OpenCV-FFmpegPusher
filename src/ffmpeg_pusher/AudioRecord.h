#pragma once
#include "DataThread.h"

enum AUDIOTYPE
{
	AUDIO_QT
};

class AudioRecord :public DataThread
{
public:
	//������
	int channels = 2;	
	//������
	int sampleRate = 44100;	
	//�����ֽڴ�С
	int sampleByte = 2;		
	//һ֡��Ƶ����ͨ������������
	int nbSamples = 1024;	
	//��ʼ¼��
	virtual bool Init() = 0;
	//ֹͣ¼��
	virtual void Stop() = 0;

	static AudioRecord *Get(AUDIOTYPE type = AUDIO_QT, unsigned char index = 0);
	virtual ~AudioRecord();
protected:
	AudioRecord();
};
