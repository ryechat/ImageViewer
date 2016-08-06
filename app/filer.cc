#include "find_file.h"
#include "list_item.h"
#include "ids.h"
#include "filer.h"

namespace image_viewer {

void CImageViewer::Filer::
generate(std::basic_string<TCHAR> dir)
{
    clear();
    basis::CFindFile e((dir += TEXT("\\*")).c_str());
    for (int i = 0; e.nextFile(); i++) {
        m_list.emplace_back(new Content(e.get()));
        m_list.back()->index = i;
    }
    m_current = begin();
}



// first != last のとき、lastは含まないことに注意
CImageViewer::iterator CImageViewer::Filer::
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



void CImageViewer::Filer::
sort()
{
    ID sort_way = m_parent.getSortWay();
    m_list.sort([sort_way, this](const Element&lhs, const Element&rhs) {
        return compare(lhs, rhs, sort_way);
    });
    giveIndices(m_list.begin(), 0);
}



bool CImageViewer::Filer::
compare(const Element &lhs, const Element &rhs, ID sortWay)
{
    FILETIME ft1, ft2;
    bool descending = false;

    switch (sortWay) {

    default:
    case ID::SORT_GREATER_WRITE:
        descending = true;    // fall through
    case ID::SORT_LESSER_WRITE:
        ft1 = lhs->ftWrite();
        ft2 = rhs->ftWrite();
        break;

    case ID::SORT_GREATER_CREATION:
        descending = true;    // fall through
    case ID::SORT_LESSER_CREATION:
        ft1 = lhs->ftCreate();
        ft2 = rhs->ftCreate();
        break;

    case ID::SORT_GREATER_ACCESS:
        descending = true;    // fall through
    case ID::SORT_LESSER_ACCESS:
        ft1 = lhs->ftAccess();
        ft2 = rhs->ftAccess();
        break;
    }

    return (::CompareFileTime(&ft1, &ft2) == (descending ? 1 : -1));
}



int CImageViewer::Filer::
indexof(const_iterator itr) const
{
    return itr == cend() ? 0 : itr->get()->index;
}



void CImageViewer::Filer::
giveIndices(iterator iStart, int index)
{
    for (; iStart != end(); ++iStart) {
        iStart->get()->index = index++;
    }
}



CImageViewer::iterator CImageViewer::Filer::
search(std::function<bool(Element&)> func)
{
    assert(func);
    iterator itr = begin();

    for (; itr != end(); ++itr)
        if (func(*itr))
            break;

    return itr;
}



CImageViewer::iterator CImageViewer::Filer::
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



}  // namespace