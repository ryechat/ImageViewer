#pragma once
#ifndef GUID_66F5CCBBF45C4752AB6FD2D76BD57BFF
#define GUID_66F5CCBBF45C4752AB6FD2D76BD57BFF

#include "file_path.h"
#include "cdd_bitmap.h"
#include "iunit_test.h"

namespace image_viewer {

UNIT_TEST(CListItem)

/*!
	CImageViewer::Impl::Filer�ɂ�
	���X�g�ŊǗ�����摜�t�@�C���N���X�B
	CFindData����̂ݐ��������
*/
class CListItem {
public:
	using tstr = std::basic_string<TCHAR>;
	CListItem(const WIN32_FIND_DATA&);
	~CListItem() = default;

	CListItem&operator=(const CListItem&) = default;
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

	// �O���g�p�B�r�b�g�}�b�v��������邩�ǂ����̔���Ɏg��
	bool bUnload;

	// �O���g�p�B���X�g���C���f�b�N�X
	int index;

	const TCHAR * fileName() const { return m_fileName.c_str(); }

	bool	isLoadingFailed() { return m_type == TYPE::Error; }

	void	unload() { m_image.reset(); }

	bool	isLoaded() const { return m_image.operator bool(); }

	bool draw(HDC dest, const RECT& rcDest, const RECT& rcSrc) {
		return m_image.transfer(dest, rcDest, rcSrc);
	}

	basis::Size	getSize()   const { return m_image.getSize(); }
	basis::Rect rect() const {
		auto size = m_image.getSize();
		return{ 0, 0, size.x, size.y };
	}

	FILETIME ftAccess() const { return m_access; }
	FILETIME ftCreate() const { return m_create; }
	FILETIME ftWrite()  const { return m_write;  }

private:
	basis::Surface m_image;
	basis::StringBuffer m_fileName;
	FILETIME  m_access;
	FILETIME  m_create;
	FILETIME  m_write;

	using TYPE = basis::CDDBitmap::TYPE;
	TYPE m_type;
};

}  // namespace

#endif