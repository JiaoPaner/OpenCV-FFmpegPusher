
#include "MediaCapture.h"
#include <opencv2/highgui.hpp>
#include <iostream>

#pragma comment(lib,"opencv_world320.lib")
using namespace std;

class MediaCaptureImpl :public MediaCapture
{
public:
	cv::VideoCapture cam;

	//��ʼ�����
	bool MediaCapture::Init(int cameraIndex)
	{
		///opencv����
		cam.open(cameraIndex);
		if (!cam.isOpened())
		{
			cout << "cam open failed!" << endl;
			return false;
		}
		cout << cameraIndex << " cam open success" << endl;
		//��ȡ�������Ϣ
		width = cam.get(cv::CAP_PROP_FRAME_WIDTH);
		height = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
		fps = cam.get(cv::CAP_PROP_FPS);
		//fps�п��ܻ�ȡ����
		if (fps == 0) 
			fps = 25;

		return true;
	}

	//��ʼ��������
	bool MediaCapture::Init(const char *url)
	{
		cam.open(url);
		if (!cam.isOpened())
		{
			cout << "cam open failed!" << endl;
			return false;
		}
		cout << url << " cam open success" << endl;
		width = cam.get(cv::CAP_PROP_FRAME_WIDTH);
		height = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
		fps = cam.get(cv::CAP_PROP_FPS);
		if (fps == 0) 
			fps = 25;
		return true;
	}

	//ֹͣ
	void MediaCapture::Stop()
	{
		DataThread::Stop();
		if (cam.isOpened())
		{
			cam.release();
		}
	}

	void Main()
	{
		cout << "�����߳�"<< endl;
		cv::Mat frame;
		while (!isExit)
		{
			if (!cam.read(frame))
			{
				Sleep(1);
				continue;
			}
			if (frame.empty())
			{
				Sleep(1);
				continue;
			}
			//ȷ�������������� 
			Data d((char*)frame.data, frame.cols*frame.rows*frame.elemSize());
			Push(d);
		}
		cout << "�˳��߳�" << endl;
	}

};

MediaCapture *MediaCapture::Get(unsigned char index) 
{
	static MediaCaptureImpl vc[255];
	return &vc[index];
}

MediaCapture::MediaCapture()
{

}

MediaCapture::~MediaCapture()
{

}
