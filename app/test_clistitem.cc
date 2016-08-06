/*! @file
    Defines a unit test function for CListItem class objects.
    This function will open and load the file, which is named "img11.jpg".
    If the file was not found, this function would quit the test immediatry
    without reporting any error.
*/
#include "find_file.h"
#include "file_path.h"
#include "surface.h"
#include "stdfnc.h"
#include "list_item.h"

namespace {
  bool equal(const FILETIME &l, const FILETIME &r) {
    return l.dwHighDateTime == r.dwHighDateTime && l.dwLowDateTime == r.dwLowDateTime;
  }
}

namespace image_viewer {

UNIT_TEST_FUNC(CListItem) {

  basis::CFilePath dir = dir.GetBootDirectory().path().c_str();

  basis::StringBuffer name{ 0, TEXT("img11.jpg") };

  if (!(dir + name.c_str()).exist())
    return true; // File not found.
  
  // CListItem needs for a WIN32_FIND_DATA to be created.
  basis::CFindFile e((dir + TEXT("*")).path().c_str());
  WIN32_FIND_DATA fd;
  
  while (e.nextFile()) {
    if (name == e.get().cFileName) {
      fd = e.get();
        break;
    }
  }
  if (e.empty())
    throw 0; // If this rose, CFindFile class should have a bug.
  
  DWORD size = fd.nFileSizeLow;
  if (size == 0 || fd.nFileSizeLow == INVALID_FILE_SIZE) {
    throw 0;
  }
  
  CListItem item(fd);

  if (name != item.fileName())
    throw 0;

  if (!equal(item.ftAccess(), fd.ftLastAccessTime) ||
    !equal(item.ftCreate(), fd.ftCreationTime) ||
    !equal(item.ftWrite(), fd.ftLastWriteTime))
    throw 0;

  int exception = 0;
  try { item.size(); }
  catch (std::runtime_error &) { exception += 1; }

  try { item.rect(); }
  catch (std::runtime_error &) { exception += 2; }
  if (exception != 3)
    throw 0;
  
  if (item.loadImage((dir + name.c_str()).path().c_str()) != CListItem::Status::Loaded)
    throw 0;
  
  if (!item.isLoaded())
    throw 0;

  item.size(); // Should not throw
  item.rect(); // Should not throw

  basis::Surface s;
  s.create(0, item.size());
  if (!item.draw(s, item.rect(), item.rect()))
    throw 0;
  
  return true;
}

}  // namespace