#pragma once

#include "Data.h"
#include <list>
#include <mutex>

void Sleep(int mis);

class DataThread
{
protected:
	//������У��Ƚ��ȳ�
	std::list<Data> lists;

	//������д�С
	int listCount = 0;

	//��
	std::mutex mutex;

	//�߳��˳����
	bool isExit = false;

	//�߳��Ƿ��˳���ɱ��
	bool isRunning = false;

public:
	DataThread();

	//(�����б��С�����ֵ������ɾ����ɵ�����
	int maxListNum = 100;

	//�����б��β
	virtual void Push(Data d);

	//��ȡ�б��������������ݣ����ص�������Ҫ����Data.Drop����
	virtual Data Pop();

	//�����߳�
	virtual bool Start();

	//�˳��̣߳����ȴ��߳��˳���������
	virtual void Stop();

	//�̺߳���
	virtual void Main() {}

	virtual ~DataThread();

private:
	void ThreadMain();
};