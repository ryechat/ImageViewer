/*!
    @file
    CFileItem class is desined to asynchronous read and write files.
    Therefore, CFilePath.open function creates CFileItem object
    with flag FILE_FLAG_OVERLAPPED.


*/
#pragma once
#ifndef GUID_7E9375E462DA48E4A6A96E298F488D2F
#define GUID_7E9375E462DA48E4A6A96E298F488D2F

#include "types.h"
#include "iunit_test.h"

namespace basis {

UNIT_TEST(CFileItem)

class CFileItem
{
public:
    CFileItem(HANDLE h = 0)
        : m_h(h), m_overlapped(0), m_offset({}) {}
    ~CFileItem() { close(); }

    CFileItem(CFileItem&) = delete;
    CFileItem&operator=(CFileItem&) = delete;
    CFileItem(CFileItem &&s);
    CFileItem&operator=(CFileItem &&s);

    explicit operator bool() {
        return m_h != nullptr && m_h != INVALID_HANDLE_VALUE;
    }

    LARGE_INTEGER getSize() const;

    LARGE_INTEGER inline offset() const noexcept;
    void inline seek(LARGE_INTEGER offset) noexcept;

    bool read(void *dest, DWORD bytes, DWORD timeWait);

    bool write(const void *source, DWORD bytes, DWORD timeWait);

    bool finish(DWORD timeWait = INFINITE);

    bool close();

private:
    HANDLE m_h;
    LPOVERLAPPED m_overlapped;
    LARGE_INTEGER m_offset;
};



// -------------- Inline Methods ------------- //



LARGE_INTEGER inline CFileItem::
offset() const noexcept
{
    return m_offset;
}



void inline CFileItem::
seek(LARGE_INTEGER offset) noexcept
{
    m_offset = offset;
}

}  // namespace

#endif