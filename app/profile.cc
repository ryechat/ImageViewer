#include "exception.h"
#include "stdfnc.h"
#include "string_buffer.h"
#include "ids.h"
#include "profile.h"

namespace image_viewer {

CImageViewer::Profile::Pair CImageViewer::Profile::m_profile_ids[] {
	{ ID::PROFILE_ENABLE, TEXT("bEnableSettings") },
	{ ID::LOADER_RANGE, TEXT("nPreloadRange") },
	{ ID::LOADER_MEMORY_CAP, TEXT("nMemoryCapMegaBytes") },

	{ ID::WINDOW_REMINDER, TEXT("bEnableWindowReminder") },
	{ ID::WINDOW_POSITION, TEXT("bRemindWindowPosition") },
	{ ID::WINDOW_ZOOMING, TEXT("bRemindWindowZooming") },
	{ ID::WINDOW_STYLE, TEXT("bRemindWindowStyle") },
	{ ID::WINDOW_LEFT, TEXT("nWindowLeft") },
	{ ID::WINDOW_TOP, TEXT("nWindowTop") },
	{ ID::WINDOW_RIGHT, TEXT("nWindowRight") },
	{ ID::WINDOW_BOTTOM, TEXT("nWindowBottom") },
	{ ID::WINDOW_MAXIMIZE, TEXT("bWindowMaximize") },

	{ ID::GRIP_IMAGE, TEXT("GripImage") },
	{ ID::LIST_REMOVE, TEXT("RemoveFromList") },
	{ ID::CONFIRM_DELETE, TEXT("ConfirmDelete") },

	{ ID::LAST_PATH, TEXT("sLastOpenedFile") },
	{ ID::USE_PROFILE, TEXT("Use Settings in INI") },
	{ ID::SCREEN_TOGGLE, TEXT("ToggleScreen") },
	{ ID::LIST_EMPTY, TEXT("ListEmpty") },
	{ ID::FILE_DELETE, TEXT("FileDelete") },
	{ ID::FILE_QUICK_DELETE, TEXT("QuickFileDelete") },
	{ ID::FILE_PREVIOUS, TEXT("FileBack") },
	{ ID::FILE_NEXT, TEXT("FileNext") },
	{ ID::FILE_FIRST, TEXT("FileFirst") },
	{ ID::FILE_LAST, TEXT("FileLast") },
	{ ID::FILE_RELOAD, TEXT("FileReload") },
	{ ID::VIEW_POPUP, TEXT("bHideTitleBar")},
	{ ID::VIEW_FILENAME, TEXT("ShowFilename") },
	{ ID::VIEW_LIST, TEXT("ShowList") },
	{ ID::VIEW_CENTER, TEXT("Centering") },
	{ ID::VIEW_UPSCALE, TEXT("UpscaleImage") },
	{ ID::VIEW_DOWNSCALE, TEXT("DownscaleImage") },
	{ ID::VIEW_END, TEXT("View") },
	{ ID::SORT_GREATER_WRITE, TEXT("DescendingWriteTime") },
	{ ID::SORT_LESSER_WRITE, TEXT("AscendingWriteTime") },
	{ ID::SORT_GREATER_CREATION, TEXT("DescendingCreationTime") },
	{ ID::SORT_LESSER_CREATION, TEXT("AscendingCreationTime") },
	{ ID::SORT_GREATER_ACCESS, TEXT("DescendingAccessTime") },
	{ ID::SORT_LESSER_ACCESS, TEXT("AscendingAccessTime") },
	{ ID::SORT_END, TEXT("SortBy") },
	{ ID::SHOW_PROPERTY, TEXT("ShowProperty") },
	{ ID::WINDOW_CLOSE, TEXT("CloseWindow") },
};



CImageViewer::Profile::
Profile() : m_enable(true)
{
	auto path = basis::CFilePath::GetBootDirectory();
	m_prof.path((path + kSettingFile).path().c_str());
	m_enable = basis::ToInt(general().get(ID::PROFILE_ENABLE, nullptr), false);

	// Set section(language) name specified at Language in General section.
	m_lang.path((path + kLanguageFile).path().c_str());
	m_lang.section(SECTION_GENERAL);
	m_lang.read(kSectionLanguage, nullptr);
	m_lang.section(m_lang.c_str());
}



tstr CImageViewer::Profile::
getTranslatedString(ID id)
{
	tstr key = getKeyString(id);
	return m_lang.read(key.c_str(), key.c_str());
}



bool CImageViewer::Profile::
exist()
{
	return GetFileAttributes(m_prof.path().c_str()) != static_cast<DWORD>(-1);
}



bool CImageViewer::Profile::
setEnable(bool bEnable)
{
	general();
	if (!write(ID::PROFILE_ENABLE, ToStr(bEnable)))
		throw std::runtime_error(LOCATION);
	m_enable = bEnable;
	return true;
}



const TCHAR * CImageViewer::Profile::
ToStr(bool b)
{
	return b ? TEXT("true") : TEXT("false");
}



const TCHAR * CImageViewer::Profile::
getKeyString(ID id)
{
	for (auto i : m_profile_ids) {
		if (i.id == id)
			return i.key;
	}
	return TEXT("");
}



int CImageViewer::Profile::
get(ID id, int nDefault)
{
	return ::basis::ToInt(get(id, nullptr), nDefault);
}



const TCHAR *CImageViewer::Profile::
get(ID id, const TCHAR *sDefault)
{
	return m_prof.read(getKeyString(id), sDefault);
}



bool CImageViewer::Profile::
write(ID id, int value)
{
	return m_prof.write(getKeyString(id), ::basis::ToStr(value).c_str());
}



bool CImageViewer::Profile::
write(ID id, const TCHAR * value)
{
	return m_prof.write(getKeyString(id), value);
}



bool CImageViewer::Profile::
loadBoolean(ID id, bool b)
{
	basis::StringBuffer str(0, m_prof.read(getKeyString(id), nullptr));
	if (str == TEXT("true"))
		return true;
	if (str == TEXT("false"))
		return false;
	return 0 != basis::ToInt(m_prof.c_str(), b ? 1 : 0);
}



bool CImageViewer::Profile::
saveBoolean(ID id, bool b)
{
	return m_prof.write(getKeyString(id), ToStr(b));
}



void CImageViewer::Profile::
access(ID id, LONG *value, bool bWrite)
{
	if (bWrite) {
		if (!write(id, static_cast<int>(*value)))
			throw 0;
		return;
	}
	*value = static_cast<LONG>(get(id, static_cast<int>(*value)));
}



const TCHAR * CImageViewer::Profile::
access(ID id, const TCHAR * value, bool bWrite)
{
	if (bWrite) {
		if (!write(id, value))
			throw 0;
		return value;
	}
	return get(id, value);
}



void CImageViewer::Profile::
booleanReadWrite(ID id, bool *value, bool bWrite)
{
	if (bWrite)
		saveBoolean(id, *value);
	else
		*value = loadBoolean(id, *value);
}



}  // namespace
