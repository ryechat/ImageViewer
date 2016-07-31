#include "singleton.h"

namespace basis {

std::vector<SingletonFinalizers::Finalizer>
SingletonFinalizers::maFinalizer;



void SingletonFinalizers::Finalize()
{
	while (!maFinalizer.empty()) {
		maFinalizer.back()();
		maFinalizer.pop_back();
	}
}

}  // namespace