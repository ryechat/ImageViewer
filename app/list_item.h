#pragma once
#ifndef GUID_66F5CCBBF45C4752AB6FD2D76BD57BFF
#define GUID_66F5CCBBF45C4752AB6FD2D76BD57BFF

#ifndef STDAFX_H
#include <string>
#include <memory>
#endif

#include "iunit_test.h"
#include "types.h"
namespace basis { class CFilePath; }

namespace image_viewer {

UNIT_TEST(CListItem)

/*!
	CImageViewer::Filer�ɂă��X�g�Ǘ������A�C�e���N���X�B
	CFindData����̂ݐ��������
*/
class CListItem {
public:
	CListItem(const WIN32_FIND_DATA&);
    ~CListItem();
	CListItem(CListItem &&) = default;
	CListItem&operator=(CListItem && ) = default;

	enum class Status : BYTE {
		Undefined = 0,
		NotExist = 1,
		CannotOpen = 2,
		SizeError = 4,
		MemoryError = 8,
		TypeError = 16,
		LoadError = 32,
		Loaded = 64,
		Release = 128,
	};

	/*!	�摜�����[�h����B
		�t�@�C�����J���Ȃ������ꍇ��Status::CannotOpen��Ԃ��B
		����ȊO�̗��R�Ŏ��s�����ꍇ�Am_type��TYPE::Error�ɐݒ肵�A
		�Ή�����G���[�X�e�[�^�X��Ԃ��B
		���������ꍇ��Status::Loaded��Ԃ��B
	*/
	Status	loadImage(::basis::CFilePath path);


    //! �t�@�C������Ԃ��B�f�B���N�g�����͊܂܂Ȃ�
    const TCHAR * fileName() const;

    //! ���[�h�Ɏ��s�����t�@�C�����ǂ�����Ԃ�
    bool	isLoadingFailed();

    //! ���[�h�ς݂̃t�@�C�����A�����[�h����
    void	unload();

    //! ���[�h�ς݂��ǂ�����Ԃ�
    bool	isLoaded() const;

    //! ���[�h�ς݂ł���ΑΏۂ̃f�o�C�X�R���e�L�X�g�ɕ`�悷��
    bool draw(HDC hdc, const RECT& destination, const RECT& source);

    basis::Size	getSize() const; //!< �摜�T�C�Y��Ԃ�
    basis::Rect getRect() const; //!< �n�_0,0�̉摜��`��Ԃ�

    FILETIME ftAccess() const;
    FILETIME ftCreate() const;
    FILETIME ftWrite()  const;

	// �O���g�p�B���X�g���C���f�b�N�X
	int index;

    // �T�Z�������g�p�ʁB���[�h�O�̂݃t�@�C���T�C�Y�ŁA���[�h���邽�эX�V
    int weight;

	// �O���g�p�B�r�b�g�}�b�v��������邩�ǂ����̔���Ɏg��
	bool bUnload;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}  // namespace

#endif