#pragma once
#ifndef GUID_E78E8F5DD24646759E45068A93FEFBCC
#define GUID_E78E8F5DD24646759E45068A93FEFBCC

#include "iunit_test.h"
#include "types.h"

namespace basis {

UNIT_TEST(Cursor);

/*! �J�[�\���n���h���N���X.
	�֘A����API�̃��b�p�[�B
	�J�[�\���̃n���h����\���N���X�ŁA
	�R�s�[���Ă��J�[�\�����Q�ɂȂ�Ȃ�Ă��Ƃ͂Ȃ��i�c�O�j
*/
class Cursor {
public:
	enum class ID : int;

	Cursor() {};
	~Cursor() {};

	//! �V�X�e���J�[�\���ɕύX����
	bool set(ID id);

	//! �t�@�C���̃J�[�\���ɕύX
	bool set(const TCHAR *fileName);

	//! �n���h���Ŏw�肵���J�[�\���ɕύX
	bool set(HCURSOR hCursor);

	//! �J�[�\���`������ɖ߂�
	void reset();

	//! �J�[�\���ʒu��������
	Point pos();

	//! �J�[�\���ʒu���w�肷��
	bool pos(Point pt);

	//! �}�E�X���L���v�`������
	void capture(HWND hWnd);

	//! �}�E�X�L���v�`��������
	void release();

	//! �J�[�\���\�����C���N�������g
	bool show();

	//! �J�[�\���\�����f�N�������g
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