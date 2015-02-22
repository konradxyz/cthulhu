
#ifndef UTILS_ALLOCATOR_H_
#define UTILS_ALLOCATOR_H_

#include <vector>
#include <memory>
#include "utils/ptr.h"

namespace utils {
template
<typename T>

class Allocator {
private:
	std::vector<std::unique_ptr<T>> allocated;
public:
	template
	<typename U,  typename... Args>
	U* alloc(Args&&... args) {
		auto result = utils::make_unique<U>(std::forward<Args>(args)...);
		auto to_return = result.get();
		allocated.push_back(std::move(result));
		return to_return;
	}

};

}

#endif

