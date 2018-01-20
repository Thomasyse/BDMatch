#include "datastruct.h"

#include <algorithm>  
using namespace DataStruct;

DataStruct::noded::noded(int num)
{
	count = num;
	head = 0;
	data = new double[count];
	for (int i = 0; i < count; i++) *(data + i) = 0;
}

DataStruct::noded::noded(noded& a)
{
	count = a.size();
	head = a.gethead();
	data = new double[count];
	for (int i = 0; i < count; i++)
		*(data + i) = *(a.data + i);
}

double DataStruct::noded::read0(const int &pos)
{
	return *(data + pos);
	return 0;
}

int DataStruct::noded::add(const double &val)
{
	*(data + head) = val;
	if (1 + head >= count) head = 0;
	else head++;
	return 0;
}

int DataStruct::noded::set(int pos,const double &val)
{
	if (pos + head >= count)pos = pos + head - count + 1;
	else pos = pos + head;
	*(data + pos) = val;
	return 0;
}

double DataStruct::noded::maxabs()
{
	double maxx = fabs(*data);
	for (int i = 0; i < count; i++) {
		if (fabs(*(data + i)) > maxx) maxx = fabs(*(data + i));
	}
	return maxx;
}

int DataStruct::noded::size()
{
	return count;
}

int DataStruct::noded::gethead()
{
	return head;
}

DataStruct::noded::~noded()
{
	if (data != nullptr) {
		delete[] data;
		data = nullptr;
	}
}


DataStruct::node::node(const int &num)
{
	count = num;
	head = 0;
	data = new char[count];
	for (int i = 0; i < count; i++) *(data + i) = 0;
}

DataStruct::node::node(node& a)
{
	count = a.size();
	head = a.gethead();
	data = new char[count];
	for (int i = 0; i < count; i++)
		*(data + i) = *(a.data + i);
}

char DataStruct::node::read0(const int &pos)
{
	return *(data + pos);
}

#pragma unmanaged
char * DataStruct::node::getdata()
{
	return data;
}
#pragma managed
int DataStruct::node::add(const char &val)
{
	*(data + head) = val;
	if (1 + head >= count) head = 0;
	else head++;
	return 0;
}

int DataStruct::node::set(int pos, const char &val)
{
	if (pos + head >= count)pos = pos + head - count + 1;
	else pos = pos + head;
	*(data + pos) = val;
	return 0;
}

int DataStruct::node::sum()
{
	int sum = 0;
	for (int i = 0; i < count; i++)
		sum += *(data + i);
	return sum;
}

char DataStruct::node::maxv()
{
	char max = -128;
	for (int i = 0; i < count; i++)
		if (*(data + i) > max)max = *(data + i);
	return max;
}

int DataStruct::node::size()
{
	return count;
}

int DataStruct::node::gethead()
{
	return head;
}

DataStruct::node::~node()
{
	if (data != nullptr) {
		delete[] data;
		data = nullptr;
	}
}


DataStruct::bdsearch::bdsearch(const int &num)
{
	bditem.reserve(num);
}

int DataStruct::bdsearch::push(const int &time, const int &diff)
{
	std::array<int, 2> a;
	a[0] = time;
	a[1] = diff;
	bditem.push_back(a);
	return 0;
}

int DataStruct::bdsearch::read(const int &pos)
{
	return bditem[pos][0];
}

int DataStruct::bdsearch::find(const int &searchnum, const int &retype)
{
	int index = 0;
	for (auto &i : bditem) {
		if (i[0] == searchnum) {
			if (!retype)return index;
			else return i[1];
		};
		index++;
	}
	return index;
}

int DataStruct::bdsearch::sort()
{
	std::sort(bditem.begin(), bditem.end(), [](std::array<int, 2> &a, std::array<int, 2>&b) {
		return a[1] < b[1];
	});
	return 0;
}

int DataStruct::bdsearch::size()
{
	return bditem.size();
}


DataStruct::timec::timec(int start0, int end0, bool iscom0, String^ head0)
{
	start1 = start0;
	end1 = end0;
	iscom1 = iscom0;
	head1 = head0;
}

int DataStruct::timec::start()
{
	return start1;
}

int DataStruct::timec::end()
{
	return end1;
}

bool DataStruct::timec::iscom()
{
	return iscom1;
}

String ^ DataStruct::timec::head()
{
	return head1;
}

int DataStruct::timec::start(const int &start0)
{
	start1 = start0;
	return 0;
}

int DataStruct::timec::end(const int &end0)
{
	end1 = end0;
	return 0;
}

bool DataStruct::timec::iscom(const bool &iscom0)
{
	iscom1 = iscom0;
	return false;
}

