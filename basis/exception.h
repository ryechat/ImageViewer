#pragma once
#ifndef GUID_92604BE061ED490DB4ED5ACF19CCBF4E
#define GUID_92604BE061ED490DB4ED5ACF19CCBF4E

#ifndef STDAFX_H
#include <exception>
#include <string>
#endif

//! ���o��������O�N���X�̃R���X�g���N�^�ɓn��������
#define LOCATION ::basis::MakeLocation(__func__, __FILE__, __LINE__)

//! API���G���[�̂Ƃ��ɓ������O
#define api_runtime_error()										\
std::runtime_error(::basis::GetErrorMessage(__func__, __FILE__, __LINE__))

namespace basis {

/*! ��O
	@file
	std::exception�̔h���N���X�����N���X�Ƃ���ƁA
	virtual�w�肪�Ȃ����߂�std::exception�Ƃ���catch���邱�Ƃ͂ł��Ȃ��B
	���̂���C++�W���̗�O�N���X�݂̂��g�����Ƃɂ���B


	��P�j���o��������O�N���X��LOCATION�}�N���̕������n��

		throw std::invalid_argument(LOCATION);


	��Q�jAPI�G���[�̏ꍇ�͐�p�}�N����runtime_error��O�����B
		�@���̂Ƃ��AGetLastError()�̐��������ǉ������B
		
		throw api_runtime_error();



	���l�@
	�W����O�̓}�N���ɂ��t�@�C�����Ȃǂ��܂߂邱�Ƃ��ł��邪�A
	���\�b�h�̈����ɂ킽���ꂽbasic_string<TCHAR>�������Ȃ��B
	�꒷��Z�ł���̂ŁA���[�U��`��O�N���X�����Ƃ���
	��O�����ꏊ��m�点��char�^�̕�����ƁA
	������G���[���e��m�点��TCHAR�^�̕�������������A
	MessageBoxA() �� MessageBox()�̂Q�i�K�Œʒm����������
*/

//! LOCATION �}�N���̎���
std::string
MakeLocation(const char *func, const char *file, int line);

//! api_runtime_error �p���b�Z�[�W
std::string
GetErrorMessage(const char *func, const char *file, int line);

}  // namespace

#endif // exception_h