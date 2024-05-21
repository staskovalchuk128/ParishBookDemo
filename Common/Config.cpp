#include "Config.h"
#include "CFunctions.hpp"

Config::Config()
{
	std::wstring currentPath = GetCurrentPath() + CONFIG_FILE_NAME;
	iniFile = IniFile((LPWSTR)currentPath.c_str(), 1024);
}

Config::~Config()
{
}

void Config::RemoveKey(std::wstring section, std::wstring key)
{
	iniFile.RemoveKey(section.c_str(), key.c_str());
}

void Config::SetBackupPath(std::wstring newPath)
{
	iniFile.WriteString(L"backup", L"path", newPath.c_str());
}

void Config::SetBackupType(std::wstring type)
{
	iniFile.WriteString(L"backup", L"type", type.c_str());
}

void Config::GetValue(std::wstring section, std::wstring key, std::wstring& val)
{
	iniFile.GetString(section.c_str(), key.c_str(), val, L"");
}
void Config::GetValue(std::wstring section, std::wstring key, float& val)
{
	iniFile.GetFloat(section.c_str(), key.c_str(), val);
}
void Config::GetValue(std::wstring section, std::wstring key, int& val)
{
	iniFile.GetInt(section.c_str(), key.c_str(), val);
}

void Config::SetValue(std::wstring section, std::wstring key, std::wstring value)
{
	iniFile.WriteString(section.c_str(), key.c_str(), value.c_str());
}