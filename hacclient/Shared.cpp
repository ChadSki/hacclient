#include "Shared.h"
#include "ServerInformation.h"

ServerInformation lastServer;
char* mapSearchPath;
wchar_t reconnectPassword[PASSWORD_LENGTH];
wchar_t* serverName;
char* serverAddress;
char* productType;
std::uint8_t* activeCamoDisable;
std::uint8_t* masterVolume;