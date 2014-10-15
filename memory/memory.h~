#ifndef __MEMORY_MEMORY_H_
#define __MEMORY_MEMORY_H_

#include "utils/logger.h"
#include <utility>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>


namespace memory {

//We expect that T type implements MarkDescendants method
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

class ThreadWaiter {
  private:
    enum State {
      CAPTURING = 1,
      RELEASING = 2,
    };
    State state = CAPTURING;
    int waiting;
    std::condition_variable waiting_queue;
    std::condition_variable waiting_for_capturing;
    bool notified;
    int waiting_limit;
  public:
    bool Wait(std::unique_lock<std::mutex>* lock) {
      while ( state == RELEASING ) {
        waiting_for_capturing.wait(*lock);
      }
      if ( waiting + 1 >= waiting_limit ) {
        state = RELEASING; 
      }
      while ( state == CAPTURING ) {
       ++waiting;
       waiting_queue.wait(*lock);
       --waiting; 
      }
      if ( waiting == 0 ) {
        state = CAPTURING;
        waiting_for_capturing.notify_all();
        return true;
      }
      return false;
    }

    void SetManagersCount(int managers_count) {
      waiting_limit = managers_count;
      if ( state == CAPTURING ) {
        if ( waiting_limit >= waiting ) {
          state = RELEASING;
          waiting_queue.notify_all();
        }
      }
    }
};
template<typename T>
class MemoryContext {
  private:
    enum State {
      WORKING = 1,
      MARKING = 2,
      SWEEPING = 3,

    }
    State = WORKING;
    std::mutex lock;
    int allocated;
    int allowed;
    int managers_count;
    int last_mark = 1;
    std::list<std::unique_ptr<MemoryPiece<T> > > to_sweep;
   
    ThreadWaiter marking_done;
    ThreadWaiter sweeping_done;

     
    std::condition_variable waiting_for_working;
    
  public:
    MemoryContext(int amount_allowed) : allowed(amount_allowed) {}
    void Init(int managers_count) {
      this->managers_count = managers_count;
      marking_done.SetManagersCount(managers_count);
      sweeping_done.SetManagersCount(managers_count);
    }

    int IncAllocated(unsigned amount) {
      std::unique_lock<std::mutex> m(lock);
      if ( state != WORKING ) {
        waiting_for_working.wait(m);
      }
      allocated += amount;
      if ( allocated >= allowed ) {
        state = MARKING;
        ++last_mark;
      }
      return state == MARKING ?  last_mark : 0;
    }

    void MarkDone(std::list<std::unique_ptr<MemoryPiece<T>>>* to_sweep) {
      std::unique_lock<std::mutex> m(lock);
      if ( marking_done.Wait(&m) ) {
        state = SWEEPING;
        to_sweep.swap(this->to_sweep);
      }
    }

    void SweepDone(unsigned amount) {
      std::unique_lock<std::mutex> m(lock);
      allocated -= amount;
      if ( sweeping_done.Wait(&m) ) {
        state = WORKING:
        waiting_for_working.notify_all();
      }
    }


    void Release(std::list<std::unique_ptr<MemoryPiece<T> > >* allocated) {
      std::unique_lock<std::mutex> m(lock);
      this->to_sweep.splice(this->allocated.end(), *allocated);
      waiting_for_working.notify_all();

    }


/*    void Release(std::list<std::unique_ptr<MemoryPiece<T> > >* allocated) {
      std::unique_lock<std::mutex> m(lock);
      this->allocated.splice(this->allocated.end(), *allocated);
    }
*/
};

template
<typename T>
class MemoryManager {
  private:
    std::list<std::unique_ptr<MemoryPiece<T> > > allocated;
    MemoryContext* context;
    int last_amount;
    int status;
  public:
    template<typename... Args>
    ManagedPtr<T> Allocate(Args&&... args) {
      allocated.push_front(std::unique_ptr<MemoryPiece<T> >(
            new MemoryPiece<T>(std::forward<Args>(args)...)));

      return ManagedPtr<T>(allocated.front().get());
    }

    MemoryManager(MemoryContext* ctx) : context(ctx) {
      ctx->Register();
    }
    // returns non zero value in case when GC is needed
    // user is expected to start marking with this value immediately 
    // and then call Sweep
    int Status() {
      //LOG(allocated.size());
      status = context->IncAllocated(allocated.size() - last_amount);
      last_amount = allocated.size();
      return status;
    }

    void Sweep() {
      std::list<std::unique_ptr<MemoryPiece<T> > > to_sweep;
      context->MarkDone(&to_sweep);
      allocated.splice(allocated.begin(), to_sweep);
      allocated.remove_if([status](std::unique_ptr<MemoryPiece<T>>& ptr) { return ptr->Mark() >= status; }); 
    }

    void Release() {
       
    }
};

//Generate MemoryManagers
//After first manager calls Status method no further managers should be created
template
<typename T>
class MemoryManagerFactory {
  private:
    MemoryContext<T> context;
    std::list<std::unique_ptr<MemoryManager<T> > > managers;
  public:
    MemoryManager<T>* Get() {
      managers.push_front(std::unique_ptr<MemoryManager<T> >(
            new MemoryManager<T>(&context)));

      return managers.front().get();
    }

    MemoryManagerFactory(int amount_allowed) : context(amount_allowed) {}
    void Init() { context.Init(managers.size()); }
};

}


#endif
