#pragma once

#include <string>
#include <cstdint>

namespace EngineTypes {
	struct ConnectionDetails;
}

class ServerInformation {
public:
	ServerInformation() { };
	ServerInformation(const EngineTypes::ConnectionDetails* details);
	ServerInformation(std::string ip, std::uint16_t port)
		: ip(ip), port(port) { };
	std::string ip;
	std::uint16_t port;
	std::wstring password;
};