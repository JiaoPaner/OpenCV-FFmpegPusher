#pragma once

class Data
{
public:
	char *data = 0;
	int size = 0;

	//�����ռ䲢����data����
	Data(char *data, int size);
	void Drop();

	Data();
	virtual ~Data();
};