#include "CSVReader.h"

CSVReader::CSVReader(std::wstring _file) : filename(_file) {}

std::wstring CSVReader::ReadFileIntoString(const std::wstring& path)
{
	auto ss = std::wostringstream{};

	std::wifstream input_file(path);
	if (!input_file.is_open())
	{
		throw std::wstring(L"Could not open the file - '" + path + L"'");
	}

	ss << input_file.rdbuf();
	return ss.str();
}

std::map<int, std::vector<std::wstring>> CSVReader::GetData(wchar_t delimiter)
{
	std::map<int, std::vector<std::wstring>> csv_contents;

	file_contents = ReadFileIntoString(filename);

	std::wistringstream sstream(file_contents);
	std::vector<std::wstring > items;
	std::wstring record;

	int counter = 0;
	int i = 0;

	while (std::getline(sstream, record))
	{
		std::wistringstream line(record);

		std::wstring prevStr;

		while (std::getline(line, record, delimiter))
		{
			if (!record.empty() && record[0] == '"')
			{
				prevStr = record.substr(1, record.length() - 1);
			}
			else if (record.length() == 1 && record[0] == ' ')
			{
				items.push_back(prevStr.length() > 0 ? prevStr + L", " + record.substr(0, record.length() - 1) : record);
				prevStr = L"";
			}
			else if (delimiter != ';' && !record.empty() && record[0] == ' ' && record[record.length() - 1] != '"')
			{
				// fix this line by delimiter != ';'
				prevStr += L", " + record.substr(1, record.length() - 1);
			}
			else
			{
				items.push_back(prevStr.length() > 0 ? prevStr + L", " + record.substr(0, record.length() - 1) : record);
				prevStr = L"";
			}
		}

		csv_contents[counter] = items;
		items.clear();
		counter += 1;
		i++;
	}

	return csv_contents;
}