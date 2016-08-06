#pragma once
#ifndef GUID_B6132F5238C640ACB180B841FEB4AF8B
#define GUID_B6132F5238C640ACB180B841FEB4AF8B
/*!
    @file
        IMemory interface class definition

        and inherit class definition,

        GlobalMemory, HeapMemory,    CMemory
*/
#ifndef STDAFX_H
#include <memory>
#include <wtypes.h>
#endif

#include "critical_section.h"
#include "iunit_test.h"

namespace basis {

UNIT_TEST(IMemory)

/*! Memory class interface.
    Memory class interface defines buffer I/O.
    These classes have no way to copy,
    because buffers are not guarranteed to be initialized.
    There are a lot of cases that
    buffers better not to be initialized entirely.
    So this is why it cannot copies itself.
*/
class IMemory {
public:
    IMemory() {}
    virtual ~IMemory() {}

    IMemory(IMemory&) = delete;
    IMemory& operator=(IMemory&) = delete;

    /*! Allocates a new buffer.
        The old buffer will be released if successful.
        If failed to allocate a new buffer,
        it throws exception while keeping the old buffer.
        @param size The least size of a buffer to be allocated.
        @param zeroFill Fills all field of a buffer with 0 if it is set to be true.
        @return A pointer to a new buffer. No chance to be nullptr.
        @exception bad_alloc
    */
    virtual void* alloc(size_t size, bool zeroFill = false) = 0;



    /*! Reallocates buffers carrying contents as possible as it can.
    */
    virtual bool realloc(size_t size) = 0;



    /*! Releases memory buffers.
        Despite for decreasing memory usage,
        it is not necessary to call explicitly.
    */
    virtual void reset() noexcept = 0;



    /*! Returns a valid pointer to buffers.
        @exception std::runtime_error can't return address.
        @return Valid pointer to buffers.
    */
    virtual    void* address() const = 0;



    /*! Returns size of memory buffers in count of bytes.
        May return 0 if memory is not allocated or already released.
    */
    virtual size_t capacity() const = 0;
};



/*!    Global Memory with IMemory interface.
    It will lock a memory block when to use address,
    while it will unlock a memory block when to use handle.
    @note
        Global memory was designed to use a small memory block.
        It is said to be recommended to use VirtualAlloc function
        when one intend to use a memory block bigger than 1 or 2MB,
        despite certain old functions need global memory handle.
*/
class GlobalMemory : public IMemory {
public:
    GlobalMemory();
    ~GlobalMemory();

    GlobalMemory(GlobalMemory&) = delete;
    GlobalMemory&operator=(GlobalMemory&) = delete;

    GlobalMemory(GlobalMemory&&s) noexcept {
        *this = std::move(s);
    }
    GlobalMemory&operator=(GlobalMemory&&) noexcept;

    void* alloc(size_t size, bool zeroFill = false) override;
    bool realloc(size_t size) noexcept override;
    void reset() noexcept override { return reset(0, 0, 0); }
    void* address() const override;
    size_t capacity() const override;

    /*! Unlock memory and return its handle.
        It makes memory address become invalid.
        @return Handle. It may be 0 if failed.
    */
    HGLOBAL handle() noexcept;

private:
    void reset(HGLOBAL h, void* addr, BOOL lock) noexcept;
    void* lock() const;

    CriticalSection m_cs;
    HGLOBAL m_h;
    mutable void* m_address;
    mutable BOOL m_lock_count;
};



//! Heap memory with IMemory interface.
class HeapMemory : public IMemory {
public:
    HeapMemory();
    ~HeapMemory();

    HeapMemory(HeapMemory&) = delete;
    HeapMemory&operator=(HeapMemory&) = delete;
    HeapMemory(HeapMemory&&s) noexcept { *this = std::move(s); }
    HeapMemory&operator=(HeapMemory&&) noexcept;

    void* alloc(size_t size, bool bZeroFill = false) override;
    bool realloc(size_t size) noexcept override;
    void reset() noexcept override { reset(0, 0); }
    void* address() const override;
    size_t    capacity() const override;

    /*! Returns Heap memory handle.
        @exception std::runtime_error No handles.
    */
    HANDLE handle() const;

    static size_t TotalAmount() { return mTotalAmount; }

private:
    void reset(void *addr, size_t s) noexcept;
    static size_t mTotalAmount;
    CriticalSection m_cs;
    HANDLE m_h;
    void* m_address;
    size_t m_size;
};



/*! IMemory interface for the memory malloc function creates.
*/
class CMemory : public IMemory {
public:
    CMemory() : m_size(0), m_buf(0) {}
    ~CMemory() { reset(); }

    CMemory(const CMemory& s) = delete;
    CMemory&operator=(const CMemory &s) = delete;

    CMemory(CMemory &&s) noexcept;
    CMemory&operator=(CMemory &&s) noexcept;

    void *alloc (size_t size, bool zeroFill) override;
    bool realloc(size_t size) noexcept override;
    void reset() noexcept override { reset(0, 0); }
    void *address() const override {
        if (m_buf)
            return m_buf;
        throw 0;
    }
    size_t capacity() const override { return m_size; }

    static size_t TotalAmount() { return mTotalAmount; }

private:
    void reset(void *p, size_t s) noexcept;

    static size_t mTotalAmount;
    CriticalSection m_cs;
    size_t m_size;
    void *m_buf;
};

}  // namespace

#endif