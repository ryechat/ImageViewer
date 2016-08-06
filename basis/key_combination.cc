#include "key_combination.h"
#include "stdfnc.h"
#include "exception.h"
#include "char_t.h"

namespace {

int perseFunctionKey(const TCHAR *str, size_t size)
{
    if (!str || *str != TEXT('F'))
        return 0;

    ::basis::Char_T<TCHAR> c;
    if (size < 2 || (c = str[1]) == TEXT('0') || !c.isDigit())
        return 0;

    if (size > 3 || (size == 3 && !(c = str[2]).isDigit()))
        return 0;

    return ::basis::ToInt(str + 1, 1) + VK_F1 - 1;
}

}  // namespace

namespace basis {

std::map<unsigned char, tstr> CKey::Text;



CKey::CKey() : m_keys(0)
{
    if (Text.empty()) {
        Text = {
            { VK_SHIFT,        TEXT("Shift") },
            { VK_CONTROL,    TEXT("Ctrl") },
            { VK_MENU,        TEXT("Alt") },
            { VK_DELETE,    TEXT("Delete") },
            { VK_ESCAPE,    TEXT("Esc") },
            { VK_BACK,        TEXT("Back") },
            { VK_RETURN,    TEXT("Enter") },
            { VK_TAB,        TEXT("Tab") },
            { VK_INSERT,    TEXT("Insert") },
            { VK_HOME,        TEXT("Home") },
            { VK_END,        TEXT("End") },
            { VK_UP,        TEXT("Up") },
            { VK_DOWN,        TEXT("Down") },
            { VK_LEFT,        TEXT("Left") },
            { VK_RIGHT,        TEXT("Right") },
            { VK_SPACE,     TEXT("Space") },
        };
    }
}



/*! 変換コンストラクタ
    @param key
    仮想キーコード。
    クラス定数の修飾キーを組み合わせても良い。
    下位WORDのみが使われ、上位WORDは無視される。
    @par
    以下の値はすべて同一のキーに変換される
    -VK_SHIFT
    -CKey::Shift
    -VK_SHIFT | CKey::Shift
*/
CKey::CKey(DWORD key) : m_keys(static_cast<WORD>(key & 0xFFFF))
{
    // 修飾キーのVKEYならば、修飾キーとしても使う
    if (vkey() == VK_SHIFT)
        m_keys |= SHIFT;
    else if (vkey() == VK_CONTROL)
        m_keys |= CTRL;
    else if (vkey() == VK_MENU)
        m_keys |= ALT;

    if (!vkey())    // vkeyがなければ修飾キーを使う
        descendToVKey();
}



CKey& CKey::read(const TCHAR *p)
{
    tstr t = p;
    tstr sub;
    size_t size;
    m_keys = 0;

    Char_T<TCHAR> c;

    for (;;) {
        size = termLength(t.c_str());
        if (!size)
            return *this;
        sub = t.substr(0, size);
        t = t.substr(size);

        c = sub.c_str()[0];
        if (c.isDigit()) {
            m_keys = flags() | (ToInt(sub.c_str(), 0) & 0xFF);
            return *this;
        }

        if (c.isAlpha()) {
            // ファンクションキー
            int vkey_f = perseFunctionKey(sub.c_str(), sub.size());
            if (vkey_f >= VK_F1 && vkey_f <= VK_F24) {
                m_keys = flags() | static_cast<WORD>(vkey_f);
                return *this;
            }

            // 文字キー
            if (sub.size() <= 1) {
                // _T("v")を'V'に変換
                m_keys = flags()
                    | (toupper(toascii(sub.c_str()[0])) & 0xFF);
                return *this;
            }

            // ネームドキー(Enterなど)
            for (auto i : Text) {
                if (!sub.compare(i.second)) {
                    CKey key{ i.first };
                    if (key.isModifier()) {
                        *this |= key;
                        break;
                    }
                    m_keys = flags() | key.vkey();
                    return *this;
                }
            }
        }
    }
}



CKey CKey::
operator&(const CKey& rhs) const
{
    CKey key{ (vkey() == rhs.vkey())
        ? vkey()
        : static_cast<DWORD>(0) };
    key.m_keys |= flags() & rhs.flags();

    if (!key.vkey())
        key.descendToVKey();
    return key;
}



CKey CKey::operator|(const CKey& rhs) const
{
    if (!isModifier()) {
        // 組み合わせられない
        if (!rhs.isModifier())
            throw std::invalid_argument(0);

        // 右が修飾キー
        return m_keys | rhs.flags();
    }

    // 両方修飾キー
    if (rhs.isModifier())
        return flags() | rhs.flags();

    // 左が修飾キー
    return flags() | rhs.m_keys;
}



tstr CKey::toStr()
{
    tstr t;
    unsigned char flags[3]{ VK_SHIFT, VK_CONTROL, VK_MENU };

    // 修飾キー
    for (auto i : flags) {
        if (*this & i) {
            Plus(t, Text[i]);
        }
    }

    if (vkey() >= VK_F1 && vkey() <= VK_F24) {
        // ファンクションキー
        Plus(t, TEXT("F"));
        t += ToStr((vkey() - VK_F1 + 1));
        return t;
    }

    if (vkey() <= (std::numeric_limits<char>::max)()) {
        basis::Char_T<char> c{ static_cast<char>(vkey()) };
        if (c.isAlpha() || c.isDigit()) {
            Plus(t, tstr() += c.tchar());
            return t;
        }
    }

    try {
        // ネームドキー
        Plus(t, Text.at(vkey()));
    }
    catch (std::out_of_range &) {
        // その他、文字キー（数字表現）
        Plus(t, ToStr(vkey()));
    }
    return t;
}




size_t CKey::termLength(const TCHAR *p, size_t nStart)
{
    size_t n = nStart;
    if (_tcslen(p) <= n)
        return 0;

    // 指定子
    bool bHex = (p[n] == _T('0') && p[n + 1] == _T('x'));
    if (bHex) {
        n += 2;
    }

    // 数値
    bool accept_comma = (n == nStart);
    for (; p[n]; ++n) {
        if (p[n] == _T('.') && n != nStart && accept_comma) {
            accept_comma = false;
            continue;
        }
        if (bHex) {
            if (!Char_T<TCHAR>(p[n]).isHexDigit())
                break;
        }
        else {
            if (!Char_T<TCHAR>(p[n]).isDigit())
                break;
        }
    }
    if (n != nStart)
        return n - nStart;

    // 変数名
    for (Char_T<TCHAR> c; ; ++n) {
        if ((c = p[n]) == _T('\0')
            || !(c.isAlpha() || c.isDigit()))
            break;
    }
    if (n != nStart)
        return n - nStart;

    // スペースとタブ、カンマ
    for (; p[n]; ++n) {
        if (p[n] == _T(' ') || p[n] == _T('\t'))
            continue;
        if (p[n] == _T(',') || p[n] == _T('　'))
            continue;
        break;
    }
    if (n != nStart)
        return n - nStart;

    // operator
    if (p[n] == _T('+') || p[n] == _T('-')) {
        if (p[n + 1] == p[n] || p[n + 1] == _T('='))
            return 2;
        return 1;
    }

    // SJIS文字列
    throw 0;
//    return 0;
}



void CKey::Plus(tstr& t, const tstr& keyname)
{
    if (!t.empty()) {
        t += TEXT("+");
    }
    t += keyname;
}

}  // namespace