#include "string_buffer.h"	// for GetDropFile
#include "stdfnc.h"

namespace
{
	const TCHAR * const TEXT_TRUE = TEXT("true");
	const TCHAR * const TEXT_FALSE = TEXT("false");
}

namespace basis {

	const TCHAR * const TEXT_EMPTY = TEXT("");
	const TCHAR * const ASTERISK = TEXT("*");

template<>
bool ToInt(const TCHAR *p, const bool &bDefault)
{
	static const tstr text_true = TEXT("true");
	static const tstr text_false = TEXT("false");
	if (text_true == p)
		return true;
	if (text_false == p)
		return false;
	return bDefault;
}




uint32_t ReverseByteOrder(uint32_t src)
{
	uint32_t dest;
	dest = (src << 24) & 0xFF000000;
	dest |= (src << 8) & 0x00FF0000;
	dest |= (src >> 8) & 0x0000FF00;
	dest |= (src >> 24) & 0x000000FF;
	return dest;
}



PTSTR GetTypeFormat(TCHAR type[5], size_t bytes, TCHAR d_x)
{
	TCHAR *p = type + 1;
	switch (bytes) {
	case 1:
		*p = _T('h');
		++p;
		// fall through
	case 2:
		*p = _T('h');
		break;
	case 8:
		*p = _T('l');
		++p;
		// fall through
	case 4:
		*p = _T('l');
		break;
	default:
		throw 0;
	}

	*type = _T('%');
	*++p = d_x;
	*++p = NULL;
	return type;
}



tstr ToStr(int64_t n)
{
	TCHAR sz[17]{ NULL }; // 64bitは15文字+符号+\0
	if (_stprintf_s(sz, TEXT("%lld"), n) < 0)
		throw 0;
	return tstr(sz);
}



// コマンドラインのnCmd番目の引数を返す。
// 0番目は実行中のプログラムのパス、以降が引数である。
// 引数が""(ダブルクォート)で囲まれている場合はこれを除去する。
// "path" や -ls など混在した引数でも動作する。
// 戻り値は対象の引数がない場合、空(empty)である。
tstr GetCommandLine(int nCmd)
{
	TCHAR c; // 引数ごとの終端とする文字
	tstr str = ::GetCommandLine();
	tstr::size_type i = 0, n;

	while (i <= str.size()) {
		// 括り文字
		if (str[i] == _T('"') || str[i] == _T('\'')) {
			c = str[i];	// 区切り文字
			if (++i > str.size())
				return{};
		}
		else
			c = _T(' ');	// 区切り文字

		if (--nCmd <= -1)  // 目的の引数。終端文字まで取り出す
			return str.substr(i, str.find(c, i) - i);

		if ((n = str.find(c, i)) == tstr::npos)
			return{};		// 末尾まで見た。目的の引数なし
		i = n + 1;
		if (str[i] == _T(' '))	// "" のあとのスペースを無視
			++i;
	}
	return{};

}



tstr GetDropFile(WPARAM wp, UINT num)
{
	UINT size = 1 + DragQueryFile((HDROP)wp, num, nullptr, 0);
	StringBuffer buf(size);

	DragQueryFile((HDROP)wp, num, buf.data(), size);
	return tstr(buf.data());
}



LONG GetFontHeight( HDC hdc, HFONT lhs, HFONT rhs )
{
	TEXTMETRIC tm;
	LONG lhs_height = 0;

	HFONT prev_font = 0;

	if ( lhs ) {
		prev_font = (HFONT)SelectObject( hdc, lhs );
	}

	GetTextMetrics( hdc, &tm );

	if ( lhs ) {
		if ( rhs ) {
			lhs_height = tm.tmHeight;
			SelectObject( hdc, rhs );
			GetTextMetrics( hdc, &tm );
		}

		SelectObject( hdc, prev_font );

		if ( rhs && lhs_height > tm.tmHeight )
			return lhs_height;
	}

	return tm.tmHeight;
}



bool ShowProperty(HWND hParentWnd, const tstr& filePath)
{
	SHELLEXECUTEINFO sInfo = { sizeof(SHELLEXECUTEINFO) };
	sInfo.hwnd = hParentWnd;
	sInfo.lpVerb = _T("properties");
	sInfo.lpFile = filePath.c_str();
	sInfo.fMask = SEE_MASK_INVOKEIDLIST;
	sInfo.nShow = SW_SHOWNORMAL;
	BOOL const ret = ShellExecuteEx(&sInfo);
	return	ret != FALSE;
}



void HimetricFromDP(HDC hdc, SIZE& s)
{
	LONG HIMETRIC_INCH = 2540;	// 2540mm(HIMETRIC) / 1inch
	int x_dpi = GetDeviceCaps( hdc, LOGPIXELSX );
	int y_dpi = GetDeviceCaps( hdc, LOGPIXELSY );
	// たいがい96dpiをかえす。
	// 合計のmmとpixelから正確に算出する方法もあるが
	// ビットバイビット表示する場合はこちらを使う

	s.cx = s.cx * HIMETRIC_INCH / x_dpi;
	s.cy = s.cy * HIMETRIC_INCH / y_dpi;
	return;
}



bool ClearBackground(HDC hdc, RECT screen, RECT diff)
{
	SetLastError(0);
	if (screen.top < diff.top) {			// 上側クリア
		Rectangle(hdc, screen.left, screen.top, screen.right, (diff.top));
	}
	if (screen.bottom > diff.bottom) {	// 下側クリア
		Rectangle(hdc, screen.left, max(diff.bottom, screen.top),
			screen.right, screen.bottom);
	}
	if (screen.left < diff.left) {		// 残りの左側クリア
		Rectangle(hdc, screen.left, max(diff.top, screen.top),
			diff.left, min(diff.bottom, screen.bottom));
	}
	if (screen.right > diff.right) {		// 右側クリア
		Rectangle(hdc, diff.right, max(diff.top, screen.top),
			screen.right, min(diff.bottom, screen.bottom));
	}
	return GetLastError() == NO_ERROR;
}

}  // namespace