#include "user interface.h"

BDMatch::TaskBar::TaskBar(void* hwnd0)
{
	CoInitialize(NULL);
	CoCreateInstance(
		CLSID_TaskbarList, NULL, CLSCTX_ALL,
		IID_ITaskbarList3, (void**)&m_pTaskBarlist);
	hwnd = reinterpret_cast<HWND>(hwnd0);
}

void BDMatch::TaskBar::ProgressValue(const unsigned long long val, const unsigned long long max)
{
	m_pTaskBarlist->SetProgressValue(hwnd, val, max);
	return;
}

void BDMatch::TaskBar::ProgressState(TBPFLAG tbpFlags)
{
	m_pTaskBarlist->SetProgressState(hwnd, tbpFlags);
	return;
}

