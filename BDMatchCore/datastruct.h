#pragma once

namespace DataStruct {

	class node
	{
	public:
		node();
		node(node &&a);
		int init_data(const int &num, char*& data_ptr);
		char read0(const int &pos);
		char *getdata();
		int sum();
		char maxv();
		int size();
	private:
		char *data = nullptr;
		int count = 0;
		int sumval = -2147483647;
	};

}


