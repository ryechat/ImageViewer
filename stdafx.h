#pragma once
#ifndef STDAFX_H
#define STDAFX_H

/*
    stdafxの名は、MFCで使われた "Application Framework Extensions" から。
    フレームワークとして扱う追加機能をまとめたものと理解できる。
    個々のプログラムによって扱いが変わるものは入れてはいけない。
*/

#pragma warning(push)
#pragma warning(disable:4820)
#include <windows.h>
#include <assert.h>
#include <process.h>
#include <stdarg.h>
#include <dwmapi.h>
#include <ctype.h>
#include <Psapi.h>

#ifdef UNICODE
#include <tchar.h>        // _stprintf_s, TCHAR
#else
#include <stdio.h>        // sprintf_s
#endif
#pragma warning(pop)

#include <exception>
#include <memory>
#include <mutex>
#include <algorithm>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <string>

// クラスメンバが隠蔽される旨の警告を一時的にdisable
#pragma warning(push)
#pragma warning(disable:4458)
#include <Gdiplus.h>
#pragma warning(pop)
#pragma comment( lib, "Gdiplus.lib" )

#pragma comment(lib, "shell32.lib")
#include <ShlObj.h>
#include <ShObjIdl.h>
#include <ShlGuid.h>

#include <atlbase.h>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif