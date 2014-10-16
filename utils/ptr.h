/*
 * ptr.h
 *
 *  Created on: Oct 14, 2014
 *      Author: kp
 */

#ifndef UTILS_PTR_H_
#define UTILS_PTR_H_
#include <memory>
namespace utils {
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}


#endif /* UTILS_PTR_H_ */