String ^ DataStruct::timec::head(String ^ head0)
{
	head1 = head0;
	return "";
}

DataStruct::Var::Var(std::vector<std::vector<node*>>* tv0, std::vector<std::vector<node*>>* bd0,
	int tvstart0, int bdstart0, int duration0, int ch0, int minroundnum0, int interval0, int ISAMode0, Int64 *diffa0)
{
	tv = tv0;
	bd = bd0;
	ch = ch0;
	diffa = diffa0;
	tvstart = tvstart0;
	bdstart = bdstart0;
	duration = duration0;
	minroundnum = minroundnum0;
	interval = interval0;
	ISAMode = ISAMode0;
}

void DataStruct::Var::caldiff()
{
	using namespace System::Threading;
	if (diffa[2] <= 0)return;
	Int64 sum = 0;
	Varum *varum; ;
	switch (ISAMode) {
	case 0:
		varum = new Varum(tv, bd, tvstart, bdstart, duration, ch, diffa + 1);
		sum = varum->caldiff();
		break;
	case 1:
		varum = new Varumsse(tv, bd, tvstart, bdstart, duration, ch, diffa + 1);
		sum = varum->caldiff();
		break;
	case 2:
		varum = new Varumavx2(tv, bd, tvstart, bdstart, duration, ch, diffa + 1);
		sum = varum->caldiff();
		break;
	default:
		break;
	}
	delete varum;
	if (sum < diffa[1]) {
		Interlocked::Exchange(diffa[1], sum);
		Interlocked::Exchange(diffa[0], bdstart);
		Interlocked::Exchange(diffa[2], minroundnum);
	}
	else if (labs(bdstart - diffa[0]) <= minroundnum * interval) {
		Interlocked::Decrement(diffa[2]);
	}
}

DataStruct::Varum::Varum(std::vector<std::vector<node*>>* tv0, std::vector<std::vector<node*>>* bd0, int tvstart0, int bdstart0,
	int duration0, int ch0, long long *diffa10)
{
	tv = tv0;
	bd = bd0;
	ch = ch0;
	diffa1 = diffa10;
	tvstart = tvstart0;
	bdstart = bdstart0;
	duration = duration0;
	size = (*tv)[0][0]->size();
}

long long DataStruct::Varum::caldiff()
{
	long long sum = 0;
	char *tvdata, *bddata;
	for (int i = 0; i <= duration; i++) {
		int tvpos = i + tvstart;
		int bdpos = i + bdstart;
		for (int j = 0; j < ch; j++) {
			tvdata = (*tv)[j][tvpos]->getdata();
			bddata = (*bd)[j][bdpos]->getdata();
			for (int k = 0; k < size; k++) {
				sum += labs(tvdata[k] - bddata[k])*(tvdata[k] + 129);
			}
		}
		if (sum > *diffa1)break;
	}
	tvdata = bddata = nullptr;
	return sum;
}

long long DataStruct::Varumsse::caldiff()
{
	long long sum = 0;
	int vectornum = size / 4;
	int *tvarray = new int[size];
	int *bdarray = new int[size];
	char *tvdata, *bddata;
	int *ls;
	__m128i tvvector, bdvector, w1vector, difvector;
	w1vector = _mm_set1_epi32(129);
	for (int i = 0; i <= duration; i++) {
		int tvpos = i + tvstart;
		int bdpos = i + bdstart;
		for (int j = 0; j < ch; j++) {
			tvdata = (*tv)[j][tvpos]->getdata();
			bddata = (*bd)[j][bdpos]->getdata();
			for (int k = 0; k < size; k++) {
				tvarray[k] = static_cast<int>(tvdata[k]);
				bdarray[k] = static_cast<int>(bddata[k]);
			}
			for (int k = 0; k < vectornum; k++) {
				ls = tvarray + k * 4;
				tvvector = _mm_set_epi32(*(ls), *(ls + 1), *(ls + 2), *(ls + 3));
				ls = bdarray + k * 4;
				bdvector = _mm_set_epi32(*(ls), *(ls + 1), *(ls + 2), *(ls + 3));
				difvector = _mm_mullo_epi32(_mm_abs_epi32(_mm_sub_epi32(tvvector, bdvector)),
					_mm_add_epi32(tvvector, w1vector));
				for (int l = 0; l < 4; l++) {
					sum += difvector.m128i_i32[l];
				}
			}
		}
		if (sum > *diffa1)break;
	}
	delete[] tvarray;
	delete[] bdarray;
	ls = nullptr;
	tvdata = bddata = nullptr;
	return sum;
}

