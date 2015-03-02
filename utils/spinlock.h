/*
 * spinlock.h
 *
 *  Created on: Mar 1, 2015
 *      Author: kp
 */

#ifndef UTILS_SPINLOCK_H_
#define UTILS_SPINLOCK_H_


#include <atomic>


namespace utils {


// Well, this one is simple.
// BUT there is one catch:
// Liveness is not ensured.
// Will do for now.
// TODO: maybe ensure liveness?
class SpinLock {
private:
    std::atomic<bool> locked;
public:
    SpinLock() : locked(false) {}

    void lock() {
    	while ( locked.exchange(true) ) {}
    }

    void unlock() {
    	locked.store(false);
    }


};


}

#endif /* UTILS_SPINLOCK_H_ */
