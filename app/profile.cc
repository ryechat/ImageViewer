#include "exception.h"
#include "stdfnc.h"
#include "ids.h"
#include "profile.h"

namespace image_viewer {

CImageViewer::Profile::Pair CImageViewer::Profile::m_profile_ids[] {
	{ ID::PROFILE_ENABLE, TEXT("bEnableSettings") },
	{ ID::LOADER_RANGE_MAX, TEXT("nPreloadRangeMax") },
    { ID::LOADER_RANGE_MIN, TEXT("nPreloadRangeMin") },
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
	{ ID::FILE_BACK, TEXT("FileBack") },
	{ ID::FILE_NEXT, TEXT("FileNext") },
	{ ID::FILE_FIRST, TEXT("FileFirst") },
	{ ID::FILE_LAST, TEXT("FileLast") },
	{ ID::FILE_RELOAD, TEXT("FileReload") },
	{ ID::VIEW_POPUP, TEXT("bHideTitleBar")},
	{ ID::VIEW_FILENAME, TEXT("ShowFilename") },
	{ ID::VIEW_FILELIST, TEXT("ShowList") },
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
	const auto path = FilePath::GetBootDirectory();
	m_prof.path((path + kSettingFile).path().c_str());
    m_enable = general().loadBoolean(ID::PROFILE_ENABLE, false);

    m_lang.path((path + kLanguageFile).path().c_str());
    m_lang.section(GENERAL);
    m_lang.section(m_lang.read(LANGUAGE, nullptr));
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
	if (save(ID::PROFILE_ENABLE, ToStr(bEnable)) == false)
		throw std::runtime_error(LOCATION);
	m_enable = bEnable;
	return true;
}



const TCHAR * CImageViewer::Profile::
ToStr(bool b)
{
	return b ? TEXT("true") : TEXT("false");
}



void CImageViewer::Profile::
applyToAllItemInTheSection(std::function<void(ID, const TCHAR *)> f)
{
    basis::CPrivateProfile prof = m_prof;
    prof.getAllKeyNames(); // Ç±ÇÍÇï€éùÇ∑ÇÈ

    basis::StringBuffer s(0, prof.c_str());
    while (!s.empty()) {
        for (auto &&i : m_profile_ids) {
            if (s == i.key)
                f(i.id, m_prof.read(i.key, nullptr));
        }
        s.refer(s.c_str() + s.getSize() + 1);
    }
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



const TCHAR * CImageViewer::Profile::
getTranslatedString(ID id)
{
    const TCHAR *key = getKeyString(id);
    return m_lang.read(key, key);
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



int CImageViewer::Profile::
load(ID id, int nDefault)
{
	return ::basis::ToInt(load(id, nullptr), nDefault);
}



bool CImageViewer::Profile::
save(ID id, int value)
{
	return m_prof.write(getKeyString(id), ::basis::ToStr(value).c_str());
}



const TCHAR *CImageViewer::Profile::
load(ID id, const TCHAR *sDefault)
{
	return m_prof.read(getKeyString(id), sDefault);
}



bool CImageViewer::Profile::
save(ID id, const TCHAR * value)
{
	return m_prof.write(getKeyString(id), value);
}


}  // namespace
