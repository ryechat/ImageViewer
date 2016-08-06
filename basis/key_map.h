#pragma once
#ifndef GUID_47EFB4BE87AD49519D4171C3CC5BAB09
#define GUID_47EFB4BE87AD49519D4171C3CC5BAB09

#include "key_combination.h"
#include "iunit_test.h"

namespace basis {

UNIT_TEST(CKeyMap)

//! Accelerator key container
class CKeyMap {
public:
    using tstr = std::basic_string<TCHAR>;

    using Index = WORD;

    //! Command Identifier; Hi-WORD would be ignored.
    using Command = DWORD;

    CKeyMap() { clear(); }

    void clear() noexcept;

    //! Returns a maximum number of key-id entry.
    size_t capacity() noexcept { return _countof(keyset) - 1; }

    //! Returns a stored number of key-id entry.
    size_t size() noexcept { return index[0]; }

    bool isFull() noexcept { return (size() >= capacity()); }

    /*! Appends a pair of key and command ID.
        @param key
        @param id    Command identifier (0, 0xFFFF)
    */
    bool append(CKey key, Command id) noexcept;

    //! Erases an entry that has specified key.
    void eraseByKey(CKey key);

    //! Erases all entry that has spedicified command ID.
    void eraseByCommand(Command id);

    //! Erases an entry specified by the index.
    void erase(Index ix);

    /*! Returns a command ID related to the key.
        If bCompliment set to true, combination keys,
        Ctrl, Alt, Shift, its states are automatically tested.
    */
    DWORD getCommand(CKey key, bool bCompliment = true);

    //! Returns n-th candidate related to the command ID.
    CKey getKey(Command id, int n);

private:
    /*! Returns index of an entry related to the key.
        If identical entry is not exist, It returns one that have
        the same vkey.
        If identical or vkey-identical entry is not exist,
        then it returns null key.
    */
    Index search(CKey key, Index start_ix);

    //! Container
    using Item = std::pair<CKey, WORD>;
    Item keyset[512];

    //! index[vkey] will have an index of keyset, at where vkey first appears.
    //! index[0] have the number of stored entry.
    WORD index[256];
};

}  // namespace

#endif