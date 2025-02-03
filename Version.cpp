#include "Version.hpp"
#include <Windows.h>
#include <version>
#include <iostream>

void Version::Print(std::ostream& stream)
{
	stream << productName << ' ' << copyright << ' ' << companyName << '\n'
		<< 'v' << version << '\n'
		<< description << std::endl;
}

Version Version::GetVersion()
{
	Version version;
	char filepath[256];
	GetModuleFileNameA(NULL, filepath, 256);

	DWORD dwHandle;
	DWORD dwSize = GetFileVersionInfoSizeA(filepath, &dwHandle);
	
	if (dwSize == 0)
	{
		return version;
	}

	char* buffer = new char[dwSize];
	
	GetFileVersionInfoA(filepath, dwHandle, dwSize, buffer);

	LPVOID lpVersionData;
	UINT uSize;

	VerQueryValueA(buffer, "\\StringFileInfo\\080404b0\\FileVersion", &lpVersionData, &uSize);
	version.version = std::string((char*)lpVersionData);

	VerQueryValueA(buffer, "\\StringFileInfo\\080404b0\\CompanyName", &lpVersionData, &uSize);
	version.companyName = std::string((char*)lpVersionData);

	VerQueryValueA(buffer, "\\StringFileInfo\\080404b0\\LegalCopyright", &lpVersionData, &uSize);
	version.copyright = std::string((char*)lpVersionData);

	VerQueryValueA(buffer, "\\StringFileInfo\\080404b0\\FileDescription", &lpVersionData, &uSize);
	version.description = std::string((char*)lpVersionData);

	VerQueryValueA(buffer, "\\StringFileInfo\\080404b0\\ProductName", &lpVersionData, &uSize);
	version.productName = std::string((char*)lpVersionData);

	return version;
}