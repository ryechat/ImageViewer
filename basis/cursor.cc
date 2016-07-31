/* APIs
LoadImage
	Windows NT 4.0 / 95 以降
	Unicode：Windows NT / 2000 は Unicode 版と ANSI 版を実装
LoadCursorFromFile
	Windows NT/2000：Windows NT 3.5 以降
SetCursor
ShowCursor
Get/SetCursorPos
Get/Set/ReleaseCapture
	Windows NT/2000：Windows NT 3.1 以降
	Windows 95/98：Windows 95 以降
	ヘッダーファイル：Winuser.h 内で宣言、Windows.h をインクルード
	インポートライブラリ：User32.lib を使用
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