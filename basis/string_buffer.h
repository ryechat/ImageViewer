#pragma once
#ifndef GUID_84F71DC04FF2434A8E59D7163851E713
#define GUID_84F71DC04FF2434A8E59D7163851E713

#ifndef STDAFX_H
#include <string>
#endif
#include "imemory.h"
#include "iunit_test.h"

namespace basis {

UNIT_TEST(StringBuffer)

/*! String class is a buffer aimed to cooperate with APIs.
	It can contain null terminated string.
	Although it needs to count its size in every functions,
	it is capable to return a non-const pointer to data.
	This object can contain a pointer to string both of dynamic or static.
*/
class StringBuffer {
public:
	//! Maximum size of a buffer, in count of TCHAR.
	static const size_t CharLimit;

	//! Indicates an error while finding elements.
	static constexpr size_t npos = static_cast<size_t>(-1);

	/*! Default Constructor.
		It will allocate a buffer that has specified size.
		If max_buf is 0, then 1 will be used instead.
		Allocated buffer is guaranteed that
		first character is null character.
	*/
	StringBuffer(size_t max_buf = 1) : StringBuffer(max_buf, nullptr) {}

	/*! Copies a string or created as reference to a static string.
		If src is a nullptr, this constructor
		works as the same as StringBuffer(size_t max_buf).
		If max_buf is 0 and src is not nullptr,
		this object behave like alias of src.
		In that case, it creates no buffer and unable to
		modify the string.
		Otherwise, it creates buffers and copies string
		as possible as it can have.
	*/
	StringBuffer(size_t max_buf, const TCHAR *src);

	//! Copies a string.
	StringBuffer(const TCHAR *src) : StringBuffer(length(src) + 1, src)
	{}

	~StringBuffer() = default;

	//! @exception std::bad_alloc
	StringBuffer(const StringBuffer &src)
		: StringBuffer(src.capacity(), src.c_str())
	{}

	//! @exception std::bad_alloc
	StringBuffer&operator=(const StringBuffer &src) {
		alloc(src.capacity());
		append(src.c_str());
		return *this;
	}

	StringBuffer(StringBuffer&&) = default;
	StringBuffer&operator=(StringBuffer&&) = default;

	//! @exception std::bad_alloc
	//! @exception std::runtime_error
	StringBuffer&operator=(const TCHAR *src);

	bool operator==(const TCHAR *rhs) const noexcept;
	bool operator==(const StringBuffer &rhs) const noexcept {
		return *this == rhs.c_str();
	}
	bool operator!=(const TCHAR *rhs) const noexcept {
		return !(*this == rhs);
	}
	bool operator!=(const StringBuffer &rhs) const noexcept {
		return !(*this == rhs.c_str());
	}

	//! Gets to be a reference to a static string.
	const TCHAR *refer(const TCHAR*str);

	//! Returns a const pointer to the string.
	const TCHAR *c_str() const noexcept {
		return m_p ? m_p : TEXT("");
	}

	/*! Returns a non-const pointer to the string.
		One can read and write using returned pointer.
		@exception std::logic_error This is a static string.
	*/
	TCHAR *data();

	/*! Returns capacity of the dynamic string.
		If string is static, this function returns 0.
		Otherwise, it indicates maximum size that
		the buffer can contain, including null terminator.
	*/
	size_t capacity() const noexcept {
		return m_buf.capacity() / sizeof(TCHAR);
	}

	/*! Returns size of string despite null terminator.
	*/
	size_t getSize() const noexcept;

	//! Shrinks buffer size to fit a contained string.
	//! If string is static, this function returns false.
	bool shrinkToFit() noexcept;

	//! ReAllocates a buffer and copies a string.
	//! May throw if any error rose.
	void realloc(size_t s);

	/*	Releases a string and allocate another buffer.
		If a parameter was set to be 0,
		this function makes buffer size doubled.
		In this case, it may throw std::logic_error
		if the string is static, its capacity is 0 in other words.
		Also, this function may throw std::bad_alloc if
		any error rose in allocating a new buffer.
	*/
	void resize(size_t s = 0);

