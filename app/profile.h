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

    //! セクション指定
    Profile& general() { return section(GENERAL); }
    //! セクション指定
    Profile& menu() { return section(MENU);    }
    //! セクション指定
    Profile& window() { return section(WINDOW);    }
    //! セクション指定
    Profile& control() { return section(CONTROL); }

    /*! セクション内のすべての項目に関数オブジェクトを適用.
        渡されるのは項目のIDと、それに対応する値文字列へのポインタ。
        この関数はControlクラスからキーコマンドの割り付けに使用される。
    */
    void applyToAllItemInTheSection(std::function<void(ID, const TCHAR *)> f);

    //! キー名を返す
    const TCHAR * getKeyString(ID id);

    //! 言語ファイルにある対訳を返す。なければキー名を返す
    const TCHAR * getTranslatedString(ID id);

    //! ブーリアン型プロファイルを読む
    bool loadBoolean(ID id, bool bDefault);
    //! ブーリアン型プロファイルを書く
    bool saveBoolean(ID id, bool b);

    //! 整数型プロファイルを読む
    int load(ID id, int nDefault);
    //! 整数型プロファイルを書く
    bool save(ID id, int n);

    /*! 文字列型プロファイルを読む.
        なければsDefaultのコピーを返し、nullptrが返ることはない。
        sDefaultはnullptrの場合、空文字列として扱われる。
    */
    const TCHAR * load(ID id, const TCHAR *sDefault);
    //! 文字列型プロファイルを書く
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
