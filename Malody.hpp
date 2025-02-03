#pragma once
#include <vector>
#include <string>
#include <rapidjson/document.h>

namespace Malody
{
	enum NoteType
	{
		HIT,
		HOLD
	};

	class Song
	{
	public:
		std::string title;
		std::string artist;
		double bpm;
	};
	class Meta
	{
	public:
		std::string creator;
		std::string version;
		Song song;
	};
	
	class Note
	{
	public:
		int beat[3];
		int column;
		virtual NoteType GetNoteType() const
		{
			return HIT;
		}
	};

	class HoldNote : public Note
	{
	public:
		int endBeat[3];
		NoteType GetNoteType() const override 
		{
			return HOLD;
		}
	};
	
	class MalodyChart
	{
	public:
		Meta meta;
		std::vector<Note*> notes;
		static MalodyChart* FromJson(const rapidjson::Document& document);

		~MalodyChart()
		{
			for (auto note : this->notes)
			{
				if (note != nullptr)
				{
					delete note;
				}
			}
		}
	};

	class MalodyLevel
	{
	public:
		std::vector<MalodyChart*> charts;
	
		~MalodyLevel()
		{
			for (auto chart : this->charts)
			{
				if (chart != nullptr)
				{
					delete chart;
				}
			}
		}
	};

	class MalodyMCZReader
	{
	private:
		std::string path;
	public:
		MalodyLevel GetMalodyLevel();
		MalodyMCZReader(std::string path) : path(path) {}
	};
};
