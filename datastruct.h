#pragma once
#include <math.h> 
#include <vector>
#include <array>

namespace DataStruct {
	
	public class noded
	{
	public:
		noded(int num);
		noded(noded& a);
		double read0(int m);
		int add(double val);
		int set(int pos, double val);
		double maxabs();
		int size();
		int gethead();
		~noded();
	private:
		int count = 0;
		int head = 0;
		double *data = nullptr;
	};
	
	public class node
	{
	public:
		node(int num);
		node(node& a);
		char read0(int m);
		int add(char val);
		int set(int pos, char val);
		int sum();
		char maxv();
		int size();
		int gethead();
		~node();
	private:
		int count = 0;
		int head = 0;
		char *data = nullptr;
	};

	public class bdsearch
	{
	public:
		bdsearch(int num);
		int push(int time,int diff);
		int read(int index);
		int find(int searchnum, int retype);
		int sort();
		int size();
	private:
		std::vector<std::array<int, 2>>bditem;
	};

	using namespace System;
	public ref class timec {
		
	public:
		timec(int start0, int end0, bool iscom0, String^ head0);
		int start();
		int end();
		bool iscom();
		String^ head();
		int start(int start0);
		int end(int end0);
		bool iscom(bool iscom0);
		String^ head(String^ head0);
	private:
		int start1;
		int end1;
		bool iscom1;
		String^ head1;
	};

	public ref class Var {
	public:
		Var(std::vector<std::vector<node*>>* tv0, std::vector<std::vector<node*>>* bd0, int samprate0,
			int tvstart0, int bdstart0, int duration0, int ch0, int minroundnum0, array<Int64>^ diffa0);
		void caldiff();
	private:
		std::vector<std::vector<node*>>* tv;
		std::vector<std::vector<node*>>* bd;
		int samprate;
		int tvstart;
		int bdstart;
		int duration;
		int ch;
		int minroundnum;
		array<Int64>^ diffa;
	};

	public ref struct SettingVals {
	public:
		SettingVals();
		SettingVals(SettingVals ^ in);
		int FFTnum = 512;
		int minfinddb = -12;
		int findfield = 8;
		int maxlength = 20;
		int minchecknum = 20;
		bool outputpcm = false;
		bool draw = false;
		bool matchass = true;
		bool paralleldecode = false;
	};
}


