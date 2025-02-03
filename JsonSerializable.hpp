#pragma once

#include <rapidjson/document.h>

class JsonSerializable
{
public:
	virtual rapidjson::Value ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator) = 0;
};