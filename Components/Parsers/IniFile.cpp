#include "IniFile.h"

#include "..//..//Common/CFunctions.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*

CIniFile	iniFile(".\\test1.ini", 1024);
iniFile.SetIniFileName(".\\test2.ini");					// change the file name after the class has bee created

// test the writing to the INI file
iniFile.WriteSection("section1", "key1=test1\x000key2=test2");	// make a section with 2 new keys
iniFile.WriteSection("section2", "key1=test1");			// make another section
iniFile.WriteString("section1", "key3", "test3");		// create a new key
iniFile.WriteString("section1", "key2", "test4");		// update an existing key
iniFile.WriteString("section1", "key1", NULL);			// remove an existing key
iniFile.RemoveKey("section1", "key3");					// remove another existing key
iniFile.WriteNumber("section1", "key4", 123);			// write an integer to a new key
iniFile.WriteNumber("section1", "key5", -123);			// write a negative integer to a new key
iniFile.WriteNumber("section1", "key6", -123.456f);		// write a float to a new key
TestStruct writeTestStruct;
writeTestStruct.x = 6;
writeTestStruct.y = 7;
strcpy(writeTestStruct.charr, "abcdefg");
iniFile.WriteStruct("section1", "key7", &writeTestStruct, sizeof(TestStruct));


// Test the reading from the INI file
CString	str;											// string that will receive the output 
iniFile.GetString("section1", "key2", str, "default");	TRACE("key2=%s\n", str);
iniFile.GetString("section1", "nokey", str, "default");	TRACE("nokey=%s\n", str);	// non-existent key
iniFile.GetString("section3", "key1", str, "default");	TRACE("nokey=%s\n", str);
TRACE("key4=%d\n", iniFile.GetInt("section1", "key4", 0));
TRACE("key5=%d\n", iniFile.GetInt("section1", "key5", 0));
TRACE("key6=%f\n", iniFile.GetFloat("section1", "key6", 0));
TestStruct readTestStruct;
iniFile.GetStruct("section1", "key7", &readTestStruct, sizeof(TestStruct));
TRACE("key7= %ld %ld %s\n", readTestStruct.x, readTestStruct.y, readTestStruct.charr);

// Test the section list retreival
CStringList lstSectionNames;
iniFile.GetSectionNames(lstSectionNames);
TRACE("Sections:\n");
for (POSITION pos = lstSectionNames.GetHeadPosition(); pos != NULL; )
{
	TRACE("\t%s\n", lstSectionNames.GetNext(pos));
}
*/

IniFile::IniFile() {};
IniFile::IniFile(LPCWSTR	strIniFileName,		// [in] new file name
	INT		iMaxStringLength)	// [in] max length of a string that can be wtritten/read to/from the INI file by this instance
	: m_MAXSTRLEN(iMaxStringLength)
{
	SetIniFileName(strIniFileName);
}


IniFile::~IniFile()
{
}

// PURPOSE: Get the name of the INI file.
std::wstring IniFile::GetIniFileName()
{
	return m_strFileName;
}

// PURPOSE: Set the name of the INI file.
void
IniFile::SetIniFileName(LPCWSTR lpIniFileName)	// [in] new file name
{
	m_strFileName = lpIniFileName;
}

// PURPOSE: Get the status of the ini file.  
//			This can be used, for example, to check the existence of the file.
BOOL
IniFile::GetStatus(std::wstring rStatus)	// [out] A reference to a user-supplied CFileStatus structure that will receive the status information.
{
	BOOL bRetVal = TRUE;
	//bRetVal = CFile::GetStatus(m_strFileName, rStatus);
	return bRetVal;
}


// PURPOSE:			Create a new section.
// NOTE:	If the INI file doesn't exist, this method creates it.
//			::WritePrivateProfileSection() can create keys with duplicate names.  That can be viewed asa bug,
BOOL		// TRUE, if the API function call was successful.  FALSE, if there was an error.  To get extended error information, call ::GetLastError(). 
IniFile::WriteSection(LPCWSTR lpSection,	// [in] name of the new section
	LPCWSTR lpData)		// [in] for the new section.  See help for the ::WritePrivateProfileSection(...) function.
{
	BOOL bRetVal = FALSE;
	bRetVal = ::WritePrivateProfileSection(lpSection, lpData, m_strFileName.c_str());
	return bRetVal;
}

// PURPOSE:	Write string data to the INI file
BOOL		// TRUE, if the API function call was successful.  FALSE, if there was an error.  To get extended error information, call ::GetLastError(). 
IniFile::WriteString(LPCWSTR lpSection,	// [in] section
	LPCWSTR lpKey,		// [in] key. If key with this name doesn't exist, it's created.
	LPCWSTR lpString)		// [in] String to be written into the file.  If this parameter is NULL, the key pointed to by the lpKeyName parameter is deleted.
{
	assert(lpString == NULL || wcslen(lpString) < m_MAXSTRLEN);
	return ::WritePrivateProfileString(lpSection, lpKey, lpString, m_strFileName.c_str());
}

// PURPOSE:	Write an integer (signed) to the INI file
BOOL		// TRUE, if the API function call was successful.  FALSE, if there was an error.  To get extended error information, call ::GetLastError(). 
IniFile::WriteNumber(LPCWSTR lpSection, 	// [in] section name
	LPCWSTR lpKey, 		// [in] key name
	INT iValue)			// [in] integer value to be written to the INI file
{
	std::wstring str(std::to_wstring(iValue));
	return WriteString(lpSection, lpKey, (LPCWSTR)str.c_str());
}

// PRUPOSE: Write the data into the specified key in the INI file.
BOOL		// TRUE, if the API function call was successful.  FALSE, if there was an error.  To get extended error information, call ::GetLastError(). 
IniFile::WriteStruct(LPCWSTR lpSection,	// [in] pointer to section name
	LPCWSTR lpKey,		// [in] pointer to key name
	LPVOID lpStruct,		// [in] pointer to buffer that contains data to add
	UINT iSizeStruct)		// [in] size, in bytes, of the buffer
{
	BOOL bRetVal = FALSE;
	bRetVal = ::WritePrivateProfileStruct(lpSection, lpKey, lpStruct, iSizeStruct, m_strFileName.c_str());
	return bRetVal;
}

// PURPOSE:	Write a float to the INI file.
BOOL		// TRUE, if the API function call was successful.  FALSE, if there was an error.  To get extended error information, call ::GetLastError(). 
IniFile::WriteNumber(LPCWSTR	lpSection,	// [in] section name
	LPCWSTR	lpKey,		// [in] key name
	FLOAT		fValue)		// [in] floating point value to be written to the INI file
{
	std::wstring str(std::to_wstring(fValue));
	return WriteString(lpSection, lpKey, (LPCWSTR)str.c_str());
}

// PURPOSE:	Remove a key from a specified section.
BOOL
IniFile::RemoveKey(LPCWSTR lpSection,		// [in] section name
	LPCWSTR lpKey)			// [in] key name
{
	return WriteString(lpSection, lpKey, NULL);
}


// PURPOSE:	Read an integer to the INI file.
// NOTE:	The MSDN article on ::GetPrivateProfileInt(...) says: "If the value of the key is less than zero, the return value is zero."
//			However, ::GetPrivateProfileInt returns negative integers just fine.
UINT
IniFile::GetInt(LPCWSTR lpSection,			// [in] section name
	LPCWSTR lpKey,				// [in] key name
	const INT iDefaultValue)	// [in] default value, which is returned, if the key is not found
{
	UINT iRet = ::GetPrivateProfileInt(lpSection, lpKey, iDefaultValue, m_strFileName.c_str());
	// ::GetLastError() is of no use here, because ::GetPrivateProfileInt() doesn't set an error-code.
	return iRet;
}

// PURPOSE:	Read a string from the INI file.
BOOL		// TRUE, if successful.  FALSE otherwise.
IniFile::GetString(LPCWSTR lpSection,		// [in] section name
	LPCWSTR lpKey,			// [in] key name
	std::wstring& strRet,		// [out] CString that receives the tring from the INI file
	LPCWSTR lpDefault)		// [in] default string, which is copied into strRet, if the key is not found.  NOTE: this pointer can't be NULL.
{
	assert(lpDefault != NULL);
	assert(wcslen(lpDefault) < m_MAXSTRLEN);
	return IniFile::GetString(lpSection, lpKey, strRet, lpDefault, m_MAXSTRLEN);
}

// PURPOSE:	Read a string from the INI file. (Helper function.)
// NOTE:	The calling code should check if strRet == lpDefault.
BOOL		// TRUE, if API call successful (incl. the case, when strRet == lpDefault).  
			// FALSE otherwise (incl. memory exception
	IniFile::GetString(LPCWSTR lpSection,		// [in] section name
		LPCWSTR lpKey,			// [in] key name
		std::wstring& strRet,		// [out] string that receives the tring from the INI file
		LPCWSTR lpDefault,		// [in] default string, which is copied into strRet, if the key is not found.  NOTE: this pointer can't be NULL. 
		const DWORD iSize)		// [in] buffer size
{
	wchar_t pBuffer[MAX_PATH];

	DWORD iRet = ::GetPrivateProfileString(lpSection, lpKey, lpDefault, pBuffer, iSize, m_strFileName.c_str());
	assert(iRet < iSize);
	size_t len = wcslen(pBuffer);
	strRet.assign(pBuffer, pBuffer + len);
	return TRUE;
}

// PURPOSE:	Read an integer to the INI file.
FLOAT
IniFile::GetFloat(LPCWSTR lpSection,		// [in] section name
	LPCWSTR lpKey,			// [in] key name
	const FLOAT fDefaultValue) // [in] default value, which is returned, if the key is not found, or if there's a memory exception
{
	const int MAXFLOATDIGS = 512;			// maximum number of digits that a floating point numebr can have.
	std::wstring strRet, strDefault(std::to_wstring(fDefaultValue));
//	strDefault.Format("%f", fDefaultValue);	// alternatinely, the default could be a non-numeric character, which would be checked with ?: in the return line
	BOOL bRet = IniFile::GetString(lpSection, lpKey, strRet, (LPCWSTR)strDefault.c_str(), MAXFLOATDIGS);
	return (bRet ? (FLOAT)atof(WstrToStr(strRet).c_str()) : fDefaultValue);
}

// PURPOSE:	Read a struct from the INI file
BOOL
IniFile::GetStruct(LPCWSTR lpSection,		// [in] section name 		  
	LPCWSTR lpKey, 			// [in]	key name	  
	LPVOID	lpRetStruct,	// [out] pointer to thye return buffer
	const UINT iSizeStruct) // [in] size of the return buffer
{
	assert(lpRetStruct != NULL);
	assert(iSizeStruct > 0);
	return ::GetPrivateProfileStruct(lpSection, lpKey, lpRetStruct, iSizeStruct, m_strFileName.c_str());
}

// PURPOSE:	Get the list of section names from the INI file.
void
IniFile::GetSectionNames(std::wstring lstSectionNames)	// [out] array of section name strings
{
	// Get the section names through an API function call.
	LPTSTR lpRetBuff[MAX_PATH] = { 0 };
	DWORD iRetVal = ::GetPrivateProfileSectionNames(*lpRetBuff, m_MAXSTRLEN, m_strFileName.c_str());


}