#ifndef STDAFX_H
#include <string>
#include <tchar.h>
#endif
#include "imemory.h"

namespace {

bool wrong_capa(std::unique_ptr<basis::IMemory>& p, size_t desired)
{
	if (p->capacity() < desired)
		return true;
	if (p->capacity() >= desired + sizeof(intptr_t))
		return true;
	return false;
}

}


namespace basis {

UNIT_TEST_FUNC(IMemory) {

	auto prev_usage_cmemory = CMemory::TotalAmount();
	auto prev_usage_hmemory = HeapMemory::TotalAmount();
	decltype(prev_usage_cmemory) usage;

	std::basic_string<TCHAR> sz{ TEXT("abcdefghijklmnopqrstuvwxyz") };

	for (int type = 0; type < 3; type++) {
		std::unique_ptr<IMemory> p;
		switch (type) {
		case 0:
			p.reset(new GlobalMemory);
			break;
		case 1:
			p.reset(new HeapMemory);
			break;
		case 2:
			p.reset(new CMemory);
			break;
		}


		// 空メモリの動作チェック
		if (p->capacity() != 0)
			throw 0;
		p->reset();			// do nothing
		try {
			p->address();	// must throw
			return false;
		} catch(...) {}

		// 割り当て
		size_t size = sizeof(TCHAR) * (sz.size() + 1);
		if (!p->alloc(size))
			throw 0;
		if (wrong_capa(p, size))
			throw 0;
		auto ptr = static_cast<TCHAR*>(p->address());
		if (ptr == nullptr)
			throw 0;

		// 書き込み
		*ptr = 0;
		_tcscpy_s(ptr, p->capacity() / sizeof(TCHAR), sz.c_str());
		if (_tcslen(ptr) != sz.size())
			throw 0;
		if (sz != ptr)
			throw 0;

		ptr = static_cast<TCHAR*>(p->address());

		// 拡張
		size = sizeof(TCHAR) * 50;
		if (p->realloc(size) == false)
			throw 0;

		ptr = static_cast<TCHAR*>(p->address());
		if (wrong_capa(p, size))
			throw 0;
		if (_tcslen(ptr) != sz.size())
			throw 0;
		if (sz != ptr)
			throw 0;

		// 縮小
		size = sizeof(TCHAR) * (sz.size() + 1);
		if (p->realloc(size) == false)
			throw 0;

		ptr = static_cast<TCHAR*>(p->address());
		if (wrong_capa(p, size))
			throw 0;
		if (sz != ptr)
			throw 0;

		// 再確保
		size = sizeof(TCHAR) * (sz.size() * 2 + 1);
		if (p->alloc(size) == false)
			throw 0;
		if (wrong_capa(p, size))
			throw 0;
		if (p->address() == nullptr)
			throw 0;

		// リークチェック用サイズの動作確認
		p->reset();
		usage = HeapMemory::TotalAmount();
		if (usage != prev_usage_hmemory)
			throw 0;
		usage = CMemory::TotalAmount();
		if (usage != prev_usage_cmemory)
			throw 0;
	}

	return true;
}

}  // namespace