/*
 * memory_test.cpp
 *
 *  Created on: Oct 16, 2014
 *      Author: kp
 */


#include <boost/test/unit_test.hpp>


#include "utils/logging.h"
#include "utils/memory.h"
#include <vector>
#include <atomic>
#include <cassert>

class Object{
public:
	static std::atomic_int count;
	std::string msg;
	Object(const std::string& msg) : msg(msg)  {
		++count;
	}
	~Object() {
		--count;
		msg = "r";
	}
};




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
		for ( int i = 0; i < runs; i++ ) {
		//	LOG("starting     " << i);
			for (int j = 0; j < run_length; j++ ) {
				auto ptr = Allocate(std::string(""));
				if ( j % 100 == 0 ){
					pointers.push_front(ptr);
				}
			}
			int last = Object::count;

			if ( this->Cleanup() ) {
				LOG("cleaning " << last << " " << Object::count << " " << pointers.size());
				pointers.clear();
			}
		}

		LOG("finishing");
		this->Finish();
	}
};


std::atomic_int Object::count;
#define ALLOWED 18


void DoMemoryTest(int thread_count) {
		LOG("start");

		memory::MemoryManagerFactory factory(ALLOWED * 1024 * 256);
		std::vector<ObjectFunctor> functors;
		for ( int i = 0; i < thread_count; ++i ) {
			functors.emplace_back(factory.Get<Object>(), 100000, 100);
		}
		std::vector<std::thread> threads;
		for ( const auto& fun : functors ) {
			threads.emplace_back(fun);
		}
		for (  auto& thread  : threads ) {
			thread.join();
		}
}



std::mutex mutex_lock;
memory::ConditionVariable sync_queue;

std::vector<std::unique_ptr<std::mutex>> locks;
std::vector<std::list<memory::ManagedPtr<Object>>> global_pointers;

class ObjectPassingFunctor : public memory::MemoryFunctor<Object> {
	private:
		int runs;
		int run_length;
		int id;
		int target_id;
	public:
		ObjectPassingFunctor(memory::MemoryManager<Object>* memory_manager, int runs, int run_length, int id, int target_id) :
			memory::MemoryFunctor<Object>(memory_manager), runs(runs), run_length(run_length), id(id), target_id(id) {}


	virtual void operator()() {
		for ( int i = 0; i < runs; i++ ) {
			std::list<memory::ManagedPtr<Object>> pointers;
		//	LOG("starting     " << i);
			for (int j = 0; j < run_length; j++ ) {
				auto ptr = Allocate(std::string(""));
				if ( j % 100 == 0 ){
					pointers.push_front(ptr);
				}
			}
			{
				std::unique_lock<std::mutex> m(*locks[target_id]);
				global_pointers[target_id].splice(global_pointers[target_id].begin(), pointers);
				pointers.clear();
			}
			int last = Object::count;

			if ( this->Cleanup() ) {
				LOG("cleaning " << last << " " << Object::count << " " << pointers.size());
				std::unique_lock<std::mutex> m(*locks[id]);
				for ( const auto& object : global_pointers[id] ) {
					assert( object->msg == "" );
				}
				global_pointers[id].clear();
			}
		}

		LOG("finishing");
		this->Finish();
	}
	void PerformMarking() {
		std::unique_lock<std::mutex> m(*locks[id]);

			for (const auto& ptr : global_pointers[id] ) {
				Mark(ptr);
			}
		}
};



void DoMemoryPassingTest(int thread_count) {
		LOG("start");

		memory::MemoryManagerFactory factory(ALLOWED * 1024 * 256);
		std::vector<ObjectPassingFunctor> functors;
	//	locks.resize(thread_count);
		global_pointers.resize(thread_count);
		for ( int i = 0; i < thread_count; ++i ) {
			functors.emplace_back(factory.Get<Object>(), 100000, 100, i ,  (i + 1) % thread_count);
			locks.push_back(utils::make_unique<std::mutex>());
		}
		std::vector<std::thread> threads;
		for ( const auto& fun : functors ) {
			threads.emplace_back(fun);
		}
		for (  auto& thread  : threads ) {
			thread.join();
		}
}