	/*	Reallocate a buffer if capacity is shorter than one desired.
		It makes no change to contained string.
		This function may throw while reallocating.
	*/
	void let(size_t desired_capacity);

	//! Return whether buffer is empty or not.
	/*! Return true when buffer is mis-allocated
		as well as its size is equal to 0.
	*/
	bool empty() const noexcept;

	/*! Fill each field of bytes with 0.
		Return false if buffer didn't have a dynamic string.
	*/
	void flush() noexcept;

	/*! Compare to strings.
		@param p To which compared.
		@param n Number of char to be comapred.
		@param pos Offset of this string.
		@return Return true if both of them contains the same char,
		that of specified number.
	*/
	bool compare(const TCHAR *p, size_t n = 0, size_t pos = 0) const noexcept;

	/*! Finds up a char first appears in this string.
		@param c char to be searched.
		@param pos starting position [0, size()]
		@return Index of a char.
		@pre pos <= size()
	*/
	size_t find(TCHAR c, size_t pos = 0) const {
		return find(c, pos, true);
	};

	/*! Finds up a char last appears in this string.
		@param c char to be searched.
		@param pos starting position [0, size()]
		@return Index of a char.
	*/
	size_t rfind(TCHAR c, size_t pos = 0) const {
		return find(c, pos, false);
	}

	/*! Finds up a chunk of chars first appear.
		@param p null terminated string that includes chars to be searched.
		@param pos stating position [0, size()]
		@param n number of chars to be searched.
			If this is 0, length of p string will be used instead.
	*/
	size_t find(const TCHAR *p, size_t pos = 0, size_t n = 0) const {
		return find(p, pos, n, true);
	}

	/*! Finds up a chunk of chars last appear.
	@param p a string that includes chars to be searched.
	@param pos starting position [0, size()]
	@param n number of chars to be searched.
	@pre p is null terminated or (n != 0 && n <= length(p))
	*/
	size_t rfind(const TCHAR *p, size_t pos = 0, size_t n = 0) const {
		return find(p, pos, n, false);
	}

	StringBuffer substr(size_t pos, size_t n) const;

	/*! Adds a string.
	*/
	StringBuffer& append(const TCHAR *str) {
		return write(getSize(), str, length(str));
	}
	StringBuffer& append(const TCHAR *str, size_t n) {
		return write(getSize(), str, n);
	}

	/*! Overwrite by a string.
		If buffers are too short, this function call let()
		to expand buffers.
		@param pos position to start. [0, size()]
		@param n number of char to be transfered.
	*/
	StringBuffer& write(size_t pos, const TCHAR *str, size_t n);

	/*! Translate the string to wide char string.
		If _UNICODE is defined, this will simply copy the string.
	*/
	std::wstring toUTF16() const;

	/*! Count of chars to reach the index.
		This function is for SJIS,
		single and multibyte character mixed string.
	*/

	size_t count() const { return count(npos); }

	size_t count(size_t cb) const;

	/*! Returns size of a string terminated with null char.
		This is a wrapper function of _tcslen and _tcsnlen.
		@return If buf is nullptr then this function returns 0.
			Otherwise, size of a string in count of TCHAR.
	*/
	static size_t length(const TCHAR *buf, size_t cap = 0) noexcept;

private:
	//! @exception std::bad_alloc
	void alloc(size_t size);

	//! Finds up a char first or last appears.
	size_t find(TCHAR c, size_t pos, bool first) const;

	//! Finds up a chunk of chars first or last appears.
	size_t find(const TCHAR *p, size_t pos, size_t n, bool first) const;

	static bool copy(TCHAR *buf, size_t max_buf,
		const TCHAR *str, size_t max_str) noexcept;

	TCHAR *m_p;
	CMemory m_buf;
    mutable size_t m_size; // npos if it's not known yet
};

}  // namespace

#endif

