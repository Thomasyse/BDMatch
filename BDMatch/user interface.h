#pragma once
#include "Shobjidl.h"
namespace BDMatch {
	class TaskBar {
	public:
		TaskBar(void* hwnd0);
		void ProgressValue(const unsigned long long val, const unsigned long long max);
		void ProgressState(TBPFLAG tbpFlags);
	private:
		ITaskbarList3* m_pTaskBarlist;
		HWND hwnd;
	};
}