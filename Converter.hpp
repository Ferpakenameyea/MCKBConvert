#pragma once
#include "KBBeat.hpp"
#include "Malody.hpp"

namespace KBBeatConvert
{
    class Converter
    {
	public:
		KBBeat::PlayerSet* ConvertMCZToKBBeatPlayerSet(
			Malody::MalodyChart& chart,
			int leftTrackSize,
			int rightTrackSize,
			KBBeat::Vector3 noteAppearPosition);
	private:
		const int LEFT = -1;
		const int RIGHT = 1;
		KBBeat::Note* ConvertNoteFromMalody(
			Malody::Note* malodyNote, 
			float bpm,
			int leftTrackSize,
			int* out_Direction);
		float GetStrikeTime(float bpm, int beat[3]);
    };
}
