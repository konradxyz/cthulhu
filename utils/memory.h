/*
 * memory.h
 *
 *  Created on: Oct 15, 2014
 *      Author: kp
 */

#ifndef UTILS_MEMORY_H_
#define UTILS_MEMORY_H_

#include "ptr.h"
#include <utility>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include "logging.h"

namespace memory {


template
<typename T>
class MemoryPiece {
  private:
    char mark;
    T object;
  public:
    template<typename... Args>
    MemoryPiece(Args&&... args) : mark(0), object(std::forward<Args>(args)...) {}
    T* GetMutableObject() { return &object; }
    //Returns true if this object has already been marked, false otherwise
    bool Mark(char new_mark) {
      if ( mark != new_mark ) {
        mark = new_mark;
        return false;
      }
      return true;
    }
    int Mark() { return mark; }
};

template
<typename T>
class ManagedPtr {
  private:
    MemoryPiece<T>* ptr;
  public:
    T*  operator->() {
      return ptr->GetMutableObject();
    }
    const T*  operator->() const {
          return ptr->GetMutableObject();
    }
    T& operator*() {
      return *(ptr->GetMutableObject());
    }
    ManagedPtr(MemoryPiece<T>*  ptr) : ptr(ptr) {}
    bool Mark(char new_mark) {  return ptr->Mark(new_mark); }
    char Mark() { return ptr->Mark(); }
};

class ConditionVariable {
private:
	std::condition_variable condition;
	int count = 0;
	enum State {
		WORKING = 1,
		WAKING_SINGLE = 2,
		WAKING_ALL = 3
	};
	State state = WORKING;
public:
	void Wait(std::unique_lock<std::mutex>* lock) {
		++count;
		while (state == WORKING) {
			condition.wait(*lock);
		}
		--count;
		if ( state == WAKING_SINGLE ) {
			state = WORKING;
		} else {
			if ( count > 0 )
				condition.notify_one();
			else {
				state = WORKING;
			}
		}
	}

	void Notify() {
		if ( count == 0 )
			return;
		state = (state == WAKING_ALL) ? WAKING_ALL : WAKING_SINGLE; condition.notify_one(); }
	void NotifyAll() {
		if ( count == 0 )
			return;
		state = WAKING_ALL; condition.notify_one(); }

	int Count() { return count; }
};


class MemoryContext {
  private:
    enum State {
      WORKING = 1,
      MARKING = 2,
      SWEEPING = 3,
	  FINALIZING = 4
    };

    std::mutex lock;

    unsigned allowed;
    unsigned allocated;
    int managers_count;
    char last_mark = 1;
    MemoryContext::State state = WORKING;


    ConditionVariable waiting_for_working;
    ConditionVariable waiting_for_marking_done;
    ConditionVariable waiting_for_release;
    ConditionVariable waiting_for_sweeping_done;
    std::vector<unsigned> allocated_per_manager;
  public:
    MemoryContext(unsigned amount_allowed) : allowed(amount_allowed), allocated(0), managers_count(0) {}

    int Register() {
    	allocated_per_manager.push_back(0);
    	return managers_count++;
    }

    char UpdatedAllocated(unsigned amount, int manager_id) {
      std::unique_lock<std::mutex> m(lock);
      allocated = allocated + amount - allocated_per_manager[manager_id];
      allocated_per_manager[manager_id] = amount;
      if ( state == MARKING ) {
    	  return last_mark;
      }
      while	 ( state != WORKING ) {
        waiting_for_working.Wait(&m);
      }
      if ( allocated >= allowed ) {
        state = MARKING;
        last_mark += 2; // we increase mark by 2 to make sure that we never reach 0.
      }
      return state == MARKING ?  last_mark : 0;
    }

    void MarkDone() {
      std::unique_lock<std::mutex> m(lock);
      assert(state == MARKING);
      if ( waiting_for_marking_done.Count() + waiting_for_release.Count() + 1 < managers_count ) {
    	  waiting_for_marking_done.Wait(&m);
      } else {
    	  state = SWEEPING;
    	  waiting_for_marking_done.NotifyAll();
    	  waiting_for_release.NotifyAll();
      }
    }

    void SweepDone(unsigned amount, int manager_id) {
      std::unique_lock<std::mutex> m(lock);
      allocated = allocated + amount - allocated_per_manager[manager_id];
      allocated_per_manager[manager_id] = amount;

      if ( waiting_for_sweeping_done.Count() + 1 < managers_count ) {
    	  waiting_for_sweeping_done.Wait(&m);
      } else {
    	  state = WORKING;
    	  waiting_for_sweeping_done.NotifyAll();
      }
    }

    char Release() {
        std::unique_lock<std::mutex> m(lock);
        if ( waiting_for_release.Count() + 1 == managers_count) {
        	waiting_for_release.NotifyAll();
        	state = FINALIZING;
        	return 0;
        }

        assert(state != SWEEPING);
        //LOG("first " << state);
		switch (state) {
		case FINALIZING:
			return 0;
		case MARKING:
			return last_mark;
		default:
			waiting_for_release.Wait(&m);
		}
		//LOG("second " << state);
		switch ( state ) {
		case FINALIZING:
			return 0;
		case MARKING:
			return last_mark;
		default: {
			LOG(state);
			assert(false);
			return 0;
		}
		}
    }

};

class MemoryManagerBase {
public:
	virtual ~MemoryManagerBase() {}
};

template
<typename T>
class MemoryManager : public MemoryManagerBase {
  private:
	std::list<std::unique_ptr<MemoryPiece<T> > > allocated;
    MemoryContext* context;
    int last_amount = 0;
    int manager_id = 0;
  public:
    template<typename... Args>
    ManagedPtr<T> Allocate(Args&&... args) {
      allocated.push_front(utils::make_unique<MemoryPiece<T>>(
            std::forward<Args>(args)...));

      return ManagedPtr<T>(allocated.front().get());
    }

    MemoryManager(MemoryContext* ctx) : context(ctx) {
    	context->Register();
    }
    // returns non zero value in case when GC is needed
    // user is expected to start marking with this value immediately
    // and then call Sweep
    char Status() {
      return context->UpdatedAllocated(allocated.size(), manager_id);
    }

    void Sweep(char mark) {
      context->MarkDone();
     // int pre = allocated.size();
      allocated.remove_if([mark](std::unique_ptr<MemoryPiece<T>>& ptr) { return ptr->Mark() != mark; });
      context->SweepDone(allocated.size(), manager_id);
    }

    void Release() {
     	char status = context->Release();
    	while ( status > 0) {
    		Sweep(status);
    		status = context->Release();
    	}
    }
};

#define MAX_PERIOD 200

template
<typename T>
class MemoryFunctor {
private:
	MemoryManager<T>* memory_manager;
	char mark = 0;
	int period = 0;
public:
	virtual void operator()() = 0;
	virtual void PerformMarking() = 0;
	virtual ~MemoryFunctor() {}
	MemoryFunctor(MemoryManager<T>* memory_manager) : memory_manager(memory_manager) {}
	template<typename... Args>
	ManagedPtr<T> Allocate(Args&&... args) {
		return memory_manager->Allocate(
				std::forward<Args>(args)...);
	}

	// Returns true if this thing has already been marked
	bool Mark(ManagedPtr<T> object) {
		return object.Mark(mark);
	}

	// Returns true if GC was run.
	bool Cleanup() {
		++period;
		if ( period < MAX_PERIOD )
			return false;
		period = 0;
		char tmp = memory_manager->Status();
		if ( tmp == 0 )
			return false;
		LOG("cleaning");
		mark = tmp;
		this->PerformMarking();
		LOG("sweeping");
		memory_manager->Sweep(mark);
		return true;
	}

	void Finish() {
		memory_manager->Release();
	}
};

//Generates MemoryManagers
//After first manager calls Status method no further managers should be created
class MemoryManagerFactory {
  private:
    MemoryContext context;
    std::list<std::unique_ptr<MemoryManagerBase > > managers;
  public:
    template<typename T>
    MemoryManager<T>* Get() {
    	auto result = utils::make_unique<MemoryManager<T> >(
                &context);
    	auto result_ptr = result.get();
    	result.release();
    	std::unique_ptr<MemoryManagerBase> base_ptr(result_ptr);
      managers.push_front(std::move(base_ptr));
      return result_ptr;
    }

    MemoryManagerFactory(unsigned amount_allowed) : context(amount_allowed) {}
};

}




#endif /* UTILS_MEMORY_H_ */