long long DataStruct::Varumavx2::caldiff()
{
	long long sum = 0;
	int vectornum = size / 8;
	int *tvarray = new int[size];
	int *bdarray = new int[size];
	char *tvdata, *bddata;
	__m256i tvvector, bdvector;
	__m256i difvector, w1vector, mask;
	mask = _mm256_set1_epi32(-1);
	w1vector = _mm256_set1_epi32(129);
	for (int i = 0; i <= duration; i++) {
		int tvpos = i + tvstart;
		int bdpos = i + bdstart;
		for (int j = 0; j < ch; j++) {
			tvdata = (*tv)[j][tvpos]->getdata();
			bddata = (*bd)[j][bdpos]->getdata();
			for (int k = 0; k < size; k++) {
				tvarray[k] = static_cast<int>(tvdata[k]);
				bdarray[k] = static_cast<int>(bddata[k]);
			}
			for (int k = 0; k < vectornum; k++) {
				tvvector = _mm256_maskload_epi32(tvarray + k * 8, mask);
				bdvector = _mm256_maskload_epi32(bdarray + k * 8, mask);
				difvector = _mm256_mullo_epi32(_mm256_abs_epi32(_mm256_sub_epi32(tvvector, bdvector)),
					_mm256_add_epi32(tvvector, w1vector));
				for (int l = 0; l < 8; l++) {
					sum += difvector.m256i_i32[l];
				}
			}
		}
		if (sum > *diffa1)break;
	}
	delete[] tvarray;
	delete[] bdarray;
	tvdata = bddata = nullptr;
	return sum;
}
#pragma managed

DataStruct::SettingVals::SettingVals()
{
}

DataStruct::SettingVals::SettingVals(SettingVals ^ in)
{
	FFTnum = in->FFTnum;
	minfinddb = in->minfinddb;
	findfield = in->findfield;
	maxlength = in->maxlength;
	minchecknum = in->minchecknum;
	outputpcm = in->outputpcm;
	draw = in->draw;
	matchass = in->matchass;
	paralleldecode = in->paralleldecode;
	fastmatch = in->fastmatch;
}

String^ DataStruct::SettingVals::getname(const SettingType &type)
{
	String^ name;
	switch (type) {
	case FFTNum:
		name = "FFTNum";
		break;
	case MinFinddB:
		name = "MinFinddB";
		break;
	case FindField:
		name = "FindField";
		break;
	case MaxLength:
		name = "MaxLength";
		break;
	case MinCheckNum:
		name = "MaxCheckNum";
		break;
	case MatchAss:
		name = "MatchAss";
		break;
	case OutputPCM:
		name = "OutputPCM";
		break;
	case Draw:
		name = "Draw";
		break;
	case ParallelDecode:
		name = "ParallelDecode";
		break;
	case FastMatch:
		name = "FastMatch";
		break;
	default:
		break;
	}
	return name;
}

int DataStruct::SettingVals::getval(const SettingType & type)
{
	int val;
	switch (type) {
	case FFTNum:
		val = FFTnum;
		break;
	case MinFinddB:
		val = minfinddb;
		break;
	case FindField:
		val = findfield;
		break;
	case MaxLength:
		val = maxlength;
		break;
	case MinCheckNum:
		val = minchecknum;
		break;
	case MatchAss:
		val = static_cast<int>(matchass);
		break;
	case OutputPCM:
		val = static_cast<int>(outputpcm);
		break;
	case Draw:
		val = static_cast<int>(draw);
		break;
	case ParallelDecode:
		val = static_cast<int>(paralleldecode);
		break;
	case FastMatch:
		val = static_cast<int>(fastmatch);
		break;
	default:
		val = 0;
		break;
	}
	return val;
}

int DataStruct::SettingVals::setval(const SettingType & type,int val)
{
	using namespace std;
	switch (type) {
	case FFTNum:
		val = max(val, 64);
		val = min(val, 16384);
		FFTnum = val;
		break;
	case MinFinddB:
		val = max(val, -80);
		val = min(val, 5);
		minfinddb = val;
		break;
	case FindField:
		val = max(val, 1);
		val = min(val, 100000);
		findfield = val;
		break;
	case MaxLength:
		val = max(val, 20);
		val = min(val, 1000);
		maxlength = val;
		break;
	case MinCheckNum:
		val = max(val, 10);
		val = min(val, 1000000);
		minchecknum = val;
		break;
	case OutputPCM:
		outputpcm = static_cast<bool>(val);
		break;
	case Draw:
		draw = static_cast<bool>(val);
		break;
	case MatchAss:
		matchass = static_cast<bool>(val);
		break;
	case ParallelDecode:
		paralleldecode = static_cast<bool>(val);
	case FastMatch:
		fastmatch = static_cast<bool>(val);
	default:
		break;
	}
	return 0;
}

