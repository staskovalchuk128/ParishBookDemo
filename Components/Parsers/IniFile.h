/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include <assert.h>
#include <string>

class IniFile
{
protected:
	std::wstring m_strFileName; 	// path to the INI file
	INT m_MAXSTRLEN; 	// max length of a string (excluding the key name)

	BOOL	GetString(LPCWSTR lpSection, LPCWSTR lpKey, std::wstring& strRet, LPCWSTR strDefault, DWORD iSize);

public:
	IniFile();
	IniFile(LPCWSTR lpIniFileName, INT iMaxStringLength);
	~IniFile();


	std::wstring GetIniFileName();
	void SetIniFileName(LPCWSTR lpIniFileName);
	BOOL GetStatus(std::wstring rStatus);

	BOOL GetString(LPCWSTR lpSection, LPCWSTR lpKey,
		std::wstring& strRet, LPCWSTR strDefault);
	UINT GetInt(LPCWSTR lpSection, LPCWSTR lpKey, INT iDefaultValue);
	FLOAT GetFloat(LPCWSTR lpSection, LPCWSTR lpKey, FLOAT fDefaultValue);
	BOOL GetStruct(LPCWSTR lpSection, LPCWSTR lpKey,
		LPVOID lpRetStruct, UINT iSizeStruct);
	void GetSectionNames(std::wstring lstSectionNames);


	BOOL WriteSection(LPCWSTR lpSection, LPCWSTR lpData);
	BOOL WriteString(LPCWSTR lpSection, LPCWSTR lpKey, LPCWSTR lpString);
	BOOL WriteNumber(LPCWSTR lpSection, LPCWSTR lpKey, INT iValue);
	BOOL WriteNumber(LPCWSTR lpSection, LPCWSTR lpKey, FLOAT fValue);
	BOOL WriteStruct(LPCWSTR lpSection, LPCWSTR lpKey,
		LPVOID lpStruct, UINT iSizeStruct);

	BOOL RemoveKey(LPCWSTR lpSection, LPCWSTR lpKey);

};