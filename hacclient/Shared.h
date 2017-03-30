#pragma once

class ServerInformation;
class EventDispatcher;

#define ADDRESS_LENGTH 32
#define PASSWORD_LENGTH 16

extern ServerInformation lastServer;
extern wchar_t reconnectPassword[PASSWORD_LENGTH];
extern char* mapSearchPath;
extern EventDispatcher* dispatcher;