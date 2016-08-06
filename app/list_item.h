/*! @file
    Defines the element of the container which CImageViewer class uses.
    CListItem class, the element, is only created from WIN32_FIND_DATA structure.
    See also CFindFile class that returns the structure.
*/


#pragma once
#ifndef GUID_66F5CCBBF45C4752AB6FD2D76BD57BFF
#define GUID_66F5CCBBF45C4752AB6FD2D76BD57BFF

#ifndef STDAFX_H
#include <string>
#include <memory>
#endif

#include "iunit_test.h"
#include "types.h"
namespace basis { class CFilePath; }

namespace image_viewer {

UNIT_TEST(CListItem)

//! List element of CImageViewer
class CListItem {
public:
  CListItem(const WIN32_FIND_DATA&);
  ~CListItem();
  CListItem(CListItem &&) = default;
  CListItem&operator=(CListItem &&) = default;

  enum class Status : BYTE {
    Undefined = 0,
    NotExist = 1,
    CannotOpen = 2,
    SizeError = 4,
    MemoryError = 8,
    TypeError = 16,
    LoadError = 32,
    Loaded = 64,
    Release = 128,
  };

  /*!    画像をロードする。
      ファイルが開けなかった場合はStatus::CannotOpenを返す。
      それ以外の理由で失敗した場合、m_typeをTYPE::Errorに設定し、
      対応するエラーステータスを返す。
      成功した場合はStatus::Loadedを返す。
  */
  Status    loadImage(::basis::CFilePath path);


  //! Returns filename, not including directory's.
  const TCHAR * fileName() const;

  /*! Whether the file can be loaded.
      This function will return false after loading a file failed,
      except for the file was not accessible at the time.
      It is because of that the file might be loadable next.
  */
  bool    isLoadingFailed();

  void    unload();

  bool    isLoaded() const;

  //! ロード済みであれば対象のデバイスコンテキストに描画する
  bool draw(HDC hdc, const RECT& destination, const RECT& source);

  basis::Size size() const; //!< 画像サイズを返す
  basis::Rect rect() const; //!< 始点0,0の画像矩形を返す

  FILETIME ftAccess() const;
  FILETIME ftCreate() const;
  FILETIME ftWrite()  const;

  // 外部使用。リスト内インデックス
  int index;

  // 概算メモリ使用量。ロード前のみファイルサイズで、ロードするたび更新
  int weight;

  // 外部使用。ビットマップを解放するかどうかの判定に使う
  bool bUnload;

private:
  class Impl;
  std::unique_ptr<Impl> impl;

}; // class

}  // namespace

#endif