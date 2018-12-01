#include <iostream>
using namespace std;
using namespace cv;

extern "C" {
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <opencv2/highgui.hpp>
#pragma comment(lib,"opencv_world320.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib,"avcodec.lib")



int main(int argc, char *argv[])
{
	//���������rtsp url
	char *inUrl = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
	VideoCapture cam;
	namedWindow("video");
	//if (cam.open(0))

	//���ظ�ʽת�������Ķ���
	SwsContext *vsc = NULL;

	//��������ݽṹ
	AVFrame *yuv = NULL;

	//������������
	AVCodecContext *vc = NULL;
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

		if(!vsc)
		{
			throw exception("sws_getCachedContext failed!");
		}

		///��ʼ����������ݽṹ
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = inWidth;
		yuv->height = inHeight;
		yuv->pts = 0;
		int result = av_frame_get_buffer(yuv, 32);
		if (result!= 0)
		{
			char arr[1024] = { 0 };
			av_strerror(result, arr, sizeof(arr));
			throw exception(arr);
		}

		///��ʼ���������
		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if(!codec)
		{
			throw exception("find h264 encoder failed");
		}

		vc = avcodec_alloc_context3(codec);
		if(!vc)
		{
			throw exception("avcodec_alloc_context3 failed!");
		}

		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		vc->codec_id = codec->id;
		vc->thread_count = 8;
		//ѹ����ÿ����Ƶ��bitλ��С 50kB
		vc->bit_rate = 50 * 1024 * 8;
		vc->width = inWidth;
		vc->height = inHeight;
		vc->time_base = { 1,fps };
		vc->framerate = { fps,1 };
		//������Ĵ�С������֡һ���ؼ�֡
		vc->gop_size = 50;
		//��Ҫb֡
		vc->max_b_frames = 0;
		vc->pix_fmt = AV_PIX_FMT_YUV420P;

		result = avcodec_open2(vc, 0, 0);
		if (result != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(result, buf, sizeof(buf) - 1);
			throw exception(buf);
		}

		cout << "avcodec_open2 success!" << endl;

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

			///rgb to yuv
			uint8_t * srcSlice[AV_NUM_DATA_POINTERS] = {0};
			//indata[0] bgrbgrbgr
			//plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr 
			srcSlice[0] = frame.data;
			int srcStride[AV_NUM_DATA_POINTERS] = {0};
			//һ�У������ݵ��ֽ���
			srcStride[0] = frame.cols*frame.elemSize();
			int h = sws_scale(vsc,
					srcSlice,
					srcStride,
					0,
					frame.rows,
					yuv->data,
					yuv->linesize
					);
			if (h < 0)
			{
				continue;
			}
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