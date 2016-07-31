#ifndef STDAFX_H
#include <algorithm>
#include <string>
#include <array>
#include <tchar.h>
#endif

#include "string_buffer.h"

namespace basis {

UNIT_TEST_FUNC(StringBuffer)
{
	// バッファサイズ指定コンストラクタ
	std::array<DWORD, 4> arSize
		= { 1, 2, MAX_PATH, 5120 };
	for (auto i : arSize) {
		StringBuffer buf(i);
		if (buf.getSize() != 0)
			throw 0;
		if (buf.capacity() != i)
			throw 0;
		if (*buf.c_str() != TEXT('\0'))
			throw 0;
		*buf.data() = TEXT('\0');
		if (buf.find(TEXT("test")) != StringBuffer::npos)
			throw 0;
		if (buf.rfind(TEXT("rfind")) != StringBuffer::npos)
			throw 0;
		try {
			buf.resize();
		}
		catch (...) {
			throw 0;
		}
		if (buf.capacity() != i * 2)
			throw 0;
	}

	// 例外を送出すべきケース
	try {
#ifdef _UNICODE
		// 非UNICODEだと指定値がオーバーフローする
		StringBuffer buf(StringBuffer::CharLimit + 1);
		return false;
#endif
	}
	catch (...) {}



	// 文字列ラテラルラッパーとするとき
	{
		LPCTSTR p = TEXT("このラテラルのポインタを保持する");
		StringBuffer buf(0, p);
		if (buf.c_str() != p)
			throw 0;
		if (buf.getSize() != _tcslen(p))
			throw 0;

		// logic_errorを送出する
		try {
			buf.data();
			throw 0;
		}
		catch (std::logic_error &e) { (void)e; }

		try {
			buf.realloc(50);
			throw 0;
		}
		catch (std::logic_error &e) { (void)e; }

		try {
			buf.resize();
			throw 0;
		}
		catch (std::logic_error &e) { (void)e; }

		int n = 2;
#ifdef _UNICODE
		n = 1;
#endif
		if (buf.find(TEXT("ポインタ")) != 7 * n)
			throw 0;
		if (buf.rfind(TEXT("ラテラル")) != 2 * n)
			throw 0;
		if (buf.find(TEXT("する")) != 14 * n)
			throw 0;
		if (buf.find(TEXT("の"), 2 * n) != 6 * n)
			throw 0;
		if (buf != p)
			throw 0;
		auto sub = buf.substr(2 * n, 4 * n);
		if (sub != TEXT("ラテラル"))
			throw 0;
	}



	// 文字列の複製、コピーコンストラクタ、コピー代入演算子
	std::array<PCTSTR, 4> arStr
		= { 0, TEXT(""), TEXT("テス"), TEXT("tes") };
	for (auto i : arStr) {
		StringBuffer buf1(i);
		if (buf1.getSize() != (i ? _tcslen(i) : 0))
			throw 0;
		std::basic_string<TCHAR> str(i ? i : TEXT(""));
		if (str.compare(buf1.c_str()) != 0)
			throw 0;
		StringBuffer buf2(buf1);
		if (str.compare(buf2.c_str()) != 0)
			throw 0;
		StringBuffer buf3;
		buf3 = buf2;
		if (str.compare(buf3.c_str()) != 0)
			throw 0;
	}

	StringBuffer from_string{ TEXT("未整理のテスト") };
	StringBuffer from_size = from_string;

	StringBuffer copy_construct{ from_size };
	StringBuffer move_construct(std::move(copy_construct));

	StringBuffer move_asign;
	move_asign = std::move(move_construct);

	using tstr = std::basic_string<TCHAR>;
	tstr data1 = from_string.c_str();
	tstr data2 = move_asign.c_str();

	if (data1 != data2)
		throw 0;

	StringBuffer buf = move_asign;
	if (buf.capacity() == 0)
		throw 0;
	if (buf.getSize() == 0)
		throw 0;
	if (buf.c_str() == 0)
		throw 0;
	if (buf.data() == 0)
		throw 0;
	if (buf.empty())
		throw 0;

	if (buf.shrinkToFit() == false)
		throw 0;
	if (buf.capacity() != buf.getSize() + 1)
		throw 0;

	buf.realloc(MAX_PATH);
	if (buf.capacity() != MAX_PATH)
		throw 0;

	data2 = buf.c_str();
	if (data1 != data2)
		throw 0;

	size_t capa = buf.capacity();

	buf.resize();

	if (buf.capacity() != capa * 2)
		throw 0;

	size_t s = buf.getSize();
	buf.append(TEXT("12345"));
	if (buf.getSize() != s + 5)
		throw 0;

#ifndef _UNICODE
	buf = "convertテスト";
	if (buf.toUTF16() != L"convertテスト")
		throw 0;
	buf = "converting";
	if (buf.toUTF16() != L"converting")
		throw 0;
	buf = "２バイトコードオンリー";
	if (buf.toUTF16() != L"２バイトコードオンリー")
		throw 0;
#endif

	return true;
}

}  // namespace