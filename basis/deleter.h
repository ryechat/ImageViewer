#pragma once
#ifndef GUID_5D731DDEDE3F412C9EFF88A7D5DECB83
#define GUID_5D731DDEDE3F412C9EFF88A7D5DECB83

#ifndef STDAFX_H
#include <wtypes.h>
#include <memory>
#endif


/*! @file
	ハンドルのデリータの短縮エイリアスを定義する。
*/


//! SignalHandle のデリータ
struct DSignalHandle {
	void operator()(HANDLE h) const {
		if (h != INVALID_HANDLE_VALUE) {
			WaitForSingleObject(h, INFINITE);
			CloseHandle(h);
		}
	}
};

//! 解体時に待機、終了処理を行うハンドル。主にイベントオブジェクト。
typedef std::unique_ptr
<std::remove_pointer<HANDLE>::type, DSignalHandle>
SignalHandle;


//! ClosableHandle のデリータ
struct DCloseHandle {
	void operator()(HANDLE h) const {
		if (h != INVALID_HANDLE_VALUE)
			CloseHandle(h);
	}
};

//! 解体時に終了処理を行うハンドル。イベント等。
typedef std::unique_ptr
<std::remove_pointer<HANDLE>::type, DCloseHandle>
ClosableHandle;

#endif