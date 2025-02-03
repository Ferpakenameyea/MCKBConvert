#include "KBBeat.hpp"
#include <string>
#include <rapidjson/document.h>

rapidjson::Value KBBeat::Vector3::ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator)
{
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("x", this->x, allocator);
	object.AddMember("y", this->y, allocator);
	object.AddMember("z", this->z, allocator);

	return object;
}

rapidjson::Value KBBeat::Note::ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator)
{
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("type", 0, allocator);
	object.AddMember("strikeTime", this->strikeTime, allocator);
	object.AddMember("trackIndex", this->trackIndex, allocator);
	return object;
}

rapidjson::Value KBBeat::HoldNote::ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator)
{
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("type", 1, allocator);
	object.AddMember("strikeTime", this->strikeTime, allocator);
	object.AddMember("trackIndex", this->trackIndex, allocator);
	object.AddMember("length", this->length, allocator);
	return object;
}

rapidjson::Value KBBeat::PlayerSet::ToJsonValue(rapidjson::MemoryPoolAllocator<>& allocator)
{
	rapidjson::Value rootJson(rapidjson::kObjectType);

	rootJson.AddMember("leftTrackSize", this->leftTrackSize, allocator);
	rootJson.AddMember("rightTrackSize", this->rightTrackSize, allocator);
	rootJson.AddMember("noteAppearPosition", 
						this->noteAppearPosition.ToJsonValue(allocator), 
						allocator);
	rootJson.AddMember("bpm", this->bpm, allocator);
	
	rapidjson::Value leftNotesArray(rapidjson::kArrayType);
	for (auto note : this->leftNotes)
	{
		leftNotesArray.PushBack(note->ToJsonValue(allocator), allocator);
	}

	rootJson.AddMember("leftNotes", leftNotesArray, allocator);

	rapidjson::Value rightNotesArray(rapidjson::kArrayType);
	for (auto note : this->rightNotes)
	{
		rightNotesArray.PushBack(note->ToJsonValue(allocator), allocator);
	}

	rootJson.AddMember("rightNotes", rightNotesArray, allocator);

	return rootJson;
}