/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>

class CSVReader 
{
public:
	CSVReader(std::wstring _file);
	std::wstring ReadFileIntoString(const std::wstring& path);

	std::map<int, std::vector<std::wstring>> GetData(wchar_t delimiter);
private:
	std::wstring filename;
	std::wstring file_contents;
};