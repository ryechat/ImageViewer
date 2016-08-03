#include "exception.h"
#include "char_t.h"
#include "string_buffer.h"

#ifndef STDAFX_H
#include <tchar.h>
#include <algorithm>
#endif

namespace basis {

const size_t StringBuffer::
CharLimit = (std::numeric_limits<size_t>::max)() / sizeof(TCHAR);



StringBuffer::
StringBuffer(size_t max_buf, const TCHAR *src)
: m_p(0), m_size(npos)
{
	if (max_buf || !src) {
		alloc(max_buf ? max_buf : 1);
        if (!src || copy(m_p, capacity(), src, 0)) {
            m_size = npos;
            return;
        }
	}
	else if (src) {
		m_p = const_cast<TCHAR*>(src);
		return;
	}

	throw std::runtime_error(LOCATION);
}



StringBuffer& StringBuffer::
operator=(const TCHAR *src)
{
    m_size = length(src);
	alloc(m_size + 1);
	if (copy(m_p, capacity(), src, capacity()))
		return *this;
	throw std::runtime_error(LOCATION);
}



bool StringBuffer::
operator==(const TCHAR *rhs) const noexcept
{
	for (size_t i = 0; m_p[i] == rhs[i]; i++) {
		if (!m_p[i])
			return true;
	}
	return false;
}



const TCHAR *StringBuffer::
refer(const TCHAR *s)
{
	m_buf.reset();
    m_size = npos;
	return m_p = const_cast<TCHAR*>(s);
}



TCHAR *StringBuffer::
data()
{
    m_size = npos;
	if (capacity()) return m_p;
	throw std::logic_error(LOCATION);
}



void StringBuffer::
resize(size_t s)
{
	if (!capacity() && !s)
		throw std::logic_error(LOCATION);

    m_size = npos;
	if (s == 0) // Doubles capacity.
		s = (capacity() > CharLimit / 2) ? CharLimit : capacity() * 2;

	if (s != capacity())
		alloc(s);
}



void StringBuffer::
let(size_t s)
{
	if (capacity() < s)
		realloc(s);
}



size_t inline
StringBuffer::
getSize() const noexcept
{
    if (m_size == npos)
        m_size = capacity() ? length(m_p, capacity()) : length(m_p);
    return m_size;
}



bool StringBuffer::
shrinkToFit() noexcept try
{
	size_t s = getSize() + 1;
	if (s != capacity())
		realloc(s);
	return true;
}
catch (...) {
	return false;
}



void StringBuffer::
realloc(size_t s)
{
	if (!capacity()) // Contains refference to a static string.
		throw std::logic_error(LOCATION);

	if (!m_buf.realloc(s * sizeof(TCHAR)))
		throw std::bad_alloc();

    m_size = npos;
	m_p = static_cast<PTSTR>(m_buf.address());
}



bool StringBuffer::
empty() const noexcept
{
	return (!m_p || *m_p == _T('\0'));
}



void StringBuffer::
flush() noexcept
{
    assert(m_p);
    m_size = npos;
    SecureZeroMemory(m_p, capacity());
}



bool StringBuffer::
compare(const TCHAR *p, size_t n, size_t pos) const noexcept
{
	if (n == 0)
		n = length(p);
	for (size_t i = 0; i < n; i++) {
		if (m_p[pos + i] != p[i])
			return false;
	}
	return true;
}



size_t StringBuffer::
find(const TCHAR *str, size_t pos, size_t n, bool first) const
{
	size_t buf_length{ getSize() };
	if (n == 0)
		n = length(str);
	if (n == 0 || buf_length < n || pos >= buf_length)
		return npos;

	for (size_t i{ first ? pos : buf_length - n };;) {
		if (compare(str, n, i))
			return i;
		if (i == (first ? buf_length - n : pos))
			return npos;
		i += first ? 1 : -1;
	}
}



size_t StringBuffer::
find(TCHAR c, size_t pos, bool first) const
{
	size_t ret = npos;
	for (size_t i = pos; m_p[i]; i+= Char_T<TCHAR>(m_p[i]).width()) {
		if (m_p[i] == c) {
			if (first)
				return i;
			ret = i;
		}
	}
	return ret;
}



StringBuffer StringBuffer::
substr(size_t pos, size_t n) const
{
	if (pos >= getSize())
		return{};
	StringBuffer t{ n + 1 };
	return t.append(m_p + pos, n);
}



StringBuffer& StringBuffer::
write(size_t pos, const TCHAR *str, size_t n)
{
	let(pos + n + 1);
    m_size = npos;
	for (size_t i = 0; (m_p[pos + i] = str[i]) != _T('\0'); i++) {
		if (i == n - 1) {
            m_size = pos + n;
            m_p[m_size] = _T('\0');
			break;
		}
	}
	return *this;
}



size_t StringBuffer::
count(size_t cb) const
{
	size_t cnt = 0;
	for (size_t i = 0; i < cb && m_p[i]; ++cnt) {
		i += Char_T<TCHAR>(m_p[i]).width();
	}
	return cnt;
}



void StringBuffer::
alloc(size_t size) try
{
    m_buf.alloc(size * sizeof(TCHAR), false);
	m_p = static_cast<TCHAR*>(m_buf.address());
	*m_p = _T('\0');
    m_size = 0;
}
catch (...) {
	throw std::bad_alloc();
}



std::wstring StringBuffer::
toUTF16() const
{
#ifdef _UNICODE
	return{ c_str() };
#else
	// Alloc intermediate buffer.
	int size = 2 * MultiByteToWideChar(CP_THREAD_ACP,
		0, c_str(), -1, 0, 0);
	wchar_t *wide = static_cast<wchar_t*>(malloc(size));

	// Translate to Unicode.
	if (MultiByteToWideChar(CP_THREAD_ACP, 0, c_str(), -1,
		wide, size) == 0)
		return{};

	std::wstring str(wide);
	free(wide);
	return str;
#endif
}



/*	Static Member Functions	*/



size_t StringBuffer::
length(const TCHAR *str, size_t cap) noexcept(true)
{
    if (cap)
        return str ? _tcsnlen(str, cap) : 0;
    else
        return str ? _tcslen(str) : 0;
}



bool StringBuffer::
copy(TCHAR *buf, size_t cap_buf, const TCHAR *str, size_t cap_str) noexcept
{
    if (!buf)
        return false;

    if (str == nullptr) {
        *buf = _T('\0');
        return true;
    }

    if (cap_str == 0)
        cap_str = length(str) + 1;

    if (cap_buf < cap_str)
        return false;

    return (_tcscpy_s(buf, cap_buf, str) == 0);
}

}  // namespace
