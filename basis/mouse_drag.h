/*!
@file mouse_drag.h	2015/12/12

�}�E�X�h���b�O�̃��X�i�N���X
�E�B���h�E���b�Z�[�W����h���b�O�Ɋ֘A���鑀��𒊏o��
�h���b�O�̊J�n�E���s�E�I���̃^�C�~���O��ʒm�A
�h���b�O�ʂȂǂ̊֐���񋟂���
*/
#pragma once
#ifndef CMouseDrag_h
#define CMouseDrag_h

#ifndef STDAFX_H
#include <wtypes.h>
#endif
#include "types.h"

// �}�E�X�֘A��WM_�ł킽�����{�^�����(WPARAM)��ALT�L�[�������킦�āA
// .m_keys�ŊJ�n���̃{�^��������Ԃ��Ǘ�����
// ���ɂ���Ă�MK_ALT�����݂���炵���̂ł��̖��O�͔�����
#ifndef MK_MENU
#define MK_MENU 0x80
#endif
// ����WM_LBUTTONDOWN�ł킽�����WPARAM�Ɠ���

namespace basis {

enum class Message : int;

class CMouseDrag {
public:
	using Point = basis::Point;
	using Size = basis::Size;
	enum STATE : int {
		UNDEFINED = 0,
		BUTTON_UP = 1,
		BUTTON_DOWN = 2,
		MOUSE_MOVE = 4,
		SLIGHT_MOVE = BUTTON_DOWN + MOUSE_MOVE,
		MOUSE_DRAG = 8,
		DRAGGING = MOUSE_MOVE + MOUSE_DRAG,
	};

	CMouseDrag();

	// ���b�Z�[�W����͂��h���b�O�̊J�n�E���s�E�I������true��������
	bool proc(HWND hWnd, basis::Message msg, WPARAM wp, LPARAM lp);

	// �h���b�O�̊J�n�n�_��������
	Point start() { return m_start; }

	// 
	Point pos() { return m_pos; }

	// �O��WM_MOUSEMOVE����̉������ړ�������Ԃ�
	int dx();

	// �O��WM_MOUSEMOVE����̏c�����ړ�������Ԃ�
	int dy();

	Size getDifference();

	// �X�N���[�����W��̃}�E�X�ړ������݂̂�Ԃ�
	// �E�B���h�E�̈ړ��ɂ�鑊�ΓI�ړ��͖��������
	int dx_abs();
	int dy_abs();
	Size getTravel();

	// �h���b�O�J�n���̕����}�E�X�L�[�X�e�[�g
	UINT keys() { return m_keys; }

	// �h���b�O�J�n�̃g���K�[�ƂȂ����{�^����virtual key
	UINT vkey() { return m_vkey; }

	int state() { return m_state; }

	bool isMoved()
	{
		return (static_cast<int>(m_state)
			& static_cast<int>(STATE::MOUSE_MOVE)) != 0;
	}

	bool isDragged()
	{
		return (static_cast<int>(m_state)
			& static_cast<int>(STATE::MOUSE_DRAG)) != 0;
	}

	// �h���b�O�J�n�Ƃ݂Ȃ�������ݒ肷��(�����l1)
	// �N���b�N�n�_���炱�̕����������܂ō������������Ȃ�
	// pixel���ȗ������ꍇ�͌��ݒl��������
	Size threshold(int dx = -1, int dy = -1);

protected:
	// �X���b�V�����h��(���ł�)�z�������ǂ���
	bool isOveringThreshold(Point p);
	// m_prev, m_pos���X�V
	Point& updatePos(HWND hWnd, const LPARAM lp);

private:
	Point m_pos;		// �N���C�A���g���W
	Point m_scr;		// �X�N���[�����W
	Point m_start;
	Point m_prev;
	Point m_prev_scr;
	UINT m_keys;
	UINT m_vkey;

	// ���ꂪBUTTON_UP�̂Ƃ��̓h���b�O���ł͂Ȃ�
	int m_state;

	// ���̒l�ȏ�ɂȂ�܂Ńh���b�O�Ƃ݂Ȃ��Ȃ�(proc��true���������Ȃ�)
	Size m_threshold;
};

}  // namespace

#endif