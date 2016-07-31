/* APIs
LoadImage
	Windows NT 4.0 / 95 �ȍ~
	Unicode�FWindows NT / 2000 �� Unicode �ł� ANSI �ł�����
LoadCursorFromFile
	Windows NT/2000�FWindows NT 3.5 �ȍ~
SetCursor
ShowCursor
Get/SetCursorPos
Get/Set/ReleaseCapture
	Windows NT/2000�FWindows NT 3.1 �ȍ~
	Windows 95/98�FWindows 95 �ȍ~
	�w�b�_�[�t�@�C���FWinuser.h ���Ő錾�AWindows.h ���C���N���[�h
	�C���|�[�g���C�u�����FUser32.lib ���g�p
*/

#include "critical_section.h"
#include "singleton.h"
#include "cursor.h"

namespace basis {

class Cursor::Impl {
public:
	Impl() : m_cursor(0) {}
	~Impl() { reset(); }

	bool set(HCURSOR hCursor)
	{
		if (!hCursor)
			return false;
		CriticalSection cs = m_cs.local();
		reset();
		m_cursor = SetCursor(hCursor);
		return true;
	}

	void reset()
	{
		auto cs = m_cs.local();
		if (m_cursor)
			SetCursor(m_cursor);
		m_cursor = nullptr;
	}

	bool load(ID idCursor)
	{
		return set(Load(idCursor));
	}

	bool load(const TCHAR *file)
	{
		return set(LoadCursorFromFile(file));
	}

	static HCURSOR Load(ID idCursor)
	{
		return static_cast<HCURSOR>(LoadImage(0,
			MAKEINTRESOURCE(static_cast<WORD>(idCursor)),
			IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	}

private:
	CriticalSection m_cs;
	HCURSOR m_cursor;
};



bool Cursor::
set(HCURSOR hCursor)
{
	return get().set(hCursor);
}



bool Cursor::
set(ID id)
{
	return get().load(id);
}



bool Cursor::
set(const TCHAR *fileName)
{
	return get().load(fileName);
}



void Cursor::
reset()
{
	return get().reset();
}



Point Cursor::
pos()
{
	POINT pt{};
	GetCursorPos(&pt);
	return{ pt.x, pt.y };
}



bool Cursor::
pos(Point pt)
{
	return 0 != SetCursorPos(pt.x, pt.y);
}



void Cursor::
capture(HWND hWnd)
{
	SetCapture(hWnd);
}



void Cursor::
release()
{
	ReleaseCapture();
}



bool Cursor::
show()
{
	return ShowCursor(TRUE) >= 0;
}



bool Cursor::
hide()
{
	return ShowCursor(FALSE) < 0;
}



Cursor::Impl & Cursor::
get()
{
	return singleton<Impl>::get();
}

}  // namespace