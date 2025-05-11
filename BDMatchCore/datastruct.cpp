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
char DataStruct::Spec_Node::read0(const int &pos) const
{
	return data[pos];
}

char * DataStruct::Spec_Node::getdata() const
{
	return data;
}

int DataStruct::Spec_Node::size() const
{
	return count;
}

int DataStruct::Spec_Node::get_sum() const
{
	return sum;
}

int DataStruct::Spec_Node::set_sum(const int& in)
{
	sum = in;
	return 0;
}