#include "headers/datastruct.h"
#include <algorithm>  

DataStruct::Spec_Node::Spec_Node()
{
}
int DataStruct::Spec_Node::init_data(const int &num, char* data_ptr)
{
	count = num;
	data = data_ptr;
	return 0;
}
char DataStruct::Spec_Node::read0(const int &pos)
{
	return data[pos];
}

char * DataStruct::Spec_Node::getdata()
{
	return data;
}

int DataStruct::Spec_Node::sum()
{
	if (sumval == std::numeric_limits<int>::min()) {
		sumval = 0;
		for (int i = 0; i < count; i++) {
			sumval += data[i];
		}
	}
	return sumval;
}
char DataStruct::Spec_Node::maxv()
{
	char max = *data;
	for (int i = 1; i < count; i++)
		if (data[i] > max)max = data[i];
	return max;
}
int DataStruct::Spec_Node::size()
{
	return count;
}


