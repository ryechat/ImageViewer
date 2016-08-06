#pragma once
#ifndef GUID_E47B5B4E32ED48FF9396F82D9615F6B1
#define GUID_E47B5B4E32ED48FF9396F82D9615F6B1

#ifndef STDAFX_H
#include <wtypes.h>
#include <vector>
#endif
#include "types.h"

namespace basis {

// ���j���[�B
// ��̎��ɏ��L���̂��郁�j���[�E�T�u���j���[�͉�̂����B
class CPopupMenu {
public:
    CPopupMenu() : CPopupMenu(CreatePopupMenu()) {}
    virtual ~CPopupMenu();

    CPopupMenu(const CPopupMenu&) = default;
    CPopupMenu& operator=(const CPopupMenu&) = default;
    CPopupMenu(CPopupMenu&&rhs) noexcept { *this = std::move(rhs); }
    CPopupMenu& operator=(CPopupMenu&&rhs) noexcept;

    CPopupMenu(HMENU h);

    // ���ڂ�position�̑O�ɑ}��
    // �����Ȃ�0�܂���(UINT)-1�ɂ��邱��
    void insert(int position, int id, const TCHAR *title);

    // �Z�p���[�^��}��
    void insertSeparator(int position);

    // �q���j���[��position�̑O�ɑ}��
    // ���j���[�n���h���͐e���j���[�ɈϏ������
    void insert(int position, CPopupMenu& submenu, const TCHAR *title);

    // �w�肳�ꂽid�������ڂ���菜��
    void erace(int id) noexcept;

    // ���ׂĂ̍��ځA�T�u���j���[��؂藣���i�j�󂳂�Ȃ��j
    void detach() noexcept;

    // ���j���[�̏��L����������A�n���h����������
    HMENU release() noexcept;

    // �ύX�𔽉f����Btrack�O�ɕK�����s���邱��
    void redraw(HWND) const;

    /*!    �V���[�g�J�b�g���j���[��\������.
        �\�����͏������u���b�N�����B
        @param position �\������X�N���[�����W
        @return item ID, if user selected. Otherwise, 0.
    */
    int track(HWND hWnd, basis::Point pos) const;

    // first~last�̂���id������I����Ԃɂ���
    void radio(int first, int last, int id) const;

    // �`�F�b�N�}�[�N
    void select(int id) const;
    void clear(int id) const;
    bool isSelected(int id) const;

    bool isRadioButton(int id) const;
    bool invert(int id) const;        // �߂�l��isSelected

    // �L����/������
    void enable(int id) const;
    void disable(int id) const;
    bool isEnable(int id) const;
    bool invertEnable(int id) const;    // �߂�l��isEnable

protected:
    int state(int id, int stat = -1) const;
    int  stateHelper(int id, int flag, int modify) const;

    HMENU m_h;
    MENUITEMINFO *mi;
};

}  // namespace

#endif

