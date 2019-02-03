#include "datastruct.h"
#include <algorithm>  

DataStruct::node::node()
{
}

DataStruct::node::node(node &&a)
{
	count = a.size();
	data = new char[count];
	for (int i = 0; i < count; i++)
		*(data + i) = *(a.data + i);
}
int DataStruct::node::init_data(const int &num, char*& data_ptr)
{
	count = num;
	data = data_ptr;
	return 0;
}
char DataStruct::node::read0(const int &pos)
{
	return data[pos];
}

char * DataStruct::node::getdata()
{
	return data;
}

int DataStruct::node::sum()
{
	if (sumval == -2147483647) {
		sumval = 0;
		for (int i = 0; i < count; i++) {
			sumval += data[i];
		}
	}
	return sumval;
}
char DataStruct::node::maxv()
{
	char max = *data;
	for (int i = 1; i < count; i++)
		if (data[i] > max)max = data[i];
	return max;
}
int DataStruct::node::size()
{
	return count;
}


