#include <iostream>
using namespace std;

extern "C" {
#include <libswscale/swscale.h>
}

#include <opencv2/highgui.hpp>
#pragma comment(lib,"opencv_world320.lib")
#pragma comment(lib, "swscale.lib")
using namespace cv;


int main(int argc, char *argv[])
{
	//���������rtsp url
	char *inUrl = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
	VideoCapture cam;
	namedWindow("video");
	//if (cam.open(0))

	//���ظ�ʽת�������Ķ���
	SwsContext *vsc = NULL;
	try
	{
		///opencv����
		cam.open(inUrl);
		if (!cam.isOpened())
		{
			throw exception("camera open failed");
		}
		cout << inUrl << " cam open success" << endl;
		//��ȡframe�����Ϣ
		int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = cam.get(CAP_PROP_FPS);

		///��ʼ�����ظ�ʽת��������
		vsc = sws_getCachedContext(vsc,
			//����ʹ��opencv�򿪵��������ĸ�ʽ��AV_PIX_FMT_BGR24����������Ҫע��
			inWidth,inHeight,AV_PIX_FMT_BGR24,
			//Ŀ��Ŀ�ߺ����ظ�ʽ
			inWidth,inHeight, AV_PIX_FMT_YUV420P,
			//�ߴ�仯�㷨
			SWS_BICUBIC,
			0,0,0
			);
		Mat frame;
		for (;;) {
			///��ȡrtsp��Ƶ֡��������
			//cam.read(frame); read��
			//����������grab��retrieve����
			if (!cam.grab()) {
				continue;
			}
			///yuvתrgb
			if (!cam.retrieve(frame)) {
				continue;
			}
			imshow("video", frame);
			waitKey(1);
		}
	}
	catch (const std::exception& ex)
	{
		if (cam.isOpened())
		{
			cam.release();
		}
		if (vsc)
		{
			sws_freeContext(vsc);
			//ָ����NULL��һ�����õ�ϰ��
			vsc = NULL;
		}
		cerr << ex.what()<< endl;
	}
	
	return 0;
}

//����ͷ�ļ�
//extern "C" {
//#include "libavformat/avformat.h"
//#include "libavutil/time.h"
//}
//
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"avcodec.lib")

//int XError(int errorNum) {
//	char buf[2048] = { 0 };
//	av_strerror(errorNum, buf, sizeof(buf));
//	cout << buf << endl;
//	getchar();
//	return -1;
//}

//static double r2d(AVRational r) {
//	return r.den == 0 || r.num == 0 ? 0. : (double)r.num / (double)r.den;
//}


//int main2(int argc, char *argv[]) {
//	char *input = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
//
//
//	//ע�⣬���ip��ַһ��Ҫ�����nginx������ipһ�£�����avio_open�ǻ�������
//	char *output = "rtmp://192.168.1.108/live";
//	//ע�����еķ�װ���װ��
//	av_register_all();
//
//	//��ʼ������
//	avformat_network_init();
//	//**********************************������
//	//���ļ�
//	AVFormatContext *ictx = NULL;
//
//	//���ò���
//	AVDictionary *opts = NULL;
//	char key[] = "max_delay";
//	char val[] = "500";
//	av_dict_set(&opts, key, val, 0);
//
//	char key2[] = "rtsp_transport";
//	char val2[] = "tcp";
//	av_dict_set(&opts, key2, val2, 0);
//
//	int result = avformat_open_input(&ictx, input, 0, &opts);
//	if (result != 0) {
//		return XError(result);
//	}
//
//	cout << "open file "<<input <<" success"<< endl;
//	result = avformat_find_stream_info(ictx, 0);
//	if (result != 0) {
//		return XError(result);
//	}
//
//	//��ӡ��Ƶ�ļ��ĸ�ʽ��Ϣ
//	av_dump_format(ictx, 0, input, 0);
//
//	//**********************************�����
//
//	AVFormatContext *octx = NULL;
//	//���������������
//	avformat_alloc_output_context2(&octx, 0, "flv", output);
//	if (!octx)
//	{
//		return XError(result);
//	}
//	//���������
//	//���������AVStream
//	for (int i = 0; i < ictx->nb_streams; i++){
//		//���������
//		AVStream *out = avformat_new_stream(octx, ictx->streams[i]->codec->codec);
//		if (!out) {
//			return XError(0);
//		}
//		//����������Ϣ mp4
//		//result = avcodec_copy_context(out->codec, ictx->streams[i]->codec);
//		result = avcodec_parameters_copy(out->codecpar, ictx->streams[i]->codecpar);
//		out->codec->codec_tag = 0;
//	}
//
//	av_dump_format(octx, 0, output, 1);
//	
//	//rtmp����
//	//��io
//	result = avio_open(&octx->pb, output, AVIO_FLAG_WRITE);
//	cout << "avio_open " << result << endl;
//	if (!octx->pb) {
//		return XError(result);
//	}
//	cout << "avio_open" << endl;
//	//д��ͷ��Ϣ
//	result = avformat_write_header(octx, 0);
//	if (result < 0) {
//		return XError(result);
//	}
//
//	cout << "avformat_write_header����ÿһ֡����" << endl;
//	//����ÿһ֡����
//	AVPacket pkt;
//	long long startTime = av_gettime();
//	for (;;) {
//		result = av_read_frame(ictx, &pkt);
//		if (result != 0 || pkt.size <=0) {
//			continue;
//		}
//		//����ת��ʱ���pts dts
//		AVRational itime = ictx->streams[pkt.stream_index]->time_base;
//		AVRational otime = octx->streams[pkt.stream_index]->time_base;
//		pkt.pts = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
//		pkt.dts = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
//		pkt.duration = av_rescale_q_rnd(pkt.duration, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
//		pkt.pos = -1;
//		
//		//��Ƶ֡�����ٶ�
//		//if (ictx->streams[pkt.stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//		//	//��ȡ��Ƶ��ʱ���
//		//	AVRational rational = ictx->streams[pkt.stream_index]->time_base;
//		//	//�ӿ�ʼ���뵽���ڹ�ȥ��ʱ��,������Ϊ�������е�ʱ�䣬�õ�ǰ֡��pts���Աȣ�����ͬ��
//		//	long long now = av_gettime() - startTime;
//		//	long long dts = 0;
//		//	//��λ΢��
//		//	dts = pkt.dts*r2d(rational)*1000*1000;
//
//		//	//˵������̫��,��һ��
//		//	if (dts > now) {
//		//		av_usleep(dts - now);
//		//		cout << "�ȴ�"<< dts - now<< endl;
//		//	}
//		//	else {
//		//		cout << "����ȴ�" << dts - now << endl;
//		//	}
//		//}
//
//		result = av_interleaved_write_frame(octx, &pkt);
//	
//		if (result < 0) {
//			//return XError(result);
//		}
//
//		//av_packet_unref(&pkt);
//	}
//	cout << "test "<< endl;
//	getchar();
//	return 0;
//}