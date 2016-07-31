#pragma once
#ifndef GUID_E78E8F5DD24646759E45068A93FEFBCC
#define GUID_E78E8F5DD24646759E45068A93FEFBCC

#include "iunit_test.h"
#include "types.h"

namespace basis {

UNIT_TEST(Cursor);

/*! カーソルハンドルクラス.
	関連するAPIのラッパー。
	カーソルのハンドルを表すクラスで、
	コピーしてもカーソルが２つになるなんてことはない（残念）
*/
class Cursor {
public:
	enum class ID : int;

	Cursor() {};
	~Cursor() {};

	//! システムカーソルに変更する
	bool set(ID id);

	//! ファイルのカーソルに変更
	bool set(const TCHAR *fileName);

	//! ハンドルで指定したカーソルに変更
	bool set(HCURSOR hCursor);

	//! カーソル形状を元に戻す
	void reset();

	//! カーソル位置をかえす
	Point pos();

	//! カーソル位置を指定する
	bool pos(Point pt);

	//! マウスをキャプチャする
	void capture(HWND hWnd);

	//! マウスキャプチャを解除
	void release();

	//! カーソル表示をインクリメント
	bool show();

	//! カーソル表示をデクリメント
	bool hide();

private:
	class Impl;
	Impl& get();
};



//! Same as OCR_ numbers defined in winuser.h
enum class Cursor::ID : int
{
	APPSTARTING = 32650, //!< Standard arrow and small hourglass
	ARROW = 32512, //!< Standard arrow
	CROSS = 32515, //!< Crosshair
	HAND = 32649, //!< Hand
	HELP = 32651, //!< Arrow and question mark
	IBEAM = 32513, //!< I - beam
	ICON = 32641, //!< Obsolete for applications marked version 4.0 or later.
	NO = 32648, //!< Slashed circle
	SIZE = 32640, //!< Obsolete for applications marked version 4.0 or later.Use IDC_SIZEALL.
	SIZEALL = 32646, //!< Four - pointed arrow pointing north, south, east, and west
	SIZENESW = 32643, //!< Double - pointed arrow pointing northeast and southwest
	SIZENS = 32645, //!< Double - pointed arrow pointing north and south
	SIZENWSE = 32642, //!< Double - pointed arrow pointing northwest and southeast
	SIZEWE = 32644, //!< Double - pointed arrow pointing west and east
	UPARROW = 32516, //!< Vertical arrow
	WAIT = 32514, //!< Hourglass
};

}  // namespace

#endif