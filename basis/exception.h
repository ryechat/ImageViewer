#pragma once
#ifndef GUID_92604BE061ED490DB4ED5ACF19CCBF4E
#define GUID_92604BE061ED490DB4ED5ACF19CCBF4E

#ifndef STDAFX_H
#include <exception>
#include <string>
#endif

//! 送出したい例外クラスのコンストラクタに渡す文字列
#define LOCATION ::basis::MakeLocation(__func__, __FILE__, __LINE__)

//! APIがエラーのときに投げる例外
#define api_runtime_error()										\
std::runtime_error(::basis::GetErrorMessage(__func__, __FILE__, __LINE__))

namespace basis {

/*! 例外
	@file
	std::exceptionの派生クラスを基底クラスとすると、
	virtual指定がないためにstd::exceptionとしてcatchすることはできない。
	そのためC++標準の例外クラスのみを使うことにする。


	例１）送出したい例外クラスにLOCATIONマクロの文字列を渡す

		throw std::invalid_argument(LOCATION);


	例２）APIエラーの場合は専用マクロでruntime_error例外を作る。
		　このとき、GetLastError()の説明文が追加される。
		
		throw api_runtime_error();



	※考察
	標準例外はマクロによりファイル名などを含めることができるが、
	メソッドの引数にわたされたbasic_string<TCHAR>を扱えない。
	一長一短であるので、ユーザ定義例外クラスを作るときは
	例外発生場所を知らせるchar型の文字列と、
	引数やエラー内容を知らせるTCHAR型の文字列を持たせ、
	MessageBoxA() と MessageBox()の２段階で通知する手もある
*/

//! LOCATION マクロの実体
std::string
MakeLocation(const char *func, const char *file, int line);

//! api_runtime_error 用メッセージ
std::string
GetErrorMessage(const char *func, const char *file, int line);

}  // namespace

#endif // exception_h