#pragma once
#ifndef GUID_5718854C303D43FF961DFBE2E38A4EA6
#define GUID_5718854C303D43FF961DFBE2E38A4EA6

#ifndef STDAFX_H
#include <string>
#include <memory>
#include <list>
#endif

#include "image_viewer.h"

namespace image_viewer {

//! ���X�g�Ǘ����s�������N���X
class CImageViewer::CFiler {
public:
	CFiler(CImageViewer &parent_) : m_parent(parent_), m_current(end()) {}
	~CFiler() = default;

	//! �f�B���N�g�����̃t�@�C�������X�g������B
	/*! @par	�֐����s�O�ɕێ����Ă������X�g�͉������A
				CFileEnumNoDirectories �ɂ���ė񋓂����t�@�C�����i�[�����B
		@par	�擪�t�@�C�����J�����g�t�@�C���ƂȂ�B
		@param dir	�ΏۂƂȂ�f�B���N�g��
	*/
	void generate(std::basic_string<TCHAR> dir);

	/*!	�A�C�e�������X�g����폜����B
		@details	�w�肳�ꂽ�A�C�e�����J�����g�t�@�C���̏ꍇ�A
					�J�����g�C�e���[�^�͎��̃t�@�C���Ɉړ������B
					���̃t�@�C�����Ȃ��ꍇ�͑O�A
					������Ȃ����end�v�f�Ɉړ������B
		@param itr	�ΏۂƂȂ�A�C�e��
		@return		�J�����g�C�e���[�^
	*/
	iterator erase(iterator itr) { return erase(itr, itr); }

	//! [first, last) �͈̗̔͂v�f���폜����
	iterator erase(iterator first, iterator last);

	/*! ���X�g�̂��ׂĂ̗v�f���폜����B
		@details �J�����g�C�e���[�^��end()�Ɉړ������B
	*/
	void clear() {
		m_list.clear();
		m_current = end();
	}

	/*! ���X�g���\�[�g����B
		@details ���j���[�Ŏw�肳�ꂽFILETIME��r�Ń\�[�g�����B
	*/
	void sort();

	//! �J�����g�C�e���[�^���擾�B��̏ꍇ��end()
	iterator current() const { return m_current; }

	//! �J�����g�C�e���[�^��ݒ�
	void setCurrent(iterator itr) { m_current = itr; }

	const_iterator cbegin() const { return m_list.cbegin(); }
	const_iterator cend() const { return m_list.cend(); }
	const_iterator clast() const { return isEmpty() ? cend() : --cend(); }

	iterator begin() { return m_list.begin(); }
	iterator end() { return m_list.end(); }
	iterator last() { return isEmpty() ? end() : --end(); }

	bool isEmpty() const { return m_list.empty(); }
	size_t size() const { return m_list.size(); }


	// ���X�g���̃C���f�b�N�X��Ԃ�
	int indexof(const_iterator itr) const;

	// �w��ʒu�ȍ~�̃A�C�e���ɁA���X�g���̃C���f�b�N�X��^����
	void giveIndices(iterator iStart, int index);

	iterator search(std::function<bool(Element&)> func);

	// �L���ȃC�e���[�^�݂̂�ΏۂɁAnCount�Ԃ�ړ�����
	iterator move(iterator itr, int nCount);

private:
	bool compare(const Element&, const Element&, int sortWay);

	CImageViewer &m_parent;
	ListTy m_list;
	iterator m_current;
};

}  // namespace

#endif
