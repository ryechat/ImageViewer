#pragma once
#ifndef GUID_B933196D89114B67BBF2F379F809436F
#define GUID_B933196D89114B67BBF2F379F809436F

#include "image_viewer.h"
#include "thread_unity.h"
#include "critical_section.h"

namespace image_viewer {

class CImageViewer::Loader {
public:
	Loader(CImageViewer &parent_);
	~Loader() = default;

	/*! �摜�W�J
		@param bWait �W�J�I���܂ő҂��ǂ���
		@return �W�J�ς݁A�܂��͏I�������ꍇ 2
				�W�J���̏ꍇ 1
				�ǂݍ��߂Ȃ������ꍇ 0
				�t�@�C���ɃA�N�Z�X�ł��Ȃ��ꍇ -1
	*/
	enum class Status : int { Finished = 2, Loading = 1, CannotOpen = -1, Failed = 0 };
	Status loadImage(iterator itr, bool bWait);

	//! �͈͓��̉摜�����O�W�J�J�n
	void preloadAround(iterator itr);

	//! �I���҂�
	bool isLoading(iterator itr) { return !waitIfLoading(itr, 0); }
	bool waitIfLoading(iterator itr, DWORD timeToWait = INFINITE);

	//! ���[�h�^�X�N��0�ɂȂ�̂�҂�
	bool waitIfAnyImageIsLoading();

	//! �L���b�V�����
	void releaseIfOk(iterator itr);
	void markToReleaseAround(iterator itr) { helper_release(itr, false, true); }
	void unmarkAround(iterator itr) { helper_release(itr, false, false); }
	void performReleaseAround(iterator itr) { helper_release(itr, true, true); }

private:
	bool beginLoad(iterator itr);
	void helper_release(iterator itr, bool bPerform, bool bMark);

	CImageViewer &parent;
	const int m_preloadRange;
	const int m_memoryCapMegaBytes;
	basis::CThreadUnity m_threads;
	basis::CriticalSection m_cs;
	std::list<std::pair<iterator, HANDLE>> m_loading;
};

}  // namespace

#endif