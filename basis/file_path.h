#pragma once
#ifndef GUID_5C163EBB008248C3BEE781FD5A572BC8
#define GUID_5C163EBB008248C3BEE781FD5A572BC8

#ifndef STDAFX_H
#include <string>
#endif

#include "string_buffer.h"
#include "iunit_test.h"

namespace basis {

class CFileItem;
UNIT_TEST(CFilePath)

/*! File path on Windows systems.
	Wrapped API list : <br />
	GetFileAttributes, CreateFile, CopyFile, MoveFile, DeleteFile,
	GetCurrentDirectory, CreateDirectory, RemoveDirectory,
*/
class CFilePath {
public:
	using tstr = std::basic_string<TCHAR>;

	//! Default constructor
	CFilePath() {}

	/*! Conversion constructor.
		@param path Null tarminated string to express
		full path name to a directory or file
	*/
	CFilePath(const TCHAR *path);

	/*! Conversion constructor.
		@param path full path name to a directory or file.
	*/
	CFilePath(const tstr &path) : CFilePath(path.c_str()) {}

	/*! Conversion constructor.
	@param path full path name to a directory or file.
	*/
	CFilePath(const StringBuffer&path) : CFilePath(path.c_str()) {}

	//! Destructor
	~CFilePath() = default;

	/*! Copy constructor.
		@exception std::bad_alloc
	*/
	CFilePath(const CFilePath& s) = default;

	//! Move constructor.
	CFilePath(CFilePath&&) = default;

	/*! Copy asignment operator.
		@exception std::bad_alloc
	*/
	CFilePath&operator=(const CFilePath&) = default;

	//! Move asignment operator.
	CFilePath&operator=(CFilePath&&) = default;

	//! Adds a filename after a backslash.
	CFilePath&operator+=(PCTSTR file) {
		m_path.append(TEXT("\\"));
		m_path.append(file);
		return *this;
	}

	//! Adds a filename after a backslash.
	CFilePath&operator+=(const tstr &file) {
		return operator+=(file.c_str());
	}

	//! Returns a path followed by a backslash and filename.
	CFilePath operator+(PCTSTR file) const {
		return CFilePath(*this) += file;
	}

	//! Returns a path followed by a backslash and filename.
	CFilePath operator+(const tstr &file) const {
		return operator+(file.c_str());
	}

	bool operator==(const CFilePath &rhs) {
		return path() == rhs.path();
	}
	bool operator==(PCTSTR rhs) {
		return path() == rhs;
	}
	bool operator==(const tstr &rhs) {
		return path() == rhs.c_str();
	}
	bool operator!=(const CFilePath &rhs) {
		return !(*this == rhs);
	}
	bool operator!=(PCTSTR rhs) {
		return !(*this == rhs);
	}
	bool operator!=(const tstr &rhs) {
		return !(*this == rhs);
	}

	//! Returns long path name also known as UNC path.
	const StringBuffer longPath() const noexcept {
		return StringBuffer(0, m_path.c_str());
	}

	//! Returns full path name.
	const StringBuffer path() const noexcept {
		return StringBuffer(0, m_path.c_str() + 4);
	}

	//! Returns path to its parent directory.
	CFilePath getDir() const noexcept {
		size_t ix = m_path.rfind(TEXT("\\"));
		if (ix == StringBuffer::npos)
			return{};
		else
			return CFilePath(m_path.substr(4, ix - 4));
	}

	//! Returns filename.
	const StringBuffer getFileName() const noexcept;

	void setFileName(LPCTSTR fileName);

	//! Whether path string contains no characters despite null.
	bool isEmpty() {
		return m_path.empty();
	}

	//! Whether expressed file exists.
	bool exist() const noexcept;

	//! Whether expressed path is existing directory.
	bool isDirectory() const noexcept;

	/*! Opens existing file.
		@param access One or two of GENERIC_READ, GENERIC_WRITE
		@param share One or two of FILE_SHARE_READ, FILE_SHARE_WRITE
		@return Opened file item or invalid file item.
	*/
	CFileItem open(DWORD access = GENERIC_READ,
		DWORD share = FILE_SHARE_READ);

	/*! Creates non-existing file.
		This function can't create directory.
		Use createAsDirectory function if necessary.
		This function creates file with containing name.
		@param attributes file attributes from CreateFile API's.
		@return Created and opend file item or invalid file item.
	*/
	CFileItem create(DWORD attributes = FILE_ATTRIBUTE_NORMAL);

	/*! Creates non-existing Directory.
		@return @c true if succeeded, or @c false.
	*/
	bool createAsDirectory();

	/*! Erases file.
		@return @c true if secceeded, or @c false.
	*/
	bool erase();

	/*! Copies file to specified place.
		@return @c true if succeeded, or @c false.
		If overwrite was set to be false and
		destination file existed already
		then return value is @c false.
	*/
	bool copyTo(const CFilePath& dest, bool overwrite = false);

	/*! Moves file to specified place.
		A path expressed by this object will be also changed.
		If destination place is in the same directory,
		rename function can be used instead.
		@return @c true if succeeded, or @c false.
	*/
	bool moveTo(const CFilePath& dest);

	/*! Makes a file renamed.
		A path expressed by this object will be also changed.
		@return @c true if succeeded, or @c false.
	*/
	bool rename(PCTSTR filename);

	/*! Moves a file to Recycle.bin
		@return @c true if succeeded, or @c false.
	*/
	bool trash() const noexcept;

	/*! Returns the path to current directory.
	*/
	static CFilePath GetCurrentDirectory() noexcept;

	/*! Returns the path to the directory at where executed file locates.
	*/
	static CFilePath GetBootDirectory() noexcept;

private:
	StringBuffer m_path;
};

}  // namespace

#endif