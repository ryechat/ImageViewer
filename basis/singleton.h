#pragma once
#ifndef GUID_F5A18BB6E7C8499CBBD555248819AE8F
#define GUID_F5A18BB6E7C8499CBBD555248819AE8F

#ifndef STDAFX_H
#include <vector>
#include <mutex>
#include <assert.h>
#endif

#include "critical_section.h"

namespace basis {

	//!	シングルトンオブジェクトのインスタンス参照を取得する
	/*	C++11で、ブロックスコープを持つstatic変数の初期化は、
		スレッドセーフであることが規定された。
		http://cpprefjp.github.io/lang/cpp11/static_initialization_thread_safely.html
	*//*

//! シングルトンオブジェクトを解放する関数のリスト
/*! 下記を参考にvectorに置き換えたもの
http://qiita.com/kikuuuty/items/fcf5f7df2f0493c437dc#%E5%8B%95%E7%9A%84%E5%89%B2%E3%82%8A%E5%BD%93%E3%81%A6%E3%81%AB%E3%82%88%E3%82%8B%E5%AE%9F%E8%A3%85%E3%81%AE%E5%95%8F%E9%A1%8C%E7%82%B9
*/
class SingletonFinalizers final
{
public:
	using Finalizer = void(*)();

	SingletonFinalizers() = delete;

	//! Push deleter to finalizer list.
	static void Push(Finalizer func)
	{
		maFinalizer.push_back(std::move(func));
	}

	/*! Execute finalizers of singleton objects.
		Finalizers will be executed in FILO order.
		Getting refference to instance after finalized
		is not thread safe.
	*/
	static void Finalize();

private:
	static std::vector<Finalizer> maFinalizer;
};



template<typename T>
class singleton {
public:
	singleton() {
		get();
	}
	T* operator->() { return &get(); }
	static T& get() {
		std::call_once(flag_init, create);
		return *m_p;
	}
	static void create() {
		SingletonFinalizers::Push(&destroy);
		m_p = new T;
	}
	static void destroy() {
		delete m_p;
		m_p = nullptr;
	}
	static std::once_flag flag_init;
	static T* m_p;
};

template<typename T>
T* singleton<T>::m_p;

template<typename T>
std::once_flag singleton<T>::flag_init;

}  // namespace

#endif