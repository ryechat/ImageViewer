#pragma once
#ifndef GUID_69EDC1CC13F64FE99777A48167DBC900
#define GUID_69EDC1CC13F64FE99777A48167DBC900

#include "thread.h"

namespace basis {

/*! 複合スレッドクラス.
	指定した数のスレッドを管理下に置く。デフォルトは1スレッド。
	タスク（関数）を渡すと空いているスレッドで処理を行う。
*/
class CThreadUnity {
public:
	//! 管理スレッド数の上限
	static size_t constexpr MAX_THREADS = 255;
	
	CThreadUnity();
	CThreadUnity(CThreadUnity&) = delete;
	CThreadUnity(CThreadUnity&&) = default;
	~CThreadUnity();

	CThreadUnity& operator=(CThreadUnity&) = delete;
	CThreadUnity& operator=(CThreadUnity&&) = default;

	//! 管理下のスレッド数を返す
	int threadCount();

	/*! スレッド数の設定.
		@param nThreads (0, MAX_THREADS ]
	*/
	bool setThreadCount(int nThreads);

	/*! 実行タスクを追加.
		@param task
			引数なし、戻り値なしの関数、ファンクタ、またはラムダ式
	*/	
	void addTask(basis::CThread::TaskTy task);

	/*! スレッドが待機状態になるのを待つ.
		@param bWaitAll
			いずれかのスレッドが待機状態になるのを待つときは false,
			全てのスレッドを待つときは true を指定する。
		@param waitMilliSeconds
			待機時間の上限。ミリ秒単位。負数を指定すると上限なし
		@return
			指定時間内にスレッドが待機状態になった場合はその時点で
			処理を復帰し、true を返す。
			依然稼働中なら false を返す。
	*/
	bool wait(bool bWaitAll, int waitMilliSeconds);

private:
	class Impl;
	Impl *impl;
};

}  // namespace

#endif