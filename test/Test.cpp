/*
 * Test.cpp
 *
 *  Created on: Oct 11, 2014
 *      Author: kp
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>

#include "utils/logging.h"
#include <atomic>
#include <iostream>
#include "utils/memory.h"
#include <thread>

using namespace std;

BOOST_AUTO_TEST_CASE(universeInOrder) {
	BOOST_CHECK(2 + 2 == 4);
}

void DoMemoryTest(int thread_count);
void DoMemoryPassingTest(int thread_count);

BOOST_AUTO_TEST_CASE(gc_single) {
	DoMemoryTest(1);
}

BOOST_AUTO_TEST_CASE(gc_double) {
	DoMemoryTest(2);
}


BOOST_AUTO_TEST_CASE(gc_big) {
	DoMemoryTest(16);
}




BOOST_AUTO_TEST_CASE(gc_passing) {
	DoMemoryPassingTest(16);
}
