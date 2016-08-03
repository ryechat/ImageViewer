#pragma once
#ifndef GUID_E45C9C8A647442EA95F7EB08ABDEF7E4
#define GUID_E45C9C8A647442EA95F7EB08ABDEF7E4
#ifndef STDAFX_H
#include <string>
#endif

#include "iunit_test.h"
#include "string_buffer.h"

namespace basis {

UNIT_TEST(CPrivateProfile)

/*! Initialization file accessor.
	API wrapper for GetPrivateProfileString and others.
	@note These APIs on WIN95 has bug that rewrites parameters
	set as default string, because they didn't declaired const.
	This class provides no countermeasure for that.
*/
class CPrivateProfile {
public:
	using tstr = std::basic_string<TCHAR>;
	CPrivateProfile() = default;
	CPrivateProfile(tstr path);
	virtual ~CPrivateProfile() = default;

	CPrivateProfile(const CPrivateProfile&) = default;
	CPrivateProfile(CPrivateProfile&&) = default;
	CPrivateProfile&operator=(const CPrivateProfile&) = default;
	CPrivateProfile&operator=(CPrivateProfile&&) = default;

	//! Sets the Ini path name.
	CPrivateProfile& path(tstr path);

	//! Gets the Ini path name.
	const tstr&	path() const { return m_path; }

	//! Sets the section name.
	CPrivateProfile& section(tstr section);

	//! Gets the section name.
	const tstr& section() const { return m_section; }

	/*! Enumlates all section that is in the ini file.
		This function uses Ini path set by path() function.
		This function stores all section names into inner buffer.
		Inner buffer is accessible with using c_str().
		Stored string consists of null-separated section names,
		and last section name is terminated with two nulls.
	*/
	bool getAllSectionNames() noexcept;

	/*! Enumlates all key names in the section.
		This function uses Ini path and section name
		set by path() and section() function.
		This function stores all key names into inner buffer.
		Inner buffer is accessible with using c_str().
		Stored string consists of null-separated key names,
		and last key name is terminated with two nulls.
	*/
	bool getAllKeyNames() noexcept;

	/*! Returns a string corresponds to the key.
		This function uses Ini path and section name
		set by path() and section() function.
		If there was no corresponded string,
		sDefault would be copied as return value.
		Returned value is pointer to inner buffer,
		so reaccessible with calling c_str()
		untill other functions of this object overwrites it.
	*/
	const TCHAR *read(const TCHAR *name, const TCHAR *sDefault = nullptr);

	/*! Writes a string corresponds to the key.
		This function uses Ini path and section name
		set with path() and section() function.
	*/
	bool write(const TCHAR *keyName, const TCHAR *str);

	/*! Returns pointer to inner buffer.
		getAllSectionNames(), getAllKeyNames(), and read()
		function uses inner buffer to return strings.
		Inner buffer keeps still, until another call
		to these functions is made.
	*/
	const TCHAR *c_str() const { return m_buf.c_str(); }

	/*! Erases a specified key and a corresponded string.
		This function uses Ini path and section name
		set with path() and section() function.
	*/
	bool	eraseKey(const TCHAR *name);

	/*! Erase all keys and strings in the section.
		This function uses Ini path and section name
		set with path() and section() function.
	*/
	bool clearSection();

private:
	bool do_read(const TCHAR *section,
		const TCHAR *name, const TCHAR *sDefault);

	bool do_write(const TCHAR *name, const TCHAR *value);
	bool isInvalid(const TCHAR *name);

	tstr	m_path;			// File path
	tstr	m_section;		// Section name
	StringBuffer m_buf;			// Inner buffer
};

}  // namespace

#endif

