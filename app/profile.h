#pragma once
#ifndef GUID_88E6B0C9D24C48378AB9ED3883518A73
#define GUID_88E6B0C9D24C48378AB9ED3883518A73

#include "private_profile.h"
#include "image_viewer.h"

namespace image_viewer {
	enum class ID : int;

class CImageViewer::Profile {
public:
	using tstr = std::basic_string<TCHAR>;

	Profile();
	~Profile() = default;

	Profile(Profile&) = delete;

	void setPath(tstr path)	{ m_prof.path(std::move(path)); }
	bool exist();
	bool isEnable() { return m_enable; }
	bool enable() { return setEnable(true); }
	bool disable() { return setEnable(false); }

	Profile& general() {
		m_prof.section(SECTION_GENERAL);
		return *this;
	}

	Profile& menu() {
		m_prof.section(SECTION_MENU);
		return *this;
	}

	Profile& window() {
		m_prof.section(SECTION_WINDOW);
		return *this;
	}

	Profile& control() {
		m_prof.section(SECTION_CONTROL);
		return *this;
	}

	const TCHAR * getKeyString(ID id);

	int get(ID id, int nDefault);
	bool write(ID id, int value);
	void access(ID id, LONG *value, bool bWrite);

	const TCHAR *get(ID id, const TCHAR *sDefault);
	bool write(ID id, const TCHAR * value);
	const TCHAR *access(ID id, const TCHAR *value, bool bWrite);

	bool loadBoolean(ID id, bool bDefault);
	bool saveBoolean(ID id, bool b);
	void booleanReadWrite(ID id, bool *b, bool bWrite);

	// ñÛÇ™Ç†ÇÍÇŒñ|ñÛÇ∑ÇÈÅBÇ»ÇØÇÍÇŒÉLÅ[ñºÇï‘Ç∑
	tstr getTranslatedString(ID id);

private:
	bool setEnable(bool bEnable);
	const TCHAR * ToStr(bool b);

	struct Pair { ID id; const TCHAR *key; };
	static Pair m_profile_ids[];
	static constexpr TCHAR * const SECTION_WINDOW = TEXT("Window");
	static constexpr TCHAR * const SECTION_CONTROL = TEXT("Control");
	static constexpr TCHAR * const SECTION_MENU = TEXT("Menu");
	static constexpr TCHAR * const SECTION_GENERAL = TEXT("General");
	static constexpr TCHAR * const kSettingFile = TEXT("setting.ini");
	static constexpr TCHAR * const kLanguageFile = TEXT("language.ini");
	static constexpr TCHAR * const kSectionLanguage = TEXT("Language");

	bool m_enable;
	basis::CPrivateProfile m_prof, m_lang;
};

}  // namespace

#endif
