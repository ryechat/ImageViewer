/*!
    @file �⏕�֐��Q�B
*/
#pragma once
#ifndef GUID_FE280A758E3643D8A9E8C30E948F6C90
#define GUID_FE280A758E3643D8A9E8C30E948F6C90

#ifndef STDAFX_H
#include <wtypes.h>
#include <tchar.h>
#include <string>
#include <functional>    // for SafeRelease
#endif

namespace
{
    using tstr = std::basic_string<TCHAR>;
}

namespace basis {

    extern const TCHAR * const TEXT_EMPTY;

// �X�R�[�v�A�E�g���Ƀ|�C���^���ǂ��ɂ�����|�C���^�L�����A
// ��̎��̓���������_���Ŏ󂯎��ȊO��std::unique_ptr�Ɠ���
template<class T>
class SafeRelease {
    using deleter = std::function<void(T*)>;
    deleter m_f;
    T *m_p;
public:
    // �|�C���^�ƃ����_����^����
    SafeRelease(T* p, deleter f)
        : m_p(p), m_f(f) {}

    SafeRelease() : m_p(0) {}
    ~SafeRelease() { if(m_p && m_f) m_f(m_p); }

    SafeRelease(SafeRelease&) = delete;
    SafeRelease&operator=(SafeRelease&) = delete;
    SafeRelease(SafeRelease&&) = default;
    SafeRelease&operator=(SafeRelease&&) = default;

    // �|�C���^�Ƃ��ĐU�镑��
    explicit operator bool() { return m_p != nullptr; }
    operator T*() { return m_p; }
    T* get() { return m_p; }
    T& operator*() { return *m_p; }
    T* operator->() { return m_p; }
    // �~���[�e�[�^�[
    T* operator=(const T* p) { m_p = p; }
    void operator=(deleter f) { m_f = f; }
};



//! �o�C�g�I�[�_�[���t�ɂ���(4Bytes)
uint32_t ReverseByteOrder(uint32_t n);

//! ���o�C�g���ɉ����Đ����^�̏�����������\������
//! d_x ��%d �Ƃ� %x ��d�Ƃ�x�̕������w��
PTSTR GetTypeFormat(TCHAR type[5], size_t bytes, TCHAR d_x);

//! �����l�𕶎���^�ɏ����o��
tstr ToStr(int64_t n);



template<typename T>
inline T ToInt(const TCHAR *p, const T &nDefault)
{
    TCHAR d_x = (p[0] == _T('0') && p[1] == _T('x'))
        ? p += 2, _T('x')    // 0x
        : _T('d');    // Decimal

    TCHAR type[5];
    GetTypeFormat(type, sizeof(T), d_x);

    T i{ 0 };
    if (*type == _T('\0') || _stscanf_s(p, type, &i) != 1)
        return nDefault;

    return i;
}



template<>
bool ToInt(const TCHAR *p, const bool &bDefault);



//! �R�}���h���C����nCmd�Ԗڂ̈�����Ԃ��B
/*! 0�Ԗڂ͎��s���̃v���O�����̃p�X�A�ȍ~�������ł���B
    ������""(�_�u���N�H�[�g)�ň͂܂�Ă���ꍇ�͂������������B
    "path" �� -ls �ȂǍ��݂��������ł����삷��B
    �߂�l��basic_string<TCHAR>�^�ŁA�Ώۂ̈������Ȃ��ꍇ�͋�(empty)�ł���B
*/
tstr GetCommandLine(int nCmd);



/*! WM_DROPFILES���b�Z�[�W�����ł����Ƃ���
    num�Ԗڂ̃t�@�C�������擾����
*/
tstr GetDropFile( WPARAM wp, UINT num = 0 );

//! �w�肳�ꂽ�t�H���g�̂����ő�̍������������B
/*!    ��3������0�̏ꍇ�͑�2�����̍������A
    ��2������0�̏ꍇ�̓J�����g�t�H���g�̍������������B
*/
LONG GetFontHeight( HDC hdc, HFONT lhs, HFONT rhs );

//! �t�@�C���̃v���p�e�B��\������
bool ShowProperty(HWND hParentWnd, const tstr &pszFile);

//! OLE�p�w���p�[�֐��BOLE_HIMETRIC�P��(mm�P��)�ւ̕ϊ�
void HimetricFromDP(HDC hdc, SIZE& s);

//! ��` screen ���� foreground ���������̈��Rectangle()���g���ēh��Ԃ��B
bool ClearBackground(HDC hdc, RECT screen, RECT foreground);

}  // namespace

#endif