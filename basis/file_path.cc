#include "file_path.h"
#include "file_item.h"
#include "stdfnc.h"

#ifndef STDAFX_H	// include files for trash()
#pragma comment(lib, "shell32.lib")
#include <ShlObj.h>
#include <atlbase.h>
#endif

namespace basis {

CFilePath::
CFilePath(const TCHAR *path) : m_path(StringBuffer::length(path) + 5)
{
	static const TCHAR * const unc = TEXT("\\\\?\\");

	if (!StringBuffer(0, path).compare(unc, 4, 0))
		m_path.append(unc);

	m_path.append(path);
}



const StringBuffer CFilePath::
getFileName() const noexcept
{
	size_t ix = m_path.rfind(TEXT('\\'));
	if (ix == StringBuffer::npos)
		return{};
	else
		return StringBuffer(0, m_path.c_str() + ix + 1);
}



void CFilePath::
setFileName(const TCHAR *p)
{
	size_t ix = m_path.rfind(TEXT('\\'));
	if (ix == StringBuffer::npos)
		m_path = p;
	else
		m_path.write(ix + 1, p, StringBuffer::length(p));
}



bool CFilePath::
exist() const noexcept
{
	return (GetFileAttributes(longPath().c_str()) != -1);
}



bool CFilePath::
isDirectory() const noexcept
{
	return(GetFileAttributes(longPath().c_str())
		& FILE_ATTRIBUTE_DIRECTORY
		) != 0;
}



CFileItem CFilePath::
open(DWORD access, DWORD share)
{
	return CFileItem(
		::CreateFile(longPath().c_str(), access, share,
			0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0)
		);
}



// 既存でない場合、占有モードでファイルを作成。
// 成功するとopen状態になる。ディレクトリは作成不可。
CFileItem CFilePath::
create(DWORD attr)
{
	return CFileItem(
		::CreateFile(longPath().c_str(),
			GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_NEW,
			attr, 0)
		);
}



bool CFilePath::
createAsDirectory()
{
	return 0 != CreateDirectory(longPath().c_str(), 0);
}



bool CFilePath::
erase()
{
	if (isDirectory())
		return 0 != RemoveDirectory(longPath().c_str());
	else
		return 0 != DeleteFile(longPath().c_str());
}



bool CFilePath::
copyTo(const CFilePath& dest, bool overwrite)
{
	return 0 != CopyFile(longPath().c_str(),
		dest.longPath().c_str(), overwrite ? FALSE : TRUE);
}



bool CFilePath::
moveTo(const CFilePath& dest)
{
	if (!MoveFile(longPath().c_str(), dest.longPath().c_str()))
		return false;
	
	*this = dest;
	return true;
}



bool CFilePath::
rename(const TCHAR *filename)
{
	return moveTo(getDir() + filename);
}



bool CFilePath::
trash() const noexcept
{
	CComPtr<IShellItem> pItem;
	if (S_OK != SHCreateItemFromParsingName(m_path.toUTF16().c_str() + 4,
		nullptr, IID_PPV_ARGS(&pItem)))
		return false;

	CComPtr<IFileOperation> op;
	if (S_OK != CoCreateInstance(
		CLSID_FileOperation, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&op)))
		return false;

	if (S_OK != op->DeleteItem(pItem, nullptr))
		return false;

	if (S_OK != op->PerformOperations())
		return false;

	BOOL isAborted;
	if (S_OK != op->GetAnyOperationsAborted(&isAborted))
		return false;

	return !isAborted;
}



// ------------------ static

CFilePath CFilePath::
GetCurrentDirectory() noexcept
{
	CFilePath t;
	t.m_path.let(::GetCurrentDirectory(0, 0) + sizeof(TCHAR));
	::GetCurrentDirectory(static_cast<DWORD>(t.m_path.capacity()),
		t.m_path.data());
	return t;
}



CFilePath CFilePath::
GetBootDirectory() noexcept
{
	tstr str = GetCommandLine(0);
	return str.substr(0, str.rfind(_T('\\')));
}

}  // namespace