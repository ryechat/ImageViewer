/*!
    @file 補助関数群。
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

// スコープアウト時にポインタをどうにかするポインタキャリア
// 解体時の動作をラムダ式で受け取る以外はstd::unique_ptrと同じ
template<class T>
class SafeRelease {
    using deleter = std::function<void(T*)>;
    deleter m_f;
    T *m_p;
public:
    // ポインタとラムダ式を与える
    SafeRelease(T* p, deleter f)
        : m_p(p), m_f(f) {}

    SafeRelease() : m_p(0) {}
    ~SafeRelease() { if(m_p && m_f) m_f(m_p); }

    SafeRelease(SafeRelease&) = delete;
    SafeRelease&operator=(SafeRelease&) = delete;
    SafeRelease(SafeRelease&&) = default;
    SafeRelease&operator=(SafeRelease&&) = default;

    // ポインタとして振る舞う
    explicit operator bool() { return m_p != nullptr; }
    operator T*() { return m_p; }
    T* get() { return m_p; }
    T& operator*() { return *m_p; }
    T* operator->() { return m_p; }
    // ミューテーター
    T* operator=(const T* p) { m_p = p; }
    void operator=(deleter f) { m_f = f; }
};



//! バイトオーダーを逆にする(4Bytes)
uint32_t ReverseByteOrder(uint32_t n);

//! 実バイト数に応じて整数型の書式文字列を構成する
//! d_x は%d とか %x のdとかxの文字を指定
PTSTR GetTypeFormat(TCHAR type[5], size_t bytes, TCHAR d_x);

//! 整数値を文字列型に書き出し
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



//! コマンドラインのnCmd番目の引数を返す。
/*! 0番目は実行中のプログラムのパス、以降が引数である。
    引数が""(ダブルクォート)で囲まれている場合はこれを除去する。
    "path" や -ls など混在した引数でも動作する。
    戻り値はbasic_string<TCHAR>型で、対象の引数がない場合は空(empty)である。
*/
tstr GetCommandLine(int nCmd);



/*! WM_DROPFILESメッセージが飛んできたときに
    num番目のファイル名を取得する
*/
tstr GetDropFile( WPARAM wp, UINT num = 0 );

//! 指定されたフォントのうち最大の高さをかえす。
/*!    第3引数が0の場合は第2引数の高さを、
    第2引数も0の場合はカレントフォントの高さをかえす。
*/
LONG GetFontHeight( HDC hdc, HFONT lhs, HFONT rhs );

//! ファイルのプロパティを表示する
bool ShowProperty(HWND hParentWnd, const tstr &pszFile);

//! OLE用ヘルパー関数。OLE_HIMETRIC単位(mm単位)への変換
void HimetricFromDP(HDC hdc, SIZE& s);

//! 矩形 screen から foreground を除いた領域をRectangle()を使って塗りつぶす。
bool ClearBackground(HDC hdc, RECT screen, RECT foreground);

}  // namespace

#endif