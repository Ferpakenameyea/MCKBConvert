#pragma once
#include <vector>
#include <rapidjson/document.h>
#include "JsonSerializable.hpp"

namespace KBBeat
{
	class Vector3 : public JsonSerializable
	{
	public:
		rapidjson::Value ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator) override;
		float x, y, z;

		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vector3() {}
	};

	class Note : public JsonSerializable
	{
	public:
		rapidjson::Value ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator) override;
		float strikeTime;
		int trackIndex;
	};

	class HoldNote : public Note
	{
	public:
		rapidjson::Value ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator) override;
		float length;
	};

	class PlayerSet : public JsonSerializable
	{
	public:
		int leftTrackSize;
		int rightTrackSize;
		Vector3 noteAppearPosition;
		float bpm;
		std::vector<Note*> leftNotes;
		std::vector<Note*> rightNotes;
		rapidjson::Value ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator) override;
	};
}

