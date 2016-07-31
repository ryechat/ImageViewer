#pragma once
#ifndef GUID_02A9C3213D894D5198B860AA07BEC6D7
#define GUID_02A9C3213D894D5198B860AA07BEC6D7

#ifndef STDAFX_H
#pragma comment( lib, "Gdiplus.lib" )
#pragma warning(push)
#pragma warning(disable:4458)
#include <Gdiplus.h>
#pragma warning(pop)
#endif

#include "singleton.h"

namespace basis {

/*!	Impl.
		�����E��̎���GDI+�̏������E�I���������s���N���X

	GdiplusInit
		��L�N���X�ɃV���O���g���p�^�[���ŃA�N�Z�X����B
		�ŏ��ɐ��������Ƃ��ɏ��������A
		�V���O���g���̉�̎��i�A�v���I�����j�ɏI���̏������s���B

		GDI+�𗘗p����A�v���́AGdiplusInit�𐶐����邱�ƁB
		���̂̓V���O���g���I�u�W�F�N�g�Ƃ��Ďc��̂ŁA
		�����ɉ�̂��č\��Ȃ��B
*/
class GdiplusInit {
	class Impl;
	using Instance = singleton<Impl>;

public:
	GdiplusInit() { Instance::get(); }
	~GdiplusInit() {}

	bool available() { return Instance::get().available(); }
	void uninit() { Instance::get().uninit(); }

private:
	//! �B�����ꂽ�V���O���g���ȓ�������
	class Impl {
	public:
		Impl() { init(); }
		~Impl() { uninit(); }

		void uninit() {
			if (m_token) return;
			Gdiplus::GdiplusShutdown(m_token);
			m_token = 0;
		}

		bool available() { return result_startup == Gdiplus::Ok; }

	private:
		void init() {
			Gdiplus::GdiplusStartupInput input;	// �R���X�g���N�^�t���\����
			result_startup = Gdiplus::GdiplusStartup(&m_token,
				&input, &m_startup);
		}
		Gdiplus::Status result_startup;
		ULONG_PTR m_token;
		Gdiplus::GdiplusStartupOutput m_startup;
	};
};

}  // basis

#endif