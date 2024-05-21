/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "../Components/Parsers/IniFile.h"

#define CONFIG_FILE_NAME L"config.ini"

class Config 
{
private:
	IniFile iniFile;
public:
	Config();
	~Config();

	void RemoveKey(std::wstring section, std::wstring key);

	void SetBackupType(std::wstring type);
	void SetBackupPath(std::wstring newPath);


	void GetValue(std::wstring section, std::wstring key, std::wstring& val);
	void GetValue(std::wstring section, std::wstring key, float& val);
	void GetValue(std::wstring section, std::wstring key, int& val);
	void SetValue(std::wstring section, std::wstring key, std::wstring value);

};