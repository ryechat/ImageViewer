#pragma once
#ifndef GUID_528AE1BB7FB24BC5A32EDEFFF61AFC10
#define GUID_528AE1BB7FB24BC5A32EDEFFF61AFC10

#ifndef STDAFX_H
#include <ctype.h>
#include <wtypes.h>
#endif
#include "exception.h"

namespace basis {

template <class T>
class Char_T {};

template<>
class Char_T<char> {    // for Ascii, Shift-JIS(932)
public:
    Char_T() : m_c(0) {}
    Char_T(char c) : m_c(c) {}
    operator char() { return m_c; }
    bool isNull() const { return m_c == '\0'; }
    bool isAlpha() const { return isLower() || isUpper(); }
    bool isLower() const { return 'a' <= m_c && m_c <= 'z'; }
    bool isUpper() const { return 'A' <= m_c && m_c <= 'Z'; }
    bool isDigit() const { return '0' <= m_c && m_c <= '9'; }
    bool isHexDigit() const {
        return ('A' <= m_c && m_c <= 'F')
            || ('a' <= m_c && m_c <= 'f')
            || isDigit();
    }
    /*! Returns whether it can be lead byte of DBC.
        This function returns whether it can be a 1st byte of
        Double Byte Char; Sjis for example.
        Even if this function returns true,
        it remains a chance that the char is a 2nd byte.
        Therefore, if this function returned false,
        the char is definitely a 2nd byte.
    */
    bool isSjisLeadByte() const {
        return (0x81 <= m_c && m_c <= 0x9f)
            || (0xe0 <= m_c && m_c <= 0xfc);
    }
    int bytes() const { return sizeof(m_c); }
    int width() const { return (isSjisLeadByte()) ? 2 : 1; }
#ifdef _UNICODE
    wchar_t tchar() const {
        char buf[2] = { m_c, '\0' };
        wchar_t str[2];
        if (!MultiByteToWideChar(CP_THREAD_ACP, 0, buf, _countof(buf),
            str, _countof(str)))
            throw api_runtime_error();
        return str[0];
    }
#else
    char tchar() const { return m_c; }
#endif
private:
    char m_c;
};

template<>
class Char_T<wchar_t> {
public:
    Char_T() : m_c(0) {}
    Char_T(wchar_t c) : m_c(c) {}
    operator wchar_t() { return m_c; }
    bool isNull() const { return m_c == L'\0'; }
    bool isAlpha() const { return isLower() || isUpper(); }
    bool isLower() const { return L'a' <= m_c && m_c <= L'z'; }
    bool isUpper() const { return L'A' <= m_c && m_c <= L'Z'; }
    bool isDigit() const { return L'0' <= m_c && m_c <= L'9'; }
    bool isHexDigit() const {
        return (L'A' <= m_c && m_c <= L'F')
            || (L'a' <= m_c && m_c <= L'f')
            || isDigit();
    }
    bool isSjisLeadByte() const { return false; }
    int bytes() const { return sizeof(m_c); }
    int width() const { return 1; }
#ifdef _UNICODE
    wchar_t tchar() const { return m_c; }
#else
    char tchar() const {
        wchar_t str[2] = { m_c, '\0' };
        char buf[2];
        if (!WideCharToMultiByte(CP_THREAD_ACP, 0, str, _countof(str),
            buf, _countof(buf), 0, nullptr))
            throw api_runtime_error();
        return buf[0];        
    }
#endif
private:
    wchar_t m_c;
};

}  // namespace

#endif