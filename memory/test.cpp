#include "memory/memory.h"
#include <string>
#include <assert.h>
#include <thread>
#include <vector>
#include <atomic>

using std::vector;
using std::thread;
using std::string;




void test1() {
  memory::MemoryManagerFactory<string> factory(2);
  auto manager = factory.Get();
  auto a = manager->Allocate(string("aa"));
  auto b = manager->Allocate(string("bb"));
  assert(*a == "aa");
  assert(*b == "bb");
  assert(manager->Status() != 0);
}

//TODO: add multithreaded test for memory;:

class TestObject {
  public:
    std::atomic_int counter;
    TestObject() {
      ++counter;
    }
    ~TestObject() {
      --counter;
    }
};

void ThreadTask(int repetitions, 

void test2() {
  int thread_count = 100;
  memory::MemoryManagerFactory<TestObject> factory(100);
  vector<vector<ManagedPtr<T> > pointers(thread_count);
  vector<thread> threads; 
}

int main() {
}
