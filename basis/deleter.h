#pragma once
#ifndef GUID_5D731DDEDE3F412C9EFF88A7D5DECB83
#define GUID_5D731DDEDE3F412C9EFF88A7D5DECB83

#ifndef STDAFX_H
#include <wtypes.h>
#include <memory>
#endif


/*! @file
	�n���h���̃f���[�^�̒Z�k�G�C���A�X���`����B
*/


//! SignalHandle �̃f���[�^
struct DSignalHandle {
	void operator()(HANDLE h) const {
		if (h != INVALID_HANDLE_VALUE) {
			WaitForSingleObject(h, INFINITE);
			CloseHandle(h);
		}
	}
};

//! ��̎��ɑҋ@�A�I���������s���n���h���B��ɃC�x���g�I�u�W�F�N�g�B
typedef std::unique_ptr
<std::remove_pointer<HANDLE>::type, DSignalHandle>
SignalHandle;


//! ClosableHandle �̃f���[�^
struct DCloseHandle {
	void operator()(HANDLE h) const {
		if (h != INVALID_HANDLE_VALUE)
			CloseHandle(h);
	}
};

//! ��̎��ɏI���������s���n���h���B�C�x���g���B
typedef std::unique_ptr
<std::remove_pointer<HANDLE>::type, DCloseHandle>
ClosableHandle;

#endif