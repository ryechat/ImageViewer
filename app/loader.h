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

	/*! 画像展開
		@param bWait 展開終了まで待つかどうか
		@return 展開済み、または終了した場合 2
				展開中の場合 1
				読み込めなかった場合 0
				ファイルにアクセスできない場合 -1
	*/
	enum class Status : int { Finished = 2, Loading = 1, CannotOpen = -1, Failed = 0 };
	Status loadImage(iterator itr, bool bWait);

	//! 範囲内の画像を事前展開開始
	void preloadAround(iterator itr);

	//! 終了待ち
	bool isLoading(iterator itr) { return !waitIfLoading(itr, 0); }
	bool waitIfLoading(iterator itr, DWORD timeToWait = INFINITE);

	//! ロードタスクが0になるのを待つ
	bool waitIfAnyImageIsLoading();

	//! キャッシュ解放
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