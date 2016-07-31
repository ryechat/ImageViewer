#pragma once
#ifndef GUID_5718854C303D43FF961DFBE2E38A4EA6
#define GUID_5718854C303D43FF961DFBE2E38A4EA6

#ifndef STDAFX_H
#include <string>
#include <memory>
#include <list>
#endif

#include "image_viewer.h"

namespace image_viewer {

//! リスト管理を行う内部クラス
class CImageViewer::CFiler {
public:
	CFiler(CImageViewer &parent_) : m_parent(parent_), m_current(end()) {}
	~CFiler() = default;

	//! ディレクトリ内のファイルをリスト化する。
	/*! @par	関数実行前に保持していたリストは解放され、
				CFileEnumNoDirectories によって列挙されるファイルが格納される。
		@par	先頭ファイルがカレントファイルとなる。
		@param dir	対象となるディレクトリ
	*/
	void generate(std::basic_string<TCHAR> dir);

	/*!	アイテムをリストから削除する。
		@details	指定されたアイテムがカレントファイルの場合、
					カレントイテレータは次のファイルに移動される。
					次のファイルがない場合は前、
					それもなければend要素に移動される。
		@param itr	対象となるアイテム
		@return		カレントイテレータ
	*/
	iterator erase(iterator itr) { return erase(itr, itr); }

	//! [first, last) の範囲の要素を削除する
	iterator erase(iterator first, iterator last);

	/*! リストのすべての要素を削除する。
		@details カレントイテレータはend()に移動される。
	*/
	void clear() {
		m_list.clear();
		m_current = end();
	}

	/*! リストをソートする。
		@details メニューで指定されたFILETIME比較でソートされる。
	*/
	void sort();

	//! カレントイテレータを取得。空の場合はend()
	iterator current() const { return m_current; }

	//! カレントイテレータを設定
	void setCurrent(iterator itr) { m_current = itr; }

	const_iterator cbegin() const { return m_list.cbegin(); }
	const_iterator cend() const { return m_list.cend(); }
	const_iterator clast() const { return isEmpty() ? cend() : --cend(); }

	iterator begin() { return m_list.begin(); }
	iterator end() { return m_list.end(); }
	iterator last() { return isEmpty() ? end() : --end(); }

	bool isEmpty() const { return m_list.empty(); }
	size_t size() const { return m_list.size(); }


	// リスト中のインデックスを返す
	int indexof(const_iterator itr) const;

	// 指定位置以降のアイテムに、リスト中のインデックスを与える
	void giveIndices(iterator iStart, int index);

	iterator search(std::function<bool(Element&)> func);

	// 有効なイテレータのみを対象に、nCountぶん移動する
	iterator move(iterator itr, int nCount);

private:
	bool compare(const Element&, const Element&, int sortWay);

	CImageViewer &m_parent;
	ListTy m_list;
	iterator m_current;
};

}  // namespace

#endif
