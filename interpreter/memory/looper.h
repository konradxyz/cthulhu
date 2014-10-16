/*
 * looper.h
 *
 *  Created on: Oct 15, 2014
 *      Author: kp
 */

#ifndef INTERPRETER_MEMORY_LOOPER_H_
#define INTERPRETER_MEMORY_LOOPER_H_

namespace memory {

class Event {
public:
	virtual ~Event(){}
};

class EventProvider {
public:
	virtual std::unique_ptr<Event> Get() = 0;
	virtual ~EventProvider() {
	}
};

class LooperContext {

};

class Looper {
private:
	std::unique_ptr<EventProvider> event_provider;
	LooperContext* context;
};

}s
#endif /* INTERPRETER_MEMORY_LOOPER_H_ */
