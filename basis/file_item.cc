#include "file_item.h"
#include "exception.h"

namespace
{

inline LPOVERLAPPED 
CreateOverlapped(const LARGE_INTEGER &offset)
{
	LPOVERLAPPED lp = new OVERLAPPED{};
	lp->hEvent = CreateEvent(0, TRUE, 0, 0);
	lp->Offset = offset.LowPart;
	lp->OffsetHigh = offset.HighPart;
	return lp;
}

} // namespace

namespace basis {

bool CFileItem::
close() try
{
	finish();
	if (*this)
		CloseHandle(m_h);
	m_h = nullptr;
	return true;
}
catch (...) {
	return false;
}



CFileItem::
CFileItem(CFileItem && s)
	: m_h(s.m_h), m_overlapped(s.m_overlapped),
	m_offset(s.m_offset)
{
	s.m_h = nullptr;
	s.m_overlapped = nullptr;
}



CFileItem& CFileItem::
operator=(CFileItem &&s)
{
	m_h = s.m_h;
	s.m_h = nullptr;
	m_overlapped = s.m_overlapped;
	s.m_overlapped = nullptr;
	return *this;
}



LARGE_INTEGER CFileItem::size() const
{
	LARGE_INTEGER s{};
	if (GetFileSizeEx(m_h, &s))
		return s;
	throw std::runtime_error(LOCATION);
}



bool CFileItem::
read(void * dest, DWORD bytes, DWORD timeWait)
{
	if (!dest || !*this || m_overlapped)
		return false;

	if (bytes == 0) {
		LARGE_INTEGER s = size();
		s.QuadPart -= m_offset.QuadPart;
		if (s.HighPart > 0)
			bytes = static_cast<DWORD>(-1);
		else
			bytes = s.LowPart;
	}

	m_overlapped = CreateOverlapped(m_offset);

	ReadFile(m_h, dest, bytes, 0, m_overlapped);
	// ↑ここで発生するエラーもfinishで検出する
	return finish(timeWait);
}



bool CFileItem::
write(const void *source, DWORD bytes, DWORD timeWait)
{
	if (!source || !*this || m_overlapped)
		return false;

	m_overlapped = CreateOverlapped(m_offset);

	WriteFile(m_h, source, bytes, 0, m_overlapped);
	// ↑ここで発生するエラーもfinishで検出する
	return finish(timeWait);
}



bool CFileItem::
finish(DWORD timeWait)
{
	if (!m_overlapped)
		return true;

	DWORD cbForward = 0;
	BOOL result = GetOverlappedResultEx(m_h,
		m_overlapped, &cbForward, timeWait, FALSE);
	m_offset.QuadPart += cbForward;

	if (result) {	// Finished
		CloseHandle(m_overlapped->hEvent);
		delete m_overlapped;
		m_overlapped = nullptr;
		return true;
	}

	// Yet loading.
	if (GetLastError() == ERROR_IO_INCOMPLETE
		|| GetLastError() == WAIT_TIMEOUT)
		return false;

	throw api_runtime_error();
}

}  // namespace