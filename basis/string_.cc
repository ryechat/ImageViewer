#ifndef STDAFX_H
#include <tchar.h>
#include <algorithm>
#endif
#include "char_t.h"
#include "exception.h"
#include "string_.h"

namespace basis {

String::String(const std::basic_string<TCHAR>& s) : String(s.c_str(), s.size()) {}
String::String(const TCHAR *s) : String(s, length(s)) {}

String::String(const TCHAR *s, size_t n)
{
	alloc(n);
	if (!s || !m_p) {
		m_size = 0;
		return;
	}
	m_size = (std::min)(capacity() - 1, length(s));
	_tcsncpy_s(m_p, capacity(), s, m_size);	// Terminated with a null.
}



String::String(const String &s) : String(s.c_str(), s.size()) {}

String& String::
operator=(const TCHAR *src)
{
	atLeast(length(src));
	if (_tcscpy_s(m_p, capacity(), src))
		throw std::runtime_error(LOCATION);
	return *this;
}



bool String::
operator==(const TCHAR *rhs) const noexcept
{
	for (const TCHAR *p = m_p; *rhs++ == *p;) {
		if (*p++ == NULL)
			return true;
	}
	return false;
}



int String::size() const
{
	if (m_size < 0)
		m_size = length(m_p);
	return m_size;
}



void String::alloc(size_t size)
{
	if (size < 0 || size >= (std::numeric_limits<int>::max)())
		throw std::runtime_error(LOCATION);

	if (size == 0) {
		m_buf.reset();
		m_p = nullptr;
		m_size = 0;
		return;
	}
	// May throw std::bad_alloc
	m_p = static_cast<TCHAR*>(m_buf.alloc((size + 1) * sizeof(TCHAR), false));
	*m_p = NULL;
	m_size = 0;
}



int String::length(const TCHAR *str)
{
	size_t len = _tcslen(str);
	if (len >= (std::numeric_limits<int>::max)())
		throw std::runtime_error(LOCATION);
	return static_cast<int>(len);
}

// Treats as ref to static string to provide it some functions.

const TCHAR * String::
reffer(const TCHAR * s)
{
	m_buf.reset();
	m_size = -1;
	return m_p = const_cast<TCHAR*>(s);
}

}