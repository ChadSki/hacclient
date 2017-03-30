#pragma once

class ServerInformation;

namespace EngineTypes {
	struct ConnectionDetails;
}

bool fullQueue(const char* response, const EngineTypes::ConnectionDetails* info);
bool fullQueue(const char* response, const ServerInformation& info, bool hideSplash);