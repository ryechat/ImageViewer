#ifndef STDAFX_H
#include <assert.h>
#endif

#include "exception.h"
#include "imemory.h"

namespace basis {

GlobalMemory::
GlobalMemory() : m_h(0), m_address(0), m_lock_count(0)
{}



GlobalMemory::
~GlobalMemory()
{
    reset();
}



GlobalMemory& GlobalMemory::
operator=(GlobalMemory &&rhs) noexcept
{
    if (m_h != rhs.m_h) {
        reset(rhs.m_h, rhs.m_address, rhs.m_lock_count);
        rhs.reset();
    }
    return *this;
}



void * GlobalMemory::
alloc(size_t size, bool bZeroFill)
{
    HGLOBAL h = 0;
    if (size != 0) {
        UINT flags = (bZeroFill) ? GMEM_MOVEABLE : GHND;
        h = GlobalAlloc(flags, size);
    }
    if (!h)
        throw std::bad_alloc();

    reset(h, 0, 0);
    return address();
}



bool GlobalMemory::
realloc(size_t size) noexcept
{
    HGLOBAL h = GlobalReAlloc(handle(), size, GMEM_MOVEABLE);
    if (!h)
        return false;
    reset(h, 0, 0);
    return true;
}



void GlobalMemory::
reset(HGLOBAL h, void* addr, BOOL lock) noexcept
{
    CriticalSection cs = m_cs.local();

    // �A�����b�N��A����B���s�����m_h�͂��̂܂�
    if (m_h && m_h != h) {
        m_h = GlobalFree(handle());
        m_h = nullptr;    // ���Ȃ��Ƃ������Ń��b�N�������̓A�����b�N���Ă�
    }
    m_h = h;
    m_address = addr;
    m_lock_count = lock;
}



void* GlobalMemory::
address() const
{
    void *p = lock();
    if (p)
        return p;
    throw std::runtime_error(LOCATION);
}



size_t    GlobalMemory::
capacity() const
{
    if (!m_h)
        return 0;
    size_t cb = static_cast<size_t>(GlobalSize(m_h));
    if (cb == 0)
        throw api_runtime_error();
    return cb;
}



// �n���h���������Ă��邩���`�F�b�N���A
// �A�����b�N�ς݂̃n���h����Ԃ��B
// �s�܂��̓G���[��0��������
HGLOBAL GlobalMemory::
handle() noexcept
{
    CriticalSection cs = m_cs.local();

    if (m_h == nullptr) return nullptr;

    // �n���h��������ł��Ȃ����H
    UINT uFlags = GlobalFlags(m_h);
    if (uFlags == GMEM_INVALID_HANDLE)
        return nullptr;

    // ���b�N�J�E���g�ɂ�����炸�A�h���X���������
    m_address = nullptr;

    // �A�����b�N�����������̃n���h����Ԃ�
    if (m_lock_count) {
        // GlobalUnlock() �߂�l�͐��� 0, ���s�� 0
        // ���b�N�J�E���g���Z���ĂȂ����b�N����Ă���Δ� 0��Ԃ�
        if (GlobalUnlock(m_h)) {
            m_lock_count--;
            return nullptr;
        }
        DWORD le = GetLastError();
        if (le == NO_ERROR || le == ERROR_NOT_LOCKED) {
            m_lock_count--;
            return m_h;
        }
        // ���Z���̂��̂����s����� �߂�l��0�ŏ�L�ȊO��Error code ��f��
        return nullptr;
    }

    // �����Ń��b�N���ĂȂ��ꍇ�́A�A�����b�N����Ă邩�ǂ�����Ԃ�
    return ((uFlags & 0xFF) == 0) ? m_h : nullptr;
}



// ���b�N���ăA�h���X��Ԃ�
void* GlobalMemory::
lock() const
{
    CriticalSection cs = m_cs.local();

    // ���łɃ��b�N
    if (m_lock_count)
        return m_address;

    m_address = GlobalLock(m_h);
    // ���s0(also:in=0)

    // �֐����s
    if (m_address == m_h) {
        m_address = nullptr;
        std::runtime_error(LOCATION);
    }

    ++m_lock_count;

    return m_address;
}



// -------------------------------------------------------- //



size_t HeapMemory::mTotalAmount = 0;



HeapMemory::
HeapMemory() : m_h(HeapCreate(0, 0, 0)), m_address(0), m_size(0)
{
    if (!m_h) throw 0;
}



HeapMemory::
~HeapMemory()
{
    reset();
    HeapDestroy(m_h);
}



HeapMemory& HeapMemory::operator=(HeapMemory&& rhs) noexcept
{
    m_cs.enter();
    rhs.m_cs.enter();
    {
        std::swap(*this, rhs);
    }
    rhs.m_cs.leave();
    m_cs.leave();
    return *this;
}



void * HeapMemory::
alloc(size_t size, bool bZeroFill)
{
    void *p = 0;
    if (m_h && size)
        p = HeapAlloc(m_h, (bZeroFill) ? HEAP_ZERO_MEMORY : 0, size);
    if (!p)
        throw std::bad_alloc();

    size_t s = static_cast<size_t>(HeapSize(m_h, 0, p));
    if (s == static_cast<size_t>(-1) || s == 0) {
        HeapFree(m_h, 0, p);
        throw std::bad_alloc();
    }

    reset(p, s);
    return p;
}



bool HeapMemory::
realloc(size_t size) noexcept
{
    if (!size)
        return false;

    void *p = HeapReAlloc(m_h, 0, m_address, size);
    if (!p)
        return false;

    if (m_h) {
        mTotalAmount -= m_size;
    }
    mTotalAmount += size;

    auto cs = m_cs.local();
    m_address = p;
    m_size = size;
    return true;
}



void HeapMemory::
reset(void *addr, size_t s) noexcept
{
    m_cs.enter();

    if (m_h && m_size) {
        mTotalAmount -= m_size;
        HeapFree(m_h, 0, m_address);
    }
    mTotalAmount += s;
    m_address = addr;
    m_size = s;
    m_cs.leave();
}



void * HeapMemory::
address() const
{
    auto cs = m_cs.local();
    if (m_size && m_address)
        return m_address;
    throw std::runtime_error(LOCATION);
}



HANDLE HeapMemory::
handle() const
{
    auto cs = m_cs.local();
    if (m_h)
        return m_h;
    throw std::runtime_error(LOCATION);
}



size_t HeapMemory::
capacity() const
{
    return m_size;
}



// -------------------------------------------------------- //

CMemory::CMemory(CMemory && s) noexcept
    : m_buf(0), m_size(0)
{
    *this = std::move(s);
}



CMemory& CMemory::
operator=(CMemory &&s) noexcept
{
    if (this == &s)
        return *this;

    auto cs = m_cs.local();
    auto cs2 = s.m_cs.local();

    reset();
    m_buf = s.m_buf;
    m_size = s.m_size;
    s.m_buf = nullptr;
    s.m_size = 0;
    return *this;
}



void * CMemory::
alloc(size_t size, bool zeroFill)
{
    void *p;
    if (!size)
        p = 0;
    else if (zeroFill)
        p = calloc(size, 1);    // �z��ŁB�[���N���A�����
    else
        p = malloc(size);
    if (!p)
        throw std::bad_alloc();

    reset(p, size);
    return p;
}



bool CMemory::
realloc(size_t size) noexcept
{
    auto cs = m_cs.local();
    if (!size)
        return false;

    void *p = ::realloc(m_buf, size);
    if (!p)
        return false;

    mTotalAmount -= m_size;
    mTotalAmount += size;

    // <- Don't reset the buffer
    m_buf = p;
    m_size = size;
    return true;
}



void CMemory::reset(void *p, size_t s) noexcept
{
    auto cs = m_cs.local();
    if (m_buf) {
        free(m_buf);
        mTotalAmount -= m_size;
    }
    mTotalAmount += s;
    m_buf = p;
    m_size = s;
}

size_t CMemory::mTotalAmount = 0;

}  // namespace