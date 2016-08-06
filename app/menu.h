/*!
    @file Class definition of CImageViewer::ContextMenu.
    That class provides the application the way to manage
    context menu item's status.
*/
#pragma once
#ifndef GUID_56ED3B352A5D42EBA6E9E85670749C89
#define GUID_56ED3B352A5D42EBA6E9E85670749C89
#include "image_viewer.h"

namespace image_viewer {

enum class ID : int;

//! Menu Controls
class CImageViewer::ContextMenu {
public:
    ContextMenu(CImageViewer &parent_);
    ~ContextMenu();

    /*! ���j���[��Ԃ𕜌����A�A�v���P�[�V�����ݒ�ɔ��f����.
        �E�B���h�E�𐶐����Ă���Ăяo�����ƁB
    */
    void initialize();

    //! ���j���[���ڂ̏�Ԃ�ݒ�t�@�C���ɕۑ�����
    void saveSettings();
 
    //! �A�v���P�[�V�����̏�Ԃɍ��킹�ă��j���[���ڂ̗L��/������؂�ւ���
    bool updateStatus();

    /*! ���ڂ̃`�F�b�N��Ԃ𔽓]����.
        ���W�I�{�^���̏ꍇ��ON�ɂȂ�A�O���[�v���̑��̃��W�I�{�^����OFF�ɂȂ�
    */
    bool changeStatus(ID id);

    //! ���j���[��\�����A���[�U�̑I����Ԃ��B�L�����Z����0�B
    int track(basis::Point pt) const;

    //! �I������Ă���\�[�g������Ԃ�
    ID getSortWay();

    //! ���ڂ�L��������
    void enable(ID id);

    //! ���ڂ𖳌�������
    void disable(ID id);

    //! ���ڂ��`�F�b�N��Ԃɂ���
    void select(ID id);

    //! ���ڂ��`�F�b�N��Ԃ��ǂ���
    bool isSelected(ID id);

    //! ���ڂ̃`�F�b�N��Ԃ�����
    void clear(ID id);

private:
    CImageViewer& parent;
    class Impl;
    std::unique_ptr<Impl> impl;
};

}  // namespace

#endif