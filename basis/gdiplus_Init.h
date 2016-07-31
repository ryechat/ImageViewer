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
		生成・解体時にGDI+の初期化・終了処理を行うクラス

	GdiplusInit
		上記クラスにシングルトンパターンでアクセスする。
		最初に生成したときに初期化を、
		シングルトンの解体時（アプリ終了時）に終了の処理を行う。

		GDI+を利用するアプリは、GdiplusInitを生成すること。
		実体はシングルトンオブジェクトとして残るので、
		すぐに解体して構わない。
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
	//! 隠蔽されたシングルトンな内部実装
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
			Gdiplus::GdiplusStartupInput input;	// コンストラクタ付き構造体
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