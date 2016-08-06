#include "key_map.h"
#include "exception.h"

namespace basis {

void CKeyMap::
clear() noexcept
{
    ZeroMemory(index, _countof(index));
    ZeroMemory(keyset, _countof(keyset));
}



bool CKeyMap::
append(CKey key, Command id) noexcept
{
    if (isFull() || !key || id == 0 || id > 0xFFFF)
        return false;

    ++index[0];

    // �C���f�b�N�X�̕ύX�Ƒ}���ʒu�擾
    Index ix;
    if (index[key.vkey()]) {
        ix = index[key.vkey()];
        // �}���ʒu�������w���Ă����C���f�b�N�X�����炷
        for (auto i = 1; i < _countof(index); ++i) {
            if (index[i] > ix)
                ++index[i];
        }
    }
    else {
        ix = index[0];
        index[key.vkey()] = ix;
    }

    // �}��
    Item item{ key, static_cast<WORD>(id) };
    for (ix; item.first; ++ix)
    {
        std::swap(item, keyset[ix]);
    }
    return true;
}



void CKeyMap::
eraseByCommand(Command id)
{
    for (unsigned i = 1; i <= size(); ) {
        if (keyset[i].second == id)
            erase(static_cast<Index>(i));
        else
            ++i;
    }
}



void CKeyMap::
eraseByKey(CKey key)
{
    WORD ix = index[key.vkey()];
    while (0 != (ix = search(key, ix))) {
        if (key == keyset[ix].first)
            erase(ix);
        else
            ++ix;
    }
    return;
}



void CKeyMap::
erase(Index ix) {
    if (ix == 0)
        throw std::invalid_argument(LOCATION);

    CKey key = keyset[ix].first;

    // �폜
    for (Index i = ix; i <= size(); ++i) {
        keyset[i] = (i <= size() - 1)
            ? keyset[i + 1]
            : Item{ 0, 0 };
    }

    // �C���f�b�N�X�ƃT�C�Y(index[0])��ύX
    for (WORD &i : index) {
        if (i > ix)
            --i;
    }

    // vkey�`�����N�����ł�����C���f�b�N�X������
    if (!search(key.vkey(), index[key.vkey()]))
        index[key.vkey()] = 0;
}



DWORD CKeyMap::
getCommand(CKey key, bool bCompliment)
{
    if (bCompliment) {
        if (GetKeyState(VK_SHIFT)   < 0) key |= CKey::SHIFT;
        if (GetKeyState(VK_CONTROL) < 0) key |= CKey::CTRL;
        if (GetKeyState(VK_MENU)    < 0) key |= CKey::ALT;
    }

    return keyset[search(key, index[key.vkey()])].second;
}



CKeyMap::Index CKeyMap::
search(CKey key, Index start_ix)
{
    CKey matched;
    WORD id = 0;

    for (auto i = start_ix; i <= size(); ++i) {
        CKey test;
        test = keyset[i].first;
        // ����vkey�̃`�����N�I��
        if (test.vkey() != key.vkey())
            return id;
        // ���S��v��������
        if (test == key) {
            return i;
        }
        // �����Ƃ���v������̂�ێ�����
        // ���Ƃ���Ctrl+F���Ȃ��Ƃ��AF�݂̂̂��̂�����΂����Ԃ�
        if (matched.flags() <= test.flags()) {
            matched = test;
            id = i;
        }
    }
    return id;
}



CKey CKeyMap::
getKey(Command id, int n)
{
    for (auto &&i : keyset) {
        if (i.second == id) {
            if (n)
                n--;
            else
                return i.first;
        }
    }
    return CKey{};
}

}  // namespace