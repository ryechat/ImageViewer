#pragma once
#ifndef GUID_69EDC1CC13F64FE99777A48167DBC900
#define GUID_69EDC1CC13F64FE99777A48167DBC900

#include "thread.h"

namespace basis {

/*! �����X���b�h�N���X.
	�w�肵�����̃X���b�h���Ǘ����ɒu���B�f�t�H���g��1�X���b�h�B
	�^�X�N�i�֐��j��n���Ƌ󂢂Ă���X���b�h�ŏ������s���B
*/
class CThreadUnity {
public:
	//! �Ǘ��X���b�h���̏��
	static size_t constexpr MAX_THREADS = 255;
	
	CThreadUnity();
	CThreadUnity(CThreadUnity&) = delete;
	CThreadUnity(CThreadUnity&&) = default;
	~CThreadUnity();

	CThreadUnity& operator=(CThreadUnity&) = delete;
	CThreadUnity& operator=(CThreadUnity&&) = default;

	//! �Ǘ����̃X���b�h����Ԃ�
	int threadCount();

	/*! �X���b�h���̐ݒ�.
		@param nThreads (0, MAX_THREADS ]
	*/
	bool setThreadCount(int nThreads);

	/*! ���s�^�X�N��ǉ�.
		@param task
			�����Ȃ��A�߂�l�Ȃ��̊֐��A�t�@���N�^�A�܂��̓����_��
	*/	
	void addTask(basis::CThread::TaskTy task);

	/*! �X���b�h���ҋ@��ԂɂȂ�̂�҂�.
		@param bWaitAll
			�����ꂩ�̃X���b�h���ҋ@��ԂɂȂ�̂�҂Ƃ��� false,
			�S�ẴX���b�h��҂Ƃ��� true ���w�肷��B
		@param waitMilliSeconds
			�ҋ@���Ԃ̏���B�~���b�P�ʁB�������w�肷��Ə���Ȃ�
		@return
			�w�莞�ԓ��ɃX���b�h���ҋ@��ԂɂȂ����ꍇ�͂��̎��_��
			�����𕜋A���Atrue ��Ԃ��B
			�ˑR�ғ����Ȃ� false ��Ԃ��B
	*/
	bool wait(bool bWaitAll, int waitMilliSeconds);

private:
	class Impl;
	Impl *impl;
};

}  // namespace

#endif