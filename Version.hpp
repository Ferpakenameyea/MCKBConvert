#pragma once

#include <string>
#include <iostream>

class Version
{
public:
	std::string version;
	std::string description;
	std::string companyName;
	std::string copyright;
	std::string productName;

	static Version GetVersion();

	void Print(std::ostream& stream);
};