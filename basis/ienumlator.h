#pragma once
#ifndef GUID_6B9CCE0D683A4A7BBBD72E7BFCEF70E0
#define GUID_6B9CCE0D683A4A7BBBD72E7BFCEF70E0

namespace basis {

/*!	Enumlator template class interface.
*/
template<typename T>
class IEnumlator {
public:
	virtual T& get() = 0;
	virtual bool next() = 0;
	virtual bool empty() const = 0;
	virtual ~IEnumlator() {}
};

}  // namespace

#endif