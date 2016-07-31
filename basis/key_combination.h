#pragma once
#ifndef GUID_9E8D5A55ABC144D1920C2E03C8E3A187
#define GUID_9E8D5A55ABC144D1920C2E03C8E3A187

#ifndef STDAFX_H
#include <wtypes.h>
#include <string>
#include <map>
#endif

#include "iunit_test.h"

namespace basis {

UNIT_TEST(CKey)

class CKey {
public:
	static constexpr WORD const SHIFT   = 0x30'00;
	static constexpr WORD const L_SHIFT = 0x20'00;
	static constexpr WORD const R_SHIFT = 0x10'00;
	static constexpr WORD const CTRL    = 0x0c'00;
	static constexpr WORD const L_CTRL  = 0x08'00;
	static constexpr WORD const R_CTRL  = 0x04'00;
	static constexpr WORD const ALT		= 0x03'00;
	static constexpr WORD const L_ALT   = 0x02'00;
	static constexpr WORD const R_ALT   = 0x01'00;

	//! コンストラクタ
	CKey();

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
	CKey(DWORD key);

	explicit operator bool() const { return vkey() != 0; }

	CKey operator&(const CKey& rhs) const;

	CKey& operator&=(const CKey& rhs) {
		return *this = *this & rhs;
	}

	/*! Combine two keys.
		As parameters, two different cases are permitted.
		One is Modifier key and non-Modifier key.
		Other is Modifier key and Modifier key.
		In latter case, the smallest modifier key
		(ALT<CTRL<SHIFT) would be treated as vkey.
		As so, SHIFT | ALT will equal to ALT | SHIFT.
		@par
		In the unpermitted case,
		both of keys were non-modifier key,
		then rise invalid_argument.
	*/
	CKey operator|(const CKey& rhs) const;

	CKey& operator|= (const CKey& rhs) {
		return *this = *this | rhs;
	}

	bool operator==(const CKey& rhs) const {
		return m_keys == rhs.m_keys;
	}

	bool operator!=(const CKey& rhs) const {
		return !operator==(rhs);
	}

	unsigned char vkey() const {
		return m_keys & 0xFF;
	}

	WORD flags() const {
		return m_keys & 0xFF00;
	}

	bool isModifier() const {
		return (vkey() == VK_SHIFT
			|| vkey() == VK_CONTROL
			|| vkey() == VK_MENU);
	}

	using tstr = std::basic_string<TCHAR>;
	CKey& read(tstr t) { return read(t.c_str()); }
	CKey& read(PCTSTR p);
	tstr toStr();

	static std::map<unsigned char, tstr> Text;

private:
	// 修飾キーを、VKEYとしても使う
	void descendToVKey() {
		m_keys = flags();
		if (m_keys & ALT)
			m_keys |= VK_MENU;
		else if (m_keys & CTRL)
			m_keys |= VK_CONTROL;
		else if (m_keys & SHIFT)
			m_keys |= VK_SHIFT;
	}

	void Plus(tstr& t, const tstr& keyname);
	size_t termLength(PCTSTR p, size_t nStart = 0);
	WORD m_keys;

};

}  // namespace

#endif