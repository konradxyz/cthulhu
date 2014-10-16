/*
 * Test.cpp
 *
 *  Created on: Oct 11, 2014
 *      Author: kp
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>

#include <atomic>
#include <iostream>
#include "utils/memory.h"
#include <thread>

using namespace std;

BOOST_AUTO_TEST_CASE(universeInOrder) {
	BOOST_CHECK(2 + 2 == 4);
}


class Object{
public:
	static std::atomic_int count;
	std::string msg;
	Object(const std::string& msg) : msg(msg)  {
		++count;
	}
	~Object() {
		--count;
	}
};



#define THREAD_COUNT 3000

std::mutex mutex_lock;
vector<int> sizes;
memory::ConditionVariable sync_queue;

class ObjectFunctor : public memory::MemoryFunctor<Object> {
private:
	std::list<memory::ManagedPtr<Object>> pointers;
	int runs;
	int run_length;
public:
	ObjectFunctor(memory::MemoryManager<Object>* memory_manager, int runs, int run_length) :
		memory::MemoryFunctor<Object>(memory_manager), runs(runs), run_length(run_length) {}

	void PerformMarking() {
		for (const auto& ptr : pointers ) {
			Mark(ptr);
		}
	}

	virtual void operator()() {
		bool gc_performed = false;
		cerr << "running" << endl;
		for ( int i = 0; i < runs; i++ ) {
			cerr << i << endl;
			for (int j = 0; j < run_length; j++ ) {
				auto ptr = Allocate(std::string(""));
				if ( j % 2 == 0 ){
					pointers.push_front(ptr);
				}
			}
			int last = Object::count;
			this->Cleanup();
				std::unique_lock<std::mutex> m(mutex_lock);
				sizes.push_back(this->pointers.size());
				if ( sizes.size() == THREAD_COUNT ) {
					int amount = std::accumulate(sizes.begin(), sizes.end(), 0);
					cerr << "l" << last;
					cerr << "o" << Object::count << endl;
					cerr << "p " << amount << endl;
				//	assert(amount == Object::count);
					sync_queue.NotifyAll();
					sizes.clear();
				} else
					sync_queue.Wait(&m);



		}
	//	assert(gc_performed);

	//	assert(pointers.size() == Object::count);

		this->Finish();
	}
};


std::atomic_int Object::count;
#define ALLOWED 1

BOOST_AUTO_TEST_CASE(gc) {
	cerr << "start" << endl;
	memory::MemoryManagerFactory factory(ALLOWED * 1024 );
	std::vector<ObjectFunctor> functors;
	for ( int i = 0; i < THREAD_COUNT; ++i ) {
		functors.emplace_back(factory.Get<Object>(), 10000000, 100);
	}
	std::vector<std::thread> threads;
	for ( const auto& fun : functors ) {
		threads.emplace_back(fun);
	}
	for (  auto& thread  : threads ) {
		thread.join();
	}
}
