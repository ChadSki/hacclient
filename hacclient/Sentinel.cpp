#include "Sentinel.h"
#include "DebugHelper.h"
#include <cstdint>

void Sentinel::pingAnticheat() {
	OutputDebugString("Sentinel pinging!");
	std::shared_ptr<Event> event(std::make_shared<Event>(PING_ANTICHEAT_THREAD));
	broadcast(event);
	requestTime = GetTickCount();
	outstandingRequests++;
}

void Sentinel::measureResponse() {
	outstandingRequests--;

	if((GetTickCount() - requestTime) > 5000) {
		OutputDebugString("Debugger found");
	} else {
		OutputDebugString("Sentinel received response in time!");
	}
}

void Sentinel::healthCheck() {
	if(outstandingRequests > 0 && (GetTickCount() - requestTime) > 5000) {
		OutputDebugString("Debugger 1 found");
	} else if(outstandingRequests == 0) {
		pingAnticheat();
	}
	
	if(SuspendThread(anticheat) == -1) {
		OutputDebugString("error");
	}

	CONTEXT context;
	if(GetThreadContext(anticheat, &context) == 0) {
		OutputDebugString("error 2");
	}

	if(SetThreadContext(anticheat, &context) == 0) {
		OutputDebugString("error 4");
	}

	if(ResumeThread(anticheat) == -1) {
		OutputDebugString("error 3");
	}
}

void Sentinel::setThreadHandle(std::shared_ptr<Event> event) {
	ThreadHandleEvent* e = static_cast<ThreadHandleEvent*>(event.get());
	anticheat = e->acthread;
}

DWORD Sentinel::run() {
	exit = false;
	const int healthCheckFrequency = 5000;
	int lastHealthCheck = 0;

	std::shared_ptr<Event> event(std::make_shared<Event>(REQUEST_ANTICHEAT_THREAD_HANDLE));
	broadcast(event);
	
	while(!exit) {
		WaitForSingleObject(eventSem, 5000);
		processQueue();
		if(GetTickCount() - lastHealthCheck > healthCheckFrequency) {
			healthCheck();
			lastHealthCheck = GetTickCount();
		}
	}

	return 0;
}

void Sentinel::processQueue() {
	std::shared_ptr<Event> event;

	while(!events.empty()) {
		if(!events.try_pop(event)) {
			return;
		}

		switch(event->type) {
			case PONG_ANTICHEAT_THREAD:
				measureResponse();
				break;
			case ANTICHEAT_THREAD_HANDLE:
				setThreadHandle(event);
				break;
			case THREAD_EXIT:
				exit = true;
				break;
		}
	}
}

void Sentinel::registerDispatcher(EventSubscriber* dispatcher) {
	this->dispatcher = dispatcher;
	std::vector<EVENTS> events;
	events.emplace_back(POST_MAP_LOAD);
	events.emplace_back(ANTICHEAT_THREAD_HANDLE);
	events.emplace_back(PONG_ANTICHEAT_THREAD);
	dispatcher->subscribe(this, events);
}