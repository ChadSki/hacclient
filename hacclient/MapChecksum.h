#pragma once

#include <string>
#include <stdexcept>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>

class ChecksumException : public std::runtime_error {
public:
	ChecksumException() : std::runtime_error("Map checksumming failed!") { }
	ChecksumException(std::string msg) : std::runtime_error(msg) { };
};

class MapChecksum {
	std::string filepath;
	DWORD xorBuffer[256];
	HANDLE file, map;
	BYTE* view;
	void initXorBuff();
	void createFileMapping();
	void xorChunk(DWORD& checksum, char* data, DWORD length, DWORD* xor);

public:
	MapChecksum(const std::string& name);
	~MapChecksum();
	DWORD checksum();
};