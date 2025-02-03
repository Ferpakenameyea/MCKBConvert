#include "Malody.hpp"
#include <fstream>
#include <iostream>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <zip.h>

static Malody::MalodyChart* GetChart(const char* buffer, int* error)
{
	rapidjson::Document document;
	document.Parse(buffer);
	if (document.HasParseError())
	{
		std::cout << document.GetErrorOffset();
		*error = document.GetParseError();
		return nullptr;
	}

	return Malody::MalodyChart::FromJson(document);
}

static bool EndsWith(std::string string, std::string suffix)
{
	if (string.size() < suffix.size())
	{
		return false;
	}

	return string.substr(string.size() - suffix.size()) == suffix;
}

Malody::MalodyLevel Malody::MalodyMCZReader::GetMalodyLevel()
{
	int error = 0;
	zip* zipArchive = zip_open(this->path.c_str(), 0, &error);
	MalodyLevel level;

	if (zipArchive == nullptr)
	{
		std::cerr << "Cannot open zip archive " << this->path << " error code: " << error << std::endl;
		return level;
	}

	auto numOfEntries = zip_get_num_entries(zipArchive, 0);
	
	zip_uint64_t bufferSize = 0;

	for (int i = 0; i < numOfEntries; i++)
	{
		auto entryName = std::string(zip_get_name(zipArchive, i, 0));
		if (EndsWith(entryName, ".mc"))
		{
			struct zip_stat stat;
			error = zip_stat_index(zipArchive, i, 0, &stat);

			if (error != 0)
			{
				std::cerr << "Failed to get .mc file stat: " << stat.name << " error code: " << error << std::endl;
				continue;
			}
			std::vector<char> buffer(stat.size + 1);

			zip_file* zipFile = zip_fopen_index(zipArchive, i, 0);
			if (zipFile == nullptr)
			{
				std::cerr << "Failed to open file in zip: " << entryName << std::endl;
				continue;
			}
			
			zip_fread(zipFile, buffer.data(), buffer.size());
			
			auto chart = GetChart(buffer.data(), &error);
			if (chart == nullptr)
			{
				std::cerr << "Error when processing .mc " << entryName << ", error code: " << error << std::endl;
				continue;
			}

			level.charts.push_back(chart);
			zip_fclose(zipFile);
		}
	}

	zip_close(zipArchive);
}

Malody::MalodyChart* Malody::MalodyChart::FromJson(const rapidjson::Document& document)
{
	Malody::MalodyChart* chart = new Malody::MalodyChart();

	auto meta = document["meta"].GetObject();
	
	chart->meta.creator = meta["creator"].GetString();
	chart->meta.version = meta["version"].GetString();
	
	auto song = meta["song"].GetObject();
	chart->meta.song.title = song["title"].GetString();
	chart->meta.song.artist = song["artist"].GetString();
	chart->meta.song.bpm = song["bpm"].GetDouble();

	for (auto& noteItem : document["note"].GetArray())
	{
		auto noteObject = noteItem.GetObject();
		if (!noteObject.HasMember("column"))
		{
			continue;
		}

		Malody::Note* note = nullptr;

		if (noteObject.HasMember("endbeat"))
		{
			note = new HoldNote();	
		}
		else
		{
			note = new Note();
		}

		auto beatArray = noteObject["beat"].GetArray();
		for (int i = 0; i < 3; i++)
		{
			note->beat[i] = beatArray[i].GetInt();
		}

		note->column = noteObject["column"].GetInt();
		
		if (note->GetNoteType() == HOLD)
		{
			HoldNote* hold = (HoldNote*)note;
			auto endBeatArray = noteObject["endbeat"].GetArray();
			for (int i = 0; i < 3; i++)
			{
				hold->endBeat[i] = endBeatArray[i].GetInt();
			}
		}
		chart->notes.push_back(note);
	}

	return chart;
}