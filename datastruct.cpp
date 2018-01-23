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
int DataStruct::bdsearch::find(int searchnum, const int &retype)
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
	int tvstart0, int bdstart0, int duration0, int ch0, int ISAMode0,
	int minchecknum0, int checkfield0, long long *diffa0)
{
	tv = tv0;
	bd = bd0;
	tvstart = tvstart0;
	bdstart = bdstart0;
	duration = duration0;
	ch = ch0;
	ISAMode = ISAMode0;
	diffa = diffa0;
	minchecknum = minchecknum0;
	checkfield = checkfield0;
}
long long DataStruct::Var::caldiff()
{
	using namespace System::Threading;
	if (diffa[2] <= 0)return -1;
	long long sum = -1;
	Varcal *varcal; ;
	switch (ISAMode) {
	case 1:
		varcal = new Varcalsse(tv, bd, tvstart, bdstart, duration, ch, diffa);
		sum = varcal->caldiff();
		delete varcal;
		break;
	case 2:
		varcal = new Varcalavx2(tv, bd, tvstart, bdstart, duration, ch, diffa);
		sum = varcal->caldiff();
		delete varcal;
		break;
	default:
		varcal = new Varcal(tv, bd, tvstart, bdstart, duration, ch, diffa);
		sum = varcal->caldiff();
		delete varcal;
		break;
	}
	if (sum < Interlocked::Read(diffa[0])) {
		Interlocked::Exchange(diffa[0], sum);
		Interlocked::Exchange(diffa[1], bdstart);
		Interlocked::Exchange(diffa[2], minchecknum);
	}
	else if (labs(bdstart - diffa[1]) <= checkfield) Interlocked::Decrement(diffa[2]);
	return sum;
}

DataStruct::Varcal::Varcal(std::vector<std::vector<node*>>* tv0, std::vector<std::vector<node*>>* bd0, int tvstart0, int bdstart0,
	int duration0, int ch0, long long *diffa0)
{
	tv = tv0;
	bd = bd0;
	ch = ch0;
	diffa = diffa0;
	tvstart = tvstart0;
	bdstart = bdstart0;
	duration = duration0;
	size = (*tv)[0][0]->size();
}

#pragma unmanaged  
long long DataStruct::Varcal::caldiff()
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
		if (sum > *diffa)break;
	}
	tvdata = bddata = nullptr;
	return sum;
}
long long DataStruct::Varcalsse::caldiff()
{
	long long sum = 0;
	int vectornum = size / 8;
	char *tvdata, *bddata;
	__m128i tvvector, bdvector, difvector[2];
	__m128i sumvector[2] = { _mm_setzero_si128(),_mm_setzero_si128() };
	const __m128i w1vector = _mm_set1_epi16(129);
	for (int i = 0; i <= duration; i++) {
		int tvpos = i + tvstart;
		int bdpos = i + bdstart;
		sumvector[0] = _mm_setzero_si128();
		sumvector[1] = _mm_setzero_si128();
		for (int j = 0; j < ch; j++) {
			tvdata = (*tv)[j][tvpos]->getdata();
			bddata = (*bd)[j][bdpos]->getdata();
			for (int k = 0; k < vectornum; k++) {
				tvvector = _mm_cvtepi8_epi16(_mm_load_si128((__m128i*)tvdata));
				bdvector = _mm_cvtepi8_epi16(_mm_load_si128((__m128i*)bddata));
				difvector[0] = _mm_mullo_epi16(_mm_abs_epi16(_mm_sub_epi16(tvvector, bdvector)),
					_mm_add_epi16(tvvector, w1vector));
				difvector[1] = _mm_mulhi_epi16(_mm_abs_epi16(_mm_sub_epi16(tvvector, bdvector)),
					_mm_add_epi16(tvvector, w1vector));
				sumvector[0] = _mm_add_epi32(_mm_unpacklo_epi16(difvector[0], difvector[1]), sumvector[0]);
				sumvector[1] = _mm_add_epi32(_mm_unpackhi_epi16(difvector[0], difvector[1]), sumvector[1]);
				tvdata += 8;
				bddata += 8;
			}
		}
		sumvector[0] = _mm_add_epi32(sumvector[0], sumvector[1]);
		sumvector[0] = _mm_add_epi32(_mm_srli_epi64(sumvector[0], 32), sumvector[0]);
		sum += _mm_extract_epi32(sumvector[0], 0) + _mm_extract_epi32(sumvector[0], 2);
		if (sum > *diffa)break;
	}
	tvdata = bddata = nullptr;
	return sum;
}
long long DataStruct::Varcalavx2::caldiff()
{
	long long sum = 0;
	int vectornum = size / 16;
	__m128i *tvdata, *bddata;
	__m256i tvvector, bdvector, difvector[2];
	__m256i sumvector[2] = { _mm256_setzero_si256(),_mm256_setzero_si256() };
	__m128i sumvector8[2];
	const __m128i mask = _mm_set1_epi32(-1);
	const __m256i w1vector = _mm256_set1_epi16(129);
	for (int i = 0; i <= duration; i++) {
		int tvpos = i + tvstart;
		int bdpos = i + bdstart;
		sumvector[0] = _mm256_setzero_si256();
		sumvector[1] = _mm256_setzero_si256();
		for (int j = 0; j < ch; j++) {
			tvdata = (__m128i*)(*tv)[j][tvpos]->getdata();
			bddata = (__m128i*)(*bd)[j][bdpos]->getdata();
			for (int k = 0; k < vectornum; k++) {
				tvvector = _mm256_cvtepi8_epi16(_mm_load_si128(tvdata));
				bdvector = _mm256_cvtepi8_epi16(_mm_load_si128(bddata));
				difvector[0] = _mm256_mullo_epi16(_mm256_abs_epi16(_mm256_sub_epi16(tvvector, bdvector)),
					_mm256_add_epi16(tvvector, w1vector));
				difvector[1] = _mm256_mulhi_epi16(_mm256_abs_epi16(_mm256_sub_epi16(tvvector, bdvector)),
					_mm256_add_epi16(tvvector, w1vector));
				sumvector[0] = _mm256_add_epi32(_mm256_unpacklo_epi16(difvector[0], difvector[1]), sumvector[0]);
				sumvector[1] = _mm256_add_epi32(_mm256_unpackhi_epi16(difvector[0], difvector[1]), sumvector[1]);
				tvdata += 1;
				bddata += 1;
			}
		}
		sumvector[0] = _mm256_add_epi32(sumvector[0], sumvector[1]);
		sumvector[0] = _mm256_add_epi32(_mm256_srli_epi64(sumvector[0], 32), sumvector[0]);
		sumvector8[0] = _mm256_extracti128_si256(sumvector[0], 0);
		sumvector8[1] = _mm256_extracti128_si256(sumvector[0], 1);
		sumvector8[0] = _mm_add_epi32(sumvector8[1], sumvector8[0]);
		sumvector[0] = _mm256_cvtepi32_epi64(sumvector8[0]);
		sum += _mm256_extract_epi64(sumvector[0], 0) + _mm256_extract_epi64(sumvector[0], 2);
		if (sum > *diffa)break;
	}
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

