/*
 * logging.h
 *
 *  Created on: Oct 16, 2014
 *      Author: kp
 */

#ifndef UTILS_LOGGING_H_
#define UTILS_LOGGING_H_

#include <boost/log/trivial.hpp>
#include <iostream>
#include <thread>

#define LOG(x) {std::cerr << "Thread " << std::this_thread::get_id() << ": " <<  x << std::endl;}



#endif /* UTILS_LOGGING_H_ */
