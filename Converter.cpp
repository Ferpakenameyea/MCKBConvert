#include "Converter.hpp"

KBBeat::PlayerSet* KBBeatConvert::Converter::ConvertMCZToKBBeatPlayerSet(
	Malody::MalodyChart& chart,
	int leftTrackSize,
	int rightTrackSize,
	KBBeat::Vector3 noteAppearPosition)
{
	KBBeat::PlayerSet* set = new KBBeat::PlayerSet();

	set->leftTrackSize = leftTrackSize;
	set->rightTrackSize = rightTrackSize;
	set->noteAppearPosition = noteAppearPosition;
	set->bpm = (float)chart.meta.song.bpm;

	for (auto note : chart.notes)
	{
		int direction;
		auto kbbeatNote = this->ConvertNoteFromMalody(note, set->bpm, set->leftTrackSize, &direction);
		if (direction == LEFT)
		{
			set->leftNotes.push_back(kbbeatNote);
		}
		else
		{
			set->rightNotes.push_back(kbbeatNote);
		}
	}

	return set;
}

KBBeat::Note* KBBeatConvert::Converter::ConvertNoteFromMalody(
	Malody::Note* malodyNote, 
	float bpm,
	int leftTrackSize,
	int* out_Direction)
{
	KBBeat::Note* kbNote = nullptr;
	auto beatArray = malodyNote->beat;
	if (malodyNote->GetNoteType() == Malody::HIT)
	{
		kbNote = new KBBeat::Note();
		kbNote->trackIndex = malodyNote->column;
		kbNote->strikeTime = this->GetStrikeTime(bpm, beatArray);
	}
	else
	{
		auto hold = new KBBeat::HoldNote();
		float start = this->GetStrikeTime(bpm, beatArray);
		float end = this->GetStrikeTime(bpm, ((Malody::HoldNote*)malodyNote)->endBeat);

		hold->length = end - start;
		hold->strikeTime = start;
		hold->trackIndex = malodyNote->column;
		kbNote = hold;
	}

	if (kbNote->trackIndex >= leftTrackSize)
	{
		kbNote->trackIndex -= leftTrackSize;
		*out_Direction = RIGHT;
	}
	else
	{
		*out_Direction = LEFT;
	}
	
	return kbNote;
}

float KBBeatConvert::Converter::GetStrikeTime(float bpm, int beat[3])
{
	float timePerRow = 60 / (bpm * 96);
	return (beat[0] * 96.0f + (96.0f / beat[2]) * beat[1]) * timePerRow;
}