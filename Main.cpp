#include <iostream>
#include <fstream>
#include <sstream>
#include "Malody.hpp"
#include "KBBeat.hpp"
#include "Converter.hpp"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include "version.hpp"
#include <Windows.h>
#include <shellapi.h>
#include <commdlg.h>

std::string ToJsonString(KBBeat::PlayerSet* set);
std::string GetBanner(const std::string& path);
std::string SelectMCZFile();

void PressKeyToContinue();

const std::string BannerPath = "./banner.txt";
const std::string OutputFileName = "./inPlaying.json";

void ListMalodyCharts(const Malody::MalodyLevel& level);

int GetTrackSize(const std::string& direction);

int main()
{	
	// change console encoding to UTF-8 to prevent
	// error when output chinese
	system("chcp 65001");
	system("cls");

	// a funny nexora banner
	std::string banner = GetBanner(BannerPath);
	std::cout << banner << '\n' << std::endl;
	auto version = Version::GetVersion();
	version.Print(std::cout);
	
	std::cout << '\n';
	std::cout << "Welcome to MCKBConverter! use this to translate your .mcz file to kbbeat inPlaying.json file" << std::endl;
	PressKeyToContinue();

	std::cout << "Select your .mcz file..." << std::endl;
	std::string mczPath = SelectMCZFile();

	if (mczPath == "")
	{
		std::cout << "file not selected or not legal" << std::endl;
		PressKeyToContinue();
		return 0;
	}

	std::cout << "Loading malody chart..." << std::endl;
	Malody::MalodyMCZReader reader(mczPath);
	auto level = reader.GetMalodyLevel();

	std::cout << "Found malody levels:" << std::endl;
	
	while (true)
	{
		ListMalodyCharts(level);
		std::cout << "Select which chart to translate: (index from 1 to " << level.charts.size() << ")" << std::endl;
		int input;
		std::cin >> input;
		if (input < 0 || input >= level.charts.size())
		{
			std::cout << "Illegal index, please input index from 1 to " << level.charts.size() << std::endl;
			continue;
		}

		auto chart = level.charts[input - 1];
		KBBeatConvert::Converter converter;
		int leftTrackSize = GetTrackSize("left");
		int rightTrackSize = GetTrackSize("right");
		KBBeat::Vector3 noteAppearPosition;
		std::cout << "Input note appear position(Vector3 type). Example: 0 0 100" << std::endl;
		std::cin >> noteAppearPosition.x >> noteAppearPosition.y >> noteAppearPosition.z;
	
		std::cout << "Converting..." << std::endl;
		auto kbbeatSet = converter.ConvertMCZToKBBeatPlayerSet(*chart, leftTrackSize, rightTrackSize, noteAppearPosition);
		std::cout << "Writing to file..." << std::endl;
		auto json = ToJsonString(kbbeatSet);
		
		std::ofstream writer("./inPlaying.json");
		writer << json;
		writer.close();

		std::cout << "Done! Converted file is inPlaying.json" << std::endl;
		delete kbbeatSet;

		system("explorer .");
		break;
	}
	PressKeyToContinue();
}

void ListMalodyCharts(const Malody::MalodyLevel& level)
{
	int index = 1;
	for (auto chart : level.charts)
	{
		std::cout << index << ": [CHART] title: " << chart->meta.song.title
			<< " artist: " << chart->meta.song.artist
			<< " bpm: " << chart->meta.song.bpm
			<< " version: " << chart->meta.version << std::endl;
		index++;
	}
}

int GetTrackSize(const std::string& direction)
{
	std::cout << "Please input the " << direction << " track size (from 1 to 5)" << std::endl;
	int result;
	while (true)
	{
		std::cin >> result;
		if (result < 1 || result > 5)
		{
			std::cout << "Illegal track size, make sure it's an integer between 1 to 5" << std::endl;
			continue;
		}

		break;
	}

	return result;
}

std::string SelectMCZFile()
{
	OPENFILENAMEA ofn;
	char szFile[512] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Malody chart file(*.mcz)\0*.MCZ\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Select a malody chart";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		return std::string(ofn.lpstrFile);
	}
	else
	{
		return "";
	}
}

void PressKeyToContinue()
{
	std::cout << "Press any key to continue..." << std::endl;
	std::cin.get();
}

std::string GetBanner(const std::string& path)
{
	std::ifstream reader(path);
	if (!reader.is_open())
	{
		return "";
	}

	std::ostringstream oStringStream;
	oStringStream << reader.rdbuf();
	reader.close();
	return oStringStream.str();
}

std::string ToJsonString(KBBeat::PlayerSet* set)
{
	rapidjson::Document document;
	document.SetObject();
	auto& allocator = document.GetAllocator();
	auto object = set->ToJsonValue(allocator);

	const int memberCount = object.MemberCount();

	for (auto member = object.MemberBegin(); member != object.MemberEnd(); member++)
	{
		document.AddMember(member->name, member->value, allocator);
	}

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	return std::string(buffer.GetString());
}