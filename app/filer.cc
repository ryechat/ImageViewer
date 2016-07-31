#include "find_file.h"
#include "list_item.h"
#include "ids.h"
#include "filer.h"
#include "menu.h"
#include "window.h"

namespace image_viewer {

void CImageViewer::CFiler::
generate(std::basic_string<TCHAR> dir)
{
	clear();
	basis::CFindFile e((dir += TEXT("\\*")).c_str());
	for (int i = 0; e.nextFile(); i++) {
		m_list.emplace_back(new CListItem(e.get()));
		m_list.back()->index = i;
	}
	m_current = begin();
}



// first != last のとき、lastは含まないことに注意
CImageViewer::iterator CImageViewer::CFiler::
erase(iterator first, iterator last)
{
	const iterator iEnd = end();
	if (first == iEnd) {
		return iEnd;
	}

	for (iterator i = first; i != iEnd; ++i) {
		if (i != m_current) {
			if (i == last)
				break;
			continue;
		}
		if (first == last && std::next(last) != iEnd)
			m_current = std::next(last);
		else if (first != last && last != iEnd)
			m_current = last;
		else if (first != begin())
			m_current = std::prev(first);
		else
			m_current = iEnd;
		break;
	}

	if (first == last)
		last = m_list.erase(first);
	else
		last = m_list.erase(first, last);

	// ひとつ前からインデックス再作成
	first = move(last, -1);
	giveIndices(first, first == begin() ? 0 : first->get()->index);
	return last;
}



void CImageViewer::CFiler::
sort()
{
	int sort_way = m_parent.menu->getSortWay();
	m_list.sort([&](const Element&lhs, const Element&rhs) {
		return compare(lhs, rhs, sort_way);
	});
	giveIndices(m_list.begin(), 0);
}



int CImageViewer::CFiler::
indexof(const_iterator itr) const
{
	return itr->get()->index;
}



void CImageViewer::CFiler::
giveIndices(iterator iStart, int index)
{
	for (; iStart != end(); ++iStart) {
		iStart->get()->index = index++;
	}
}



CImageViewer::iterator CImageViewer::CFiler::
search(std::function<bool(Element&)> func)
{
	assert(func);
	iterator itr;

	for (itr = begin(); itr != end(); ++itr) {
		if (func(*itr))
			break;
	}
	return itr;
}



CImageViewer::iterator CImageViewer::CFiler::
move(iterator itr, int nCount)
{
	iterator iEnd = end();
	if (isEmpty()) return iEnd;

	// search prev
	for (iterator iFirst = begin(); (nCount < 0 && itr != iFirst); ++nCount) {
		--itr;
	}

	// search next
	for (--iEnd; nCount > 0 && itr != iEnd; --nCount) {
		++itr;
	}

	return itr;
}



bool CImageViewer::CFiler::
compare(const Element &lhs, const Element &rhs, int sortWay)
{
	FILETIME ft1, ft2;
	bool bGreaterSort = false;

	using ID = ID;
	switch (static_cast<ID>(sortWay)) {

	default:
	case ID::SORT_GREATER_WRITE:
		bGreaterSort = true;	// fall through
	case ID::SORT_LESSER_WRITE:
		ft1 = lhs->ftWrite();
		ft2 = rhs->ftWrite();
		break;

	case ID::SORT_GREATER_CREATION:
		bGreaterSort = true;	// fall through
	case ID::SORT_LESSER_CREATION:
		ft1 = lhs->ftCreate();
		ft2 = rhs->ftCreate();
		break;

	case ID::SORT_GREATER_ACCESS:
		bGreaterSort = true;	// fall through
	case ID::SORT_LESSER_ACCESS:
		ft1 = lhs->ftAccess();
		ft2 = rhs->ftAccess();
		break;
	}

	return (::CompareFileTime(&ft1, &ft2) == (bGreaterSort ? 1 : -1));
}

}  // namespace