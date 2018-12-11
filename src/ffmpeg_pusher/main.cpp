#include <iostream>
using namespace std;

#include "MediaEncoder.h"
#include "MediaPusher.h"
#include "MediaCapture.h"

int main(int argc, char *argv[])
{

	//rtsp url������һ�����õĲ�������ַ
	char *inUrl = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
	//nginx-rtmp ֱ��������rtmp����URL(192.168.1.106�����������ip,ȷ������������)
	char *outUrl = "rtmp://192.168.43.24/live";

	MediaCapture *capture = MediaCapture::Get();
	if (!capture->Init(inUrl))
	{
		cout << "open camera failed!" << endl;
		getchar();
		return -1;
	}
	cout << "open camera success!" << endl;
	capture->Start();

	MediaEncoder *encode = MediaEncoder::Get();
	MediaPusher *pusher = MediaPusher::Get();

	try
	{
		//������
		encode->inChannels = 2;
		//�����������ʽ
		encode->inSampleFmt = SampleFmt::S16;
		//����Ĳ�����
		encode->inSampleRate = 44100;
		//��ͨ��ÿ֡��С
		encode->nbSample = 1024;
		//�����������ʽ
		encode->outSmapleFmt = SampleFmt::FLATP;


		///��Ƶ����
		encode->inWidth = capture->width;
		encode->inHeight = capture->height;
		encode->outWidth = capture->width;
		encode->outHeight = capture->height;

		///��ʼ�����ظ�ʽת��������
		if (!encode->InitScale()){
			getchar();
			return -1;
		}
		cout << inUrl << "��ʼ�����ظ�ʽת�� success" << endl;
		///��ʼ����Ƶ�ز���������
		if (!encode->InitResample())
		{
			getchar();
			return -1;
		}
		cout << inUrl << "��ʼ����Ƶ�ز��� success" << endl;
		///��ʼ����Ƶ������
		if (!encode->InitVideoCodec())
		{
			getchar();
			return -1;
		}
		cout << inUrl << "��ʼ����Ƶ������ success" << endl;
		///��ʼ����Ƶ������
		if (!encode->InitAudioCodec())
		{
			getchar();
			return -1;
		}
		cout << inUrl << "��ʼ����Ƶ������ success" << endl;
		///��װ������
		if (!pusher->Init(outUrl)){
			getchar();
			return -1;
		}
		cout << inUrl << "��װ������ success" << endl;
		//�����Ƶ��
		pusher->AddStream(encode->vc);
		cout << inUrl << "�����Ƶ�� success" << endl;
		//�����Ƶ��
		pusher->AddStream(encode->ac);
		cout << inUrl << "�����Ƶ�� success" << endl;
		///������IO��ͨ��
		pusher->OpenIO();
		cout << inUrl << "������IO��ͨ�� success" << endl;
		for (;;) {
			Data videoData = capture->Pop();
			if (videoData.size <= 0)
			{
				Sleep(1);
				continue;
			}

			if (videoData.size > 0)
			{
				///rgb to yuv
				AVFrame *yuv = encode->RgbToYuv((char *)videoData.data);
				if (!yuv) continue;
				cout << inUrl << "RgbToYuv success" << endl;
				///h264����
				AVPacket *packet = encode->EncodeVideo(yuv);
				if (!packet)continue;
				cout << inUrl << "EncodeVideo success" << endl;
				if (pusher->SendPacket(packet))
				{
					cout << packet << flush;
				}
			}
		}
	}
	catch (const std::exception& ex)
	{
		if (encode)
		{
			encode->Close();
		}
		if (pusher)
		{
			pusher->Close();
		}
		cerr << ex.what() << endl;
	}

	return 0;
}

//int main2(int argc, char *argv[])
//{
//	//rtsp url������һ�����õĲ�������ַ
//	char *inUrl = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
//	//nginx-rtmp ֱ��������rtmp����URL(192.168.1.106�����������ip,ȷ������������)
//	char *outUrl = "rtmp://192.168.42.134/live";
//	//ע�����еķ�װ��
//	av_register_all();
//	//ע�����еı������
//	avcodec_register_all();
//	//ע����������Э��
//	avformat_network_init();
//
//	VideoCapture cam;
//
//	namedWindow("video");
//	//if (cam.open(0))
//	Mat frame;
//
//	//���ظ�ʽת�������Ķ���
//	SwsContext *vsc = NULL;
//
//	//��������ݽṹ
//	AVFrame *yuv = NULL;
//
//	//������������
//	AVCodecContext *vc = NULL;
//
//	//rtmp flv ��װ��
//	AVFormatContext *ic = NULL;
//	try
//	{
//		///opencv����
//		cam.open(inUrl);
//		if (!cam.isOpened())
//		{
//			throw exception("camera open failed");
//		}
//		cout << inUrl << " cam open success" << endl;
//		//��ȡ�������Ϣ
//		int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
//		int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
//		int fps = cam.get(CAP_PROP_FPS);
//
//		///��ʼ�����ظ�ʽת��������
//		vsc = sws_getCachedContext(vsc,
//			//����ʹ��opencv�򿪵��������ĸ�ʽ��AV_PIX_FMT_BGR24����������Ҫע��
//			inWidth,inHeight,AV_PIX_FMT_BGR24,
//			//Ŀ��Ŀ�ߺ����ظ�ʽ
//			inWidth,inHeight, AV_PIX_FMT_YUV420P,
//			//�ߴ�仯�㷨
//			SWS_BICUBIC,
//			0,0,0
//			);
//
//		if(!vsc)
//		{
//			throw exception("sws_getCachedContext failed!");
//		}
//
//		///��ʼ����������ݽṹ
//		yuv = av_frame_alloc();
//		yuv->format = AV_PIX_FMT_YUV420P;
//		yuv->width = inWidth;
//		yuv->height = inHeight;
//		yuv->pts = 0;
//
//		//�����������֮ǰҪȷ����Щ�����Ѿ�����
//		//  * -format (pixel format for video, sample format for audio)
//		//  * -width and height for video
//		//	* -nb_samples and channel_layout for audio
//		int result = av_frame_get_buffer(yuv, 32);
//		if (result!= 0)
//		{
//			char arr[1024] = { 0 };
//			av_strerror(result, arr, sizeof(arr));
//			throw exception(arr);
//		}
//
//		///��ʼ���������
//		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
//		if(!codec)
//		{
//			throw exception("find h264 encoder failed");
//		}
//
//		vc = avcodec_alloc_context3(codec);
//		if(!vc)
//		{
//			throw exception("avcodec_alloc_context3 failed!");
//		}
//
//		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//		vc->codec_id = codec->id;
//		vc->thread_count = 8;
//		//ѹ����ÿ����Ƶ��bitλ��С 50kB
//		vc->bit_rate = 50 * 1024 * 8;
//		vc->width = inWidth;
//		vc->height = inHeight;
//		vc->time_base = { 1,fps };
//		vc->framerate = { fps,1 };
//		//������Ĵ�С������֡һ���ؼ�֡
//		vc->gop_size = 50;
//		//��Ҫb֡
//		vc->max_b_frames = 0;
//		vc->pix_fmt = AV_PIX_FMT_YUV420P;
//
//		result = avcodec_open2(vc, 0, 0);
//		if (result != 0)
//		{
//			char buf[1024] = { 0 };
//			av_strerror(result, buf, sizeof(buf) - 1);
//			throw exception(buf);
//		}
//
//		cout << "avcodec_open2 success!" << endl;
//
//
//		///��װ������
//		result = avformat_alloc_output_context2(&ic, 0, "flv", outUrl);
//		if (result < 0)
//		{
//			char buf[1024] = { 0 };
//			av_strerror(result, buf, sizeof(buf) - 1);
//			throw exception(buf);
//		}
//
//		//�����Ƶ��
//		AVStream *vs = avformat_new_stream(ic, NULL);
//		if (!vs) 
//		{
//			throw exception("avformat_new_stream failed");
//		}
//		vs->codecpar->codec_tag = 0;
//
//		//�ӱ��������Ʋ���
//		avcodec_parameters_from_context(vs->codecpar, vc);
//		av_dump_format(ic, 0, outUrl, 1);
//
//		///������IO��ͨ��
//		result = avio_open(&ic->pb, outUrl, AVIO_FLAG_WRITE);
//		if (result < 0)
//		{
//			char buf[1024] = { 0 };
//			av_strerror(result, buf, sizeof(buf) - 1);
//			throw exception(buf);
//		}
//		//д���װͷ
//		result = avformat_write_header(ic, NULL);
//		if (result < 0) {
//			char buf[1024 ]= { 0 };
//			av_strerror(result, buf, sizeof(buf) - 1);
//			throw exception(buf);
//		}
//
//
//		int vpts = 0;
//
//		AVPacket packet;
//		memset(&packet, 0, sizeof(AVPacket));
//		for (;;) {
//			///��ȡrtsp��Ƶ֡��������
//			//cam.read(frame); read��
//			//����������grab��retrieve����
//			if (!cam.grab()) {
//				continue;
//			}
//			///yuvתrgb
//			if (!cam.retrieve(frame)) {
//				continue;
//			}
//			imshow("video", frame);
//
//			///rgb to yuv
//			uint8_t * srcSlice[AV_NUM_DATA_POINTERS] = { 0 };
//			//indata[0] bgrbgrbgr
//			//plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr 
//			srcSlice[0] = frame.data;
//			int srcStride[AV_NUM_DATA_POINTERS] = { 0 };
//			//һ�У������ݵ��ֽ���
//			srcStride[0] = frame.cols*frame.elemSize();
//			int h = sws_scale(vsc,
//				srcSlice,
//				srcStride,
//				0,
//				frame.rows,
//				yuv->data,
//				yuv->linesize
//			);
//			if (h < 0)
//			{
//				continue;
//			}
//
//			///h264����
//			yuv->pts = vpts;
//
//			//pts��Ҫ��������Ȼ��ʾ
//			// non-strictly-monotonic PTS
//			vpts++;
//			result = avcodec_send_frame(vc, yuv);
//
//			if (result != 0)
//			{
//				continue;
//			}
//			result = avcodec_receive_packet(vc, &packet);
//			if (result != 0 ){
//				continue;
//			}
//			cout << "*" << packet.size << flush;
//			
//			packet.pts = av_rescale_q(packet.pts, vc->time_base, vs->time_base);
//			packet.dts = av_rescale_q(packet.dts, vc->time_base, vs->time_base);
//			packet.duration = av_rescale_q(packet.duration, vc->time_base, vs->time_base);
//
//			result = av_interleaved_write_frame(ic, &packet);
//			if (result == 0)
//			{
//				cout <<"#" << flush;
//			}
//			waitKey(1);
//		}
//	}
//	catch (const std::exception& ex)
//	{
//		if (cam.isOpened())
//		{
//			cam.release();
//		}
//		if (vsc)
//		{
//			sws_freeContext(vsc);
//			//ָ����NULL��һ�����õ�ϰ��
//			vsc = NULL;
//		}
//		if (vc)
//		{
//			avio_closep(&ic->pb);
//			avcodec_free_context(&vc);
//		}
//		cerr << ex.what()<< endl;
//	}
//
//	return 0;
//}

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