#pragma once

#ifndef _CLI_

// No need for platform other than Windows

// 包括 SDKDDKVer.h 将定义可用的最高版本的 Windows 平台。

// 如果要为以前的 Windows 平台生成应用程序，请包括 WinSDKVer.h，并
// 将 _WIN32_WINNT 宏设置为要支持的平台，然后再包括 SDKDDKVer.h。
#include <WinSDKVer.h>
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <SDKDDKVer.h>

#endif
