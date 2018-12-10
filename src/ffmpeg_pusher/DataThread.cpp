#include "DataThread.h"

//thread��Ҫͬʱ�����±�����
#include <thread>
using namespace std;


void Sleep(int mis)
{
	chrono::milliseconds du(mis);
	this_thread::sleep_for(du);
}

DataThread::DataThread() 
{

}

//�����б��β
void DataThread::Push(Data d)
{
	mutex.lock();
	//�����ʱ������������ɾ������������������
	if (lists.size() > maxListNum)
	{
		lists.front().Drop();
		lists.pop_front();
	}
	//�²���һ������
	lists.push_back(d);
	mutex.unlock();
}

//��ȡ�б��������������ݣ����ص�������Ҫ����Data.Drop����
Data  DataThread::Pop()
{
	mutex.lock();
	if (lists.empty())
	{
		mutex.unlock();
		return Data();
	}
	//ȡ��һ��
	Data data = lists.front();
	//�Ӷ��������
	lists.pop_front();
	mutex.unlock();
	return data;
}

//�����߳�
bool  DataThread::Start()
{
	isExit = false;
	thread th(&DataThread::ThreadMain, this);
	th.detach();
	return true;
}

void DataThread::ThreadMain()
{
	isRunning = true;
	Main();
	//mainִ����ɾ����߳��˳����
	isRunning = false;
}

//�˳��̣߳����ȴ��߳��˳���������
void  DataThread::Stop()
{
	isExit = true;
	for (int i = 0; i < 200; i++)
	{
		if (!isRunning)
		{
			return;
		}
		Sleep(1);
	}
}

DataThread::~DataThread()
{

}
