/* APIs
Get/WritePrivateProfileString
	Windows NT 3.1 / 95 �ȍ~
	(Winbase.h), Kernel32.lib
	Unicode�FWindows NT/2000 �� Unicode �ł� ANSI �ł�����
*/


#include "private_profile.h"

namespace basis {

CPrivateProfile::
CPrivateProfile(tstr path)
	: m_path(std::move(path))
{}



void CPrivateProfile::
path(tstr path)
{
	m_path = std::move(path);
}



void CPrivateProfile::
section(tstr sectionname)
{
	m_section = std::move(sectionname);
}



bool CPrivateProfile::
getAllKeyNames() noexcept
{
	if (m_path.empty() || m_section.empty())
		return false;

	return do_read(m_section.c_str(), nullptr, nullptr);
}



bool CPrivateProfile::
getAllSectionNames() noexcept
{
	if (m_path.empty())
		return false;
	return do_read(nullptr, nullptr, nullptr);
}



const TCHAR *CPrivateProfile::
read(const TCHAR *name, const TCHAR *sDefault)
{
	if (isInvalid(name)) {
		m_buf = sDefault;
		return m_buf.c_str();
	}
		
	if (do_read(m_section.c_str(), name, sDefault) == false) {
		m_buf = StringBuffer(sDefault);
	}
	return m_buf.c_str();
}



bool CPrivateProfile::
do_read(const TCHAR *section, const TCHAR *name, const TCHAR *def)
{
	for(DWORD capacity, nRead; ;m_buf.resize()) {
		capacity = static_cast<DWORD>(m_buf.capacity());
		nRead = GetPrivateProfileString(section, name, def,
			m_buf.data(), capacity, m_path.c_str());
		if (nRead < capacity - 2) {
			if (nRead != 0)
				return true;

			// getAll~ function�łЂƂ�������Ȃ������ꍇ�A
			// NULL �͍Ō�̂ЂƂ����t������Ȃ��̂ŁA�₤�B
			if (capacity >= 2) {
				m_buf.data()[1] = NULL;
				return true;
			}
		}
	}
}



bool CPrivateProfile::
write(const TCHAR *name, const TCHAR *value)
{
	return (!isInvalid(name) && do_write(name, value));
}



bool CPrivateProfile::
eraseKey(const TCHAR *name)
{
	return (!isInvalid(name) && do_write(name, nullptr));
}



bool CPrivateProfile::
clearSection()
{
	if (m_path.empty() || m_section.empty())
		return false;
	return do_write(nullptr, nullptr);
}



bool CPrivateProfile::
do_write(const TCHAR *name, const TCHAR *value)
{
	return (FALSE != WritePrivateProfileString(m_section.c_str(),
		name, value, m_path.c_str()));
}



// IO���Ɏg�p����p�����[�^���e�X�g���A�����Ȃ��̂������true��Ԃ�
bool CPrivateProfile::
isInvalid(const TCHAR *name)
{
	return (m_path.empty() || m_section.empty()
		|| name == nullptr || *name == NULL);
}

}  // namespace