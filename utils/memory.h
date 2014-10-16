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

namespace memory {


template
<typename T>
class MemoryPiece {
  private:
    int mark;
    T object;
  public:
    template<typename... Args>
    MemoryPiece(Args&&... args) : mark(0), object(std::forward<Args>(args)...) {}
    T* GetMutableObject() { return &object; }
    //Returns true if this object has already been marked, false otherwise
    bool Mark(int new_mark) {
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
    T& operator*() {
      return *(ptr->GetMutableObject());
    }
    ManagedPtr(MemoryPiece<T>*  ptr) : ptr(ptr) {}
    bool Mark(int new_mark) {  return ptr->Mark(new_mark); }
    int Mark() { return ptr->Mark(); }
};

class ConditionVariable {
private:
	std::condition_variable condition;
	int count = 0;
	bool waking = false;
public:
	void Wait(std::unique_lock<std::mutex>* lock) {
		++count;
		while (!waking) {
			condition.wait(*lock);
		}
		--count;
	}

	void Notify() { condition.notify_one(); }
	void NotifyAll() { condition.notify_all(); }

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

    int allowed;
    unsigned allocated;
    int managers_count;
    int last_mark = 1;
    MemoryContext::State state = WORKING;


    ConditionVariable waiting_for_working;
    ConditionVariable waiting_for_marking_done;
    ConditionVariable waiting_for_release;
    ConditionVariable waiting_for_sweeping_done;
    std::vector<unsigned> allocated_per_manager;
  public:
    MemoryContext(int amount_allowed) : allowed(amount_allowed), allocated(0), managers_count(0) {}

    int Register() {
    	allocated_per_manager.push_back(0);
    	return managers_count++;
    }

    int UpdatedAllocated(unsigned amount, int manager_id) {
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
        ++last_mark;
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

    int Release() {
        std::unique_lock<std::mutex> m(lock);
        if ( waiting_for_release.Count() == managers_count) {
        	waiting_for_release.NotifyAll();
        	state = FINALIZING;
        	return 0;
        }

        assert(state != SWEEPING);
		switch (state) {
		case FINALIZING:
			return 0;
		case MARKING:
			return last_mark;
		default:
			waiting_for_release.Wait(&m);
		}
		switch ( state ) {
		case FINALIZING:
			return 0;
		case MARKING:
			return last_mark;
		default: {
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
    int Status() {
      //LOG(allocated.size());
      int mark = context->UpdatedAllocated(allocated.size(), manager_id);
      return mark;
    }

    void Sweep(int mark) {
      context->MarkDone();
      allocated.remove_if([mark](std::unique_ptr<MemoryPiece<T>>& ptr) { return ptr->Mark() != mark; });
      context->SweepDone(allocated.size(), manager_id);
    }

    void Release() {
     	int status = context->Release();
    	while ( status > 0) {
    		Sweep(status);
    		status = context->Release();
    	}
    }
};

#define MAX_PERIOD 100

template
<typename T>
class MemoryFunctor {
private:
	MemoryManager<T>* memory_manager;
	int mark = 0;
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

	void Cleanup() {
		++period;
		if ( period < MAX_PERIOD )
			return;
		period = 0;
		int tmp = memory_manager->Status();
		if ( tmp == 0 )
			return;
		mark = tmp;
		this->PerformMarking();
		memory_manager->Sweep(mark);
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

    MemoryManagerFactory(int amount_allowed) : context(amount_allowed) {}
};

}




#endif /* UTILS_MEMORY_H_ */
