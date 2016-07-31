#pragma once
#ifndef GUID_AE62FCFCEFE348AD9E93B20B97EECAC2
#define GUID_AE62FCFCEFE348AD9E93B20B97EECAC2

#ifndef STDAFX_H
#include <wtypes.h>
#endif
#include "IEnumlator.h"

namespace basis {

/*!	File Enumlator.
	
	CFindFile enumlates files and/or directories.

	To enumlate all items in a directory, please give constructor
	the directory name that is followed by "\\*".
	An astarisk is a file mask, it shows.

	Calling next() function will raise first file.
	To get access to enumlated files expressed by
	WIN32_FIND_DATA structure, call get() function.
	get() function is mere accessor and no overhead costs.

	nextFile() and nextDirectory() function is also available
	instead of next() function.
	These two functions enumlate only files or directories.

	Each call to next(), nextFile() or nextDirectory() function
	enumlates matched file or directory.
	These functions returns false when no more file left.

	Next sample shows how to select all files.

	@code
	TCHAR buf[MAX_PATH + 2];
	GetCurrentDirectory(MAX_PATH, buf);
	_tcscat_s(buf, MAX_PATH + 2, TEXT("\\*"));

	CFindFile e(buf);
	while (e.nextFile()) {
		cout << e.get().cFileName << "(";
		cout << e.get().nFileSizeLow << "bytes)" << endl;
		if (e.get().nFileSizeLow >= 1024)
			break;
	}
	if (e.empty())
		cout << "All files are below 1024 bytes." << endl;
	else
		cout << "This is a first file overs 1024 bytes." << endl;
	@endcode
*/
class CFindFile : public IEnumlator<WIN32_FIND_DATA> {
public:
	CFindFile(const TCHAR *path);
	~CFindFile();

	CFindFile(const CFindFile& s) = delete;
	CFindFile& operator=(const CFindFile &s) = delete;

	CFindFile(CFindFile &&s) noexcept;
	CFindFile& operator=(CFindFile &&s) noexcept;

	bool next() noexcept override;
	bool nextFile() noexcept;
	bool nextDirectory() noexcept;
	bool empty() const noexcept { return hf == nullptr; }
	WIN32_FIND_DATA& get() noexcept override { return *fd; }

private:
	void close() noexcept;

	// Indicates first file is already set.
	// Make sure not to compete with other attributes.
	static DWORD constexpr kFileAttrFirstFile = 0x800000;

	LPWIN32_FIND_DATA fd;	// guaranteed to be a valid pointer.
	HANDLE hf;	// May be nullptr.
};

}  // namespace

#endif