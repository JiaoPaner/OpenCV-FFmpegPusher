#include "Data.h"
#include <string.h>

//�����ռ䲢����data����
Data::Data(char *data, int size)
{
	this->data = new char[size];
	memcpy(this->data, data, size);
	this->size = size;
}

void Data::Drop()
{
	if (data)
		delete data;
	data = 0;
	size = 0;
}

Data::Data()
{

}

Data::~Data() 
{

}