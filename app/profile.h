#pragma once
#ifndef GUID_88E6B0C9D24C48378AB9ED3883518A73
#define GUID_88E6B0C9D24C48378AB9ED3883518A73
#ifndef STDAFX_H
#include <functional>
#endif
#include "private_profile.h"
#include "image_viewer.h"

namespace image_viewer {
    enum class ID : int;

class CImageViewer::Profile {
public:
    Profile();
    Profile(Profile&) = delete;

    bool exist();
    bool isEnable() { return m_enable; }
    bool enable() { return setEnable(true); }
    bool disable() { return setEnable(false); }

    //! �Z�N�V�����w��
    Profile& general() { return section(GENERAL); }
    //! �Z�N�V�����w��
    Profile& menu() { return section(MENU);    }
    //! �Z�N�V�����w��
    Profile& window() { return section(WINDOW);    }
    //! �Z�N�V�����w��
    Profile& control() { return section(CONTROL); }

    /*! �Z�N�V�������̂��ׂĂ̍��ڂɊ֐��I�u�W�F�N�g��K�p.
        �n�����͍̂��ڂ�ID�ƁA����ɑΉ�����l������ւ̃|�C���^�B
        ���̊֐���Control�N���X����L�[�R�}���h�̊���t���Ɏg�p�����B
    */
    void applyToAllItemInTheSection(std::function<void(ID, const TCHAR *)> f);

    //! �L�[����Ԃ�
    const TCHAR * getKeyString(ID id);

    //! ����t�@�C���ɂ���Ζ��Ԃ��B�Ȃ���΃L�[����Ԃ�
    const TCHAR * getTranslatedString(ID id);

    //! �u�[���A���^�v���t�@�C����ǂ�
    bool loadBoolean(ID id, bool bDefault);
    //! �u�[���A���^�v���t�@�C��������
    bool saveBoolean(ID id, bool b);

    //! �����^�v���t�@�C����ǂ�
    int load(ID id, int nDefault);
    //! �����^�v���t�@�C��������
    bool save(ID id, int n);

    /*! ������^�v���t�@�C����ǂ�.
        �Ȃ����sDefault�̃R�s�[��Ԃ��Anullptr���Ԃ邱�Ƃ͂Ȃ��B
        sDefault��nullptr�̏ꍇ�A�󕶎���Ƃ��Ĉ�����B
    */
    const TCHAR * load(ID id, const TCHAR *sDefault);
    //! ������^�v���t�@�C��������
    bool save(ID id, const TCHAR * value);

private:
    bool setEnable(bool bEnable);
    Profile& section(const TCHAR *p) {
        if (!p)
            throw 0;
        m_prof.section(p); return *this;
    }
    const TCHAR * ToStr(bool b);

    struct Pair { ID id; const TCHAR *key; };
    static Pair m_profile_ids[];
    static constexpr TCHAR * const WINDOW = TEXT("Window");
    static constexpr TCHAR * const CONTROL = TEXT("Control");
    static constexpr TCHAR * const MENU = TEXT("Menu");
    static constexpr TCHAR * const GENERAL = TEXT("General");
    static constexpr TCHAR * const LANGUAGE = TEXT("Language");
    static constexpr TCHAR * const kSettingFile = TEXT("setting.ini");
    static constexpr TCHAR * const kLanguageFile = TEXT("language.ini");

    bool m_enable;
    basis::CPrivateProfile m_prof, m_lang;
};

}  // namespace

#endif
