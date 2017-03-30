#pragma once

#include "EventSubscriber.h"
#include <vector>
#include <WinDef.h>
#include <memory>

class Sentinel : public EventSubscriber {
	DWORD run();
	void processQueue();
	EventSubscriber* dispatcher;
	void pingAnticheat();
	void measureResponse();
	void healthCheck();
	void setThreadHandle(std::shared_ptr<Event> event);
	bool exit;
	int responseTime, requestTime;
	int outstandingRequests;
	HANDLE anticheat;

public:
	void registerDispatcher(EventSubscriber* dispatcher);
};