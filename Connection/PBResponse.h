/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <string>
#include <map>
#include <vector>

class PBResponse
{
public:
	PBResponse();
	PBResponse(std::string res);

	bool Succeed();
	const std::wstring& GetError();
	std::vector<std::map<std::string, std::wstring>> GetResult();
	std::map<std::string, std::wstring> GetResultMap(std::string key);
	std::vector<std::map<std::string, std::wstring>> GetResultVec(std::string key);

private:
	bool succeed;
	std::wstring error;
	std::map<std::string, std::map<std::string, std::wstring>> resultMap;
	std::map<std::string, std::vector<std::map<std::string, std::wstring>>> resultVec;
};