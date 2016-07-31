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
	TCHAR sz[17]{ NULL }; // 64bit��15����+����+\0
	if (_stprintf_s(sz, TEXT("%lld"), n) < 0)
		throw 0;
	return tstr(sz);
}



// �R�}���h���C����nCmd�Ԗڂ̈�����Ԃ��B
// 0�Ԗڂ͎��s���̃v���O�����̃p�X�A�ȍ~�������ł���B
// ������""(�_�u���N�H�[�g)�ň͂܂�Ă���ꍇ�͂������������B
// "path" �� -ls �ȂǍ��݂��������ł����삷��B
// �߂�l�͑Ώۂ̈������Ȃ��ꍇ�A��(empty)�ł���B
tstr GetCommandLine(int nCmd)
{
	TCHAR c; // �������Ƃ̏I�[�Ƃ��镶��
	tstr str = ::GetCommandLine();
	tstr::size_type i = 0, n;

	while (i <= str.size()) {
		// ���蕶��
		if (str[i] == _T('"') || str[i] == _T('\'')) {
			c = str[i];	// ��؂蕶��
			if (++i > str.size())
				return{};
		}
		else
			c = _T(' ');	// ��؂蕶��

		if (--nCmd <= -1)  // �ړI�̈����B�I�[�����܂Ŏ��o��
			return str.substr(i, str.find(c, i) - i);

		if ((n = str.find(c, i)) == tstr::npos)
			return{};		// �����܂Ō����B�ړI�̈����Ȃ�
		i = n + 1;
		if (str[i] == _T(' '))	// "" �̂��Ƃ̃X�y�[�X�𖳎�
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
	// ��������96dpi���������B
	// ���v��mm��pixel���琳�m�ɎZ�o������@�����邪
	// �r�b�g�o�C�r�b�g�\������ꍇ�͂�������g��

	s.cx = s.cx * HIMETRIC_INCH / x_dpi;
	s.cy = s.cy * HIMETRIC_INCH / y_dpi;
	return;
}



bool ClearBackground(HDC hdc, RECT screen, RECT diff)
{
	SetLastError(0);
	if (screen.top < diff.top) {			// �㑤�N���A
		Rectangle(hdc, screen.left, screen.top, screen.right, (diff.top));
	}
	if (screen.bottom > diff.bottom) {	// �����N���A
		Rectangle(hdc, screen.left, max(diff.bottom, screen.top),
			screen.right, screen.bottom);
	}
	if (screen.left < diff.left) {		// �c��̍����N���A
		Rectangle(hdc, screen.left, max(diff.top, screen.top),
			diff.left, min(diff.bottom, screen.bottom));
	}
	if (screen.right > diff.right) {		// �E���N���A
		Rectangle(hdc, diff.right, max(diff.top, screen.top),
			screen.right, min(diff.bottom, screen.bottom));
	}
	return GetLastError() == NO_ERROR;
}

}  // namespace