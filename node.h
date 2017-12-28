#pragma once
#include <math.h> 
#include <vector>

namespace Node {
	
	public ref class noded
	{
	public:
		noded(int num);
		noded(noded^ a);
		double read0(int m);
		int add(double val);
		int set(int pos, double val);
		double maxabs();
		int size();
		int gethead();
		int release();
	private:
		int *count = nullptr;
		int *head = nullptr;
		double *data = nullptr;
	};
	
	public ref class node
	{
	public:
		node(int num);
		node(node ^a);
		char read0(int m);
		int add(char val);
		int set(int pos, char val);
		int sum();
		char maxv();
		int size();
		int gethead();
		int release();
	private:
		int *count = nullptr;
		int *head = nullptr;
		char *data = nullptr;
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
		Var(array<node^, 2>^ tv0, array<node^, 2>^ bd0, int samprate0,
			int tvstart0, int bdstart0, int duration0, int ch0, int minroundnum0, array<Int64>^ diffa0);
		void caldiff();
	private:
		array<node^, 2>^ tv;
		array<node^,2>^ bd;
		int samprate;
		int tvstart;
		int bdstart;
		int duration;
		int ch;
		int minroundnum;
		array<Int64>^ diffa;
	};
}


