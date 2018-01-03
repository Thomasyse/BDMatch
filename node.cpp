#include "node.h"
using namespace Node;

Node::noded::noded(int num)
{
	count = new int(num);
	head = new int(0);
	data = new double[*count];
	for (int i = 0; i < *count; i++) *(data + i) = 0;
}

Node::noded::noded(noded& a)
{
	count = new int(a.size());
	head = new int(a.gethead());
	data = new double[*count];
	for (int i = 0; i < *count; i++)
		*(data + i) = *(a.data + i);
}

double noded::read0(int m)
{
	return *(data + m);
	return 0;
}

int noded::add(double val)
{
	*(data + *head) = val;
	if (1 + *head >= *count) *head = 0;
	else (*head)++;
	return 0;
}

int Node::noded::set(int pos, double val)
{
	if (pos + *head >= *count)pos = pos + *head - *count + 1;
	else pos = pos + *head;
	*(data + pos) = val;
	return 0;
}

double Node::noded::maxabs()
{
	double maxx = fabs(*data);
	for (int i = 0; i < *count; i++) {
		if (fabs(*(data + i)) > maxx) maxx = fabs(*(data + i));
	}
	return maxx;
}

int noded::size()
{
	return *count;
}

int Node::noded::gethead()
{
	return *head;
}

noded::~noded()
{
	if (data != nullptr) {
		delete[] data;
		data = nullptr;
	}
	if (count != nullptr) {
		delete count;
		count = nullptr;
	}
	if (head != nullptr) {
		delete head;
		head = nullptr;
	}
}

Node::node::node(int num)
{
	count = new int(num);
	head = new int(0);
	data = new char[*count];
	for (int i = 0; i < *count; i++) *(data + i) = 0;
}

Node::node::node(node& a)
{
	count = new int(a.size());
	head = new int(a.gethead());
	data = new char[*count];
	for (int i = 0; i < *count; i++)
		*(data + i) = *(a.data + i);
}

char Node::node::read0(int m)
{
	return *(data + m);
}

int node::add(char val)
{
	*(data + *head) = val;
	if (1 + *head >= *count) *head = 0;
	else (*head)++;
	return 0;
}

int Node::node::set(int pos, char val)
{
	if (pos + *head >= *count)pos = pos + *head - *count + 1;
	else pos = pos + *head;
	*(data + pos) = val;
	return 0;
}

int Node::node::sum()
{
	int sum = 0;
	for (int i = 0; i < *count; i++)
		sum += *(data + i);
	return sum;
}

char Node::node::maxv()
{
	char max = -128;
	for (int i = 0; i < *count; i++)
		if (*(data + i) > max)max = *(data + i);
	return max;
}

int node::size()
{
	return *count;
}

int Node::node::gethead()
{
	return *head;
}

node::~node()
{
	if (data != nullptr) {
		delete[] data;
		data = nullptr;
	}
	if (count != nullptr) {
		delete count;
		count = nullptr;
	}
	if (head != nullptr) {
		delete head;
		head = nullptr;
	}
}

Node::timec::timec(int start0, int end0, bool iscom0, String^ head0)
{
	start1 = start0;
	end1 = end0;
	iscom1 = iscom0;
	head1 = head0;
}

int Node::timec::start()
{
	return start1;
}

int Node::timec::end()
{
	return end1;
}

bool Node::timec::iscom()
{
	return iscom1;
}

String ^ Node::timec::head()
{
	return head1;
}

int Node::timec::start(int start0)
{
	start1 = start0;
	return 0;
}

int Node::timec::end(int end0)
{
	end1 = end0;
	return 0;
}

bool Node::timec::iscom(bool iscom0)
{
	iscom1 = iscom0;
	return false;
}

String ^ Node::timec::head(String ^ head0)
{
	head1 = head0;
	return "";
}

Node::Var::Var(std::vector<std::vector<node*>>* tv0, std::vector<std::vector<node*>>* bd0, int samprate0,
	int tvstart0, int bdstart0, int duration0, int ch0, int minroundnum0, array<Int64>^ diffa0)
{
	tv = tv0;
	bd = bd0;
	ch = ch0;
	diffa = diffa0;
	samprate = samprate0;
	tvstart = tvstart0;
	bdstart = bdstart0;
	duration = duration0;
	minroundnum = minroundnum0;
}

void Node::Var::caldiff()
{
	using namespace System::Threading;
	if (diffa[2] <= 0)return;
	Int64 sum = 0;
	int size = (*tv)[0][0]->size();
	int minfreq = static_cast<int>(100.0 / samprate * 2 * size);
	int maxfreq = static_cast<int>(25000.0 / samprate * 2 * size);
	for(int i = 0; i <= duration; i++) {
		int tvpos = i + tvstart;
		int bdpos = i + bdstart;
		for (int j = 0; j < ch; j++) {
			for (int k = 0; k < size; k++) {
				if (k > minfreq && k < maxfreq) {
					sum += labs((*tv)[j][tvpos]->read0(k) - (*bd)[j][bdpos]->read0(k))*((*tv)[j][tvpos]->read0(k) + 128);
				}
			}
		}	
		if (sum > diffa[1])break;
	}
	if (sum < diffa[1]) {
		Interlocked::Exchange(diffa[1], sum);
		Interlocked::Exchange(diffa[0], bdstart);
		Interlocked::Exchange(diffa[2], minroundnum);
	}
	else if (labs(bdstart - diffa[0]) <= minroundnum) {
		Interlocked::Decrement(diffa[2]);
	}
}
