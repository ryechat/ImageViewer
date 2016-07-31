#include "exception.h"
#include "find_file.h"

namespace basis {

CFindFile::CFindFile(const TCHAR *path)
	: fd(new WIN32_FIND_DATA)
{
	hf = path ? FindFirstFile(path, fd) : nullptr;
	if (hf == INVALID_HANDLE_VALUE)
		hf = nullptr;
	fd->dwFileAttributes |= kFileAttrFirstFile;
}



CFindFile::~CFindFile()
{
	close();
	delete fd;
}



CFindFile::CFindFile(CFindFile && s) noexcept
	: fd(0), hf(0)
{
	*this = std::move(s);
}



CFindFile& CFindFile::
operator=(CFindFile &&s) noexcept
{
	std::swap(*this, s);
	return *this;
}



bool CFindFile::
next() noexcept
{
	// First file was already set.
	if (fd->dwFileAttributes & kFileAttrFirstFile) {
		fd->dwFileAttributes &= ~kFileAttrFirstFile;
	}
	else {
		if (hf && FindNextFile(hf, fd) == FALSE)
			close();
	}
	return hf != nullptr;
}



bool CFindFile::
nextFile() noexcept
{
	while (next()) {
		if (!(get().dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			return true;
	}
	return false;
}



bool CFindFile::
nextDirectory() noexcept
{
	while (next()) {
		if (get().dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}
	return false;
}



void CFindFile::
close() noexcept
{
	if (hf) {
		FindClose(hf);
		hf = nullptr;
	}
}



} // namespace