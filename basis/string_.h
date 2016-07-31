#pragma once
#ifndef STDAFX_H
#include <string>
#endif

#include "imemory.h"

namespace basis {

class String {
	static int constexpr npos = -1;

	String() : m_p(0), m_size(0) {}
	String(size_t size) { alloc(size); }
	String(const TCHAR *s, size_t n);
	~String() = default;

	String(const String &s);
	String(String &&s);

	explicit String(const std::basic_string<TCHAR> &s);
	explicit String(const TCHAR *s);

	String& operator=(const String &src);
	String& operator=(String &&s);
	String& operator=(const TCHAR *s);

	bool operator==(const TCHAR *r) const noexcept;
	bool operator!=(const TCHAR *r) const noexcept;
	bool operator==(const String &r) const noexcept;
	bool operator!=(const String &r) const noexcept;
	bool operator==(const std::basic_string<TCHAR> &r) const noexcept;
	bool operator!=(const std::basic_string<TCHAR> &r) const noexcept;

	// Treats as ref to static string to provide it some functions.
	const TCHAR *reffer(const TCHAR *s);

	const TCHAR *c_str() const noexcept;
	TCHAR *data();

	int capacity() { return static_cast<int>(m_buf.capacity()); }
	int size() const;
	int countSjis();

	void alloc(size_t newSize);
	void realloc(size_t newSize);
	void shrinkToFit();
	void atLeast(size_t sizeAtLeast);
	void flush() noexcept;

	// âΩÇï‘Ç∑ÅH
	int compare(const TCHAR *p, size_t n = 0, size_t pos = 0) const noexcept;
	
	int find(TCHAR c, size_t pos = 0) const;
	int rfind(TCHAR c, size_t pos = 0) const;
	int find(const TCHAR *p, size_t pos = 0) const;
	int find(const TCHAR *p, size_t pos, size_t n) const;
	int rfind(const TCHAR *p, size_t pos = 0) const;
	int rfind(const TCHAR *p, size_t pos, size_t n) const;
	std::basic_string<TCHAR> substr(size_t pos, size_t n = 0) const;

	String& append(const TCHAR *str);
	String& append(const TCHAR *str, size_t n);
	String& write(const TCHAR *p, size_t pos, size_t n) const;

private:
	static int length(const TCHAR *str);
	static int length(const TCHAR *str, size_t cap) noexcept;

	int getSize() const noexcept;
	int find(TCHAR c, size_t pos, bool first) const;
	int find(const TCHAR *p, size_t pos, size_t n, bool first) const;

	TCHAR *m_p;  // 0:empty
	CMemory m_buf;  // capacity()==0:static_string
	mutable int m_size;	 // -1:uncounted
};

}