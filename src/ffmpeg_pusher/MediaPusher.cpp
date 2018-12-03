#include "MediaPusher.h"
#include <iostream>
#include <string>
using namespace std;

extern "C"
{
#include <libavformat/avformat.h>

}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")


class CMediaPusher :public MediaPusher {
private:
	//��װ��
	AVFormatContext *ic = NULL;

	//����ַ
	string url = "";

	//��Ƶ��������
	const AVCodecContext *vc = NULL;

	//��Ƶ��
	AVStream *vs = NULL;
public:

	void Close()
	{
		if (ic)
		{
			avformat_close_input(&ic);
			vs = NULL;
		}
		vc = NULL;
		url = "";
	}

	bool Init(const char *url) {
		this->url = url;
		///��װ������
		int result = avformat_alloc_output_context2(&ic, 0, "flv", url);
		if (result < 0)
		{
			char buf[1024] = { 0 };
			av_strerror(result, buf, sizeof(buf) - 1);
			cout << buf;
			return false;
		}
		return true;
	}

	bool AddStream(const AVCodecContext *actx) {
		if (!ic)
		{
			return false;
		}
		//�����Ƶ��
		AVStream *vs = avformat_new_stream(ic, NULL);
		if (!vs)
		{
			cout << "avformat_new_stream failed" << endl;
			return false;
		}
		vs->codecpar->codec_tag = 0;

		//�ӱ��������Ʋ���
		avcodec_parameters_from_context(vs->codecpar, actx);
		av_dump_format(ic, 0, url.c_str(), 1);

		if (actx->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			this->vc = actx;
			this->vs = vs;
		}
		return true;
	}

	bool OpenIO() {
		///������IO��ͨ��
		int result = avio_open(&ic->pb, url.c_str(), AVIO_FLAG_WRITE);
		if (result < 0)
		{
			char buf[1024] = { 0 };
			av_strerror(result, buf, sizeof(buf) - 1);
			cout << buf << endl;
			return false;
		}
		//д���װͷ
		result = avformat_write_header(ic, NULL);
		if (result < 0) {
			char buf[1024] = { 0 };
			av_strerror(result, buf, sizeof(buf) - 1);
			cout << buf << endl;
			return false;
		}
		return true;
	}

	bool SendPacket(AVPacket *packet) {
		if (!packet || packet->size <= 0 || !packet->data)return false;
		packet->pts = av_rescale_q(packet->pts, vc->time_base, vs->time_base);
		packet->dts = av_rescale_q(packet->dts, vc->time_base, vs->time_base);
		packet->duration = av_rescale_q(packet->duration, vc->time_base, vs->time_base);

		int result = av_interleaved_write_frame(ic, packet);
		if (result == 0)
		{
			cout << "#" << flush;
			return true;
		}
		return false;
	}
};

MediaPusher * MediaPusher::Get(unsigned char index) {
	static CMediaPusher pusher[255];
	static bool isFirst = true;
	if (isFirst)
	{
		av_register_all();
		avformat_network_init();
		isFirst = false;
	}
	return &pusher[index];
}

MediaPusher::MediaPusher() {

}

MediaPusher::~MediaPusher() {

}