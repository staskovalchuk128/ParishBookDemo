/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <sstream>
#include <codecvt>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

inline int timeZoneOffset = 0;
static bool timeZoneOffsetSet = false;

inline RECT GetWinRc(HWND h)
{
	RECT Rect;
	GetWindowRect(h, &Rect);

	WINDOWPLACEMENT place;
	memset(&place, 0, sizeof(WINDOWPLACEMENT));
	place.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(h, &place);

	if (place.showCmd == SW_SHOWMAXIMIZED)
	{
		Rect.right -= Rect.left * -1;
		Rect.bottom -= Rect.top * -1;
		Rect.left = 0;
		Rect.top = 0;
	}

	return Rect;
}


inline std::wstring GetDayEnd(int day)
{
	std::wstring str = L"th";
	if (day == 1 || day == 21 || day == 31) str = L"st";
	if (day == 2 || day == 22) str = L"nd";
	if (day == 3 || day == 23) str = L"rd";
	return str;
}

inline RECT GetLocalCoordinates(HWND h)
{
	RECT Rect = GetWinRc(h);
	MapWindowPoints(HWND_DESKTOP, GetParent(h), (LPPOINT)&Rect, 2);
	return Rect;
}

inline std::wstring GetWindowTextStr(HWND h)
{
	wchar_t buff[MAX_PATH];
	GetWindowTextW(h, buff, _countof(buff));
	return std::wstring(*buff != NULL ? buff : L"");
}

inline std::wstring StrToWStr(std::string str)
{
	return std::wstring(str.begin(), str.end());
}

inline std::string WstrToStr(std::wstring wstr)
{
	std::string str;
	std::transform(wstr.begin(), wstr.end(), std::back_inserter(str), [](wchar_t c)
		{
			return (char)c;
		});
	return str;
}

inline std::string Ltrim(std::string str)
{
	str.erase(str.begin(), find_if(str.begin(), str.end(), [](unsigned char ch)
		{
			return !isspace(ch);
		}));
	return str;
}

inline std::string Rtrim(std::string str)
{
	str.erase(find_if(str.rbegin(), str.rend(), [](unsigned char ch)
		{
			return !isspace(ch);
		}).base(), str.end());
	return str;
}

inline std::wstring Ltrim(std::wstring str)
{
	str.erase(str.begin(), find_if(str.begin(), str.end(), [](wchar_t ch)
		{
			return !isspace(ch);
		}));
	return str;
}

inline std::wstring Rtrim(std::wstring str)
{
	str.erase(find_if(str.rbegin(), str.rend(), [](wchar_t ch)
		{
			return !isspace(ch);
		}).base(), str.end());
	return str;
}

inline std::string Trim(std::string str)
{
	return Ltrim(Rtrim(str));
}

inline std::wstring Trim(std::wstring str)
{
	return Ltrim(Rtrim(str));
}

inline std::string RemoveAllSpaces(std::string str)
{
	str.erase(remove_if(str.begin(), str.end(), [](unsigned char x)
		{
			return isspace(x);
		}), str.end());
	return str;
}

inline std::wstring RemoveAllSpaces(std::wstring str)
{
	str.erase(remove_if(str.begin(), str.end(), [](wchar_t x)
		{
			return isspace(x);
		}), str.end());
	return str;
}

inline std::string RemoveAllLineBreaks(std::string str)
{
	str.erase(remove_if(str.begin(), str.end(), [](unsigned char x)
		{
			return x == '\n' || x == '\r';
		}), str.end());
	return str;
}

inline std::wstring RemoveAllLineBreaks(std::wstring str)
{
	str.erase(remove_if(str.begin(), str.end(), [](wchar_t x)
		{
			return x == '\n' || x == '\r';
		}), str.end());
	return str;
}

inline std::vector<std::string > SplitStr(const std::string& s, char d)
{
	std::vector<std::string > r;
	size_t j = 0;
	for (size_t i = 0; i < s.length(); i++)
	{
		if (s[i] == d)
		{
			r.push_back(s.substr(j, i - j));
			j = i + 1;
		}
	}

	r.push_back(s.substr(j));
	return r;
}

inline std::vector<std::wstring > SplitStr(const std::wstring& s, wchar_t d)
{
	std::vector<std::wstring > r;
	size_t j = 0;
	for (size_t i = 0; i < s.length(); i++)
	{
		if (s[i] == d)
		{
			r.push_back(s.substr(j, i - j));
			j = i + 1;
		}
	}

	r.push_back(s.substr(j));
	return r;
}

inline std::wstring GetLastErrorAsString()
{
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
	{
		return std::wstring();
	}

	LPWSTR messageBuffer = nullptr;

	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

	std::wstring message(messageBuffer, size);

	LocalFree(messageBuffer);

	return message;
}

inline std::wstring ChangeAmountFormat(std::wstring amount)
{
	if (amount == L"-") return L"0";
	size_t foundDollar = amount.find(L"$");
	if (foundDollar != std::wstring::npos) amount.erase(foundDollar, 1);
	size_t foundComma = amount.find(L",");
	if (foundComma != std::wstring::npos) amount.replace(foundComma, 1, L".");
	amount = RemoveAllSpaces(amount);
	return amount;
}

inline std::wstring ChangeDateFormat(std::wstring date)
{
	std::wstring newDate;

	if (date.length() < 8) return date;

	std::wstring dec = L"/";

	size_t foundPoint = date.find(L".");
	if (foundPoint != std::string::npos) dec = L".";

	size_t len = date.length();
	len = len > 10 ? 10 : len;

	for (size_t i = 0; i < len; i++)
	{
		if (i == 1)
		{
			if (date.substr(i, 1) == dec)
			{
				newDate = L"0" + date.substr(0, 1);
			}
			else
			{
				newDate = date.substr(0, 2);
			}
		}
		else if (i == 2)
		{
			if (date.substr(len - 8, 1) == dec)
			{
				newDate += L"-" + date.substr(len - 7, 2);
			}
			else
			{
				newDate += L"-0" + date.substr(len - 6, 1);
			}

			newDate = date.substr(len - 4, 4) + L"-" + newDate;
		}
	}

	if (date.length() > 10)
	{
		newDate += date.substr(10, date.length() - 1);
	}

	return newDate;
}

inline std::wstring ReverseDateFormatToNormal(std::wstring date)
{
	std::wstring newDate;

	if (date.length() < 8) return date;

	std::wstring year = date.substr(0, 4), month = date.substr(5, 2), day = date.substr(8, 2);

	newDate = month + L"/" + day + L"/" + year;

	if (date.length() > 10)
	{
		newDate += date.substr(10, date.length() - 1);
	}

	return newDate;
}

inline bool IsFloat(std::string str)
{
	std::istringstream iss(str);
	float f;
	iss >> std::noskipws >> f;
	return iss.eof() && !iss.fail();
}

inline bool IsFloat(std::wstring str)
{
	std::wistringstream iss(str);
	float f;
	iss >> std::noskipws >> f;
	return iss.eof() && !iss.fail();
}

inline bool IsNumber(std::string str)
{
	return !str.empty() && std::find_if(str.begin(),
		str.end(), [](unsigned char c)
		{
			return !(c == '-' || std::isdigit(c));
		}) == str.end();
}

inline bool IsNumber(std::wstring str)
{
	return !str.empty() && std::find_if(str.begin(),
		str.end(), [](wchar_t c)
		{
			return !(c == '-' || std::isdigit(c));
		}) == str.end();
}

inline int GetInt(std::string str)
{
	if (str.length() == 0) return 0;
	if (!IsNumber(str)) return 0;
	return stoi(str);
}

inline int GetInt(std::wstring str)
{
	if (str.length() == 0) return 0;
	if (!IsNumber(str)) return 0;
	return stoi(str);
}

inline float GetFloat(std::string str)
{
	float val = 0.00;
	if (str.length() == 0) return val;
	if (!IsFloat(str)) return val;
	val = stof(str);
	return val;
}

inline float GetFloat(std::wstring str)
{
	float val = 0.00;
	if (str.length() == 0) return val;
	if (!IsFloat(str)) return val;
	val = stof(str);
	return val;
}

inline std::string GetFloatFormatStr(std::string str)
{
	size_t foundPt = str.find(".");
	if (foundPt == std::string::npos) str += ".00";
	else
	{
		std::string tempStr = str.substr(foundPt, str.length() - 1);
		if (tempStr.length() > 2)
		{
			str = str.substr(0, foundPt + 3);
		}
	}

	return str;
}

inline std::wstring GetFloatFormatStr(std::wstring str)
{
	size_t foundPt = str.find(L".");
	if (foundPt == std::wstring::npos) str += L".00";
	else
	{
		std::wstring tempStr = str.substr(foundPt, str.length() - 1);
		if (tempStr.length() > 2)
		{
			str = str.substr(0, foundPt + 3);
		}
	}

	return str;
}

inline SYSTEMTIME GetSysDate(std::wstring str)
{
	std::wstring d, m, y;

	size_t mPos = str.find(L"/");
	if (mPos != std::string::npos)
	{
		m = str.substr(0, mPos);

		size_t dPos = str.find(L"/", mPos + 1);
		if (dPos != std::string::npos)
		{
			d = str.substr(mPos + 1, dPos - 2);
			y = str.substr(dPos + 1, str.length() - 1);
		}
	}

	SYSTEMTIME st = { 0 };

	st.wYear = GetInt(y);
	st.wMonth = GetInt(m);
	st.wDay = GetInt(d);
	return st;
}

inline int GetExtraWindowHeight(HWND hWnd)
{
	RECT w, c;
	GetWindowRect(hWnd, &w);
	GetClientRect(hWnd, &c);
	return (w.bottom - w.top) - (c.bottom - c.top);
}

inline int GetExtraWindowWidth(HWND hWnd)
{
	RECT w, c;
	GetWindowRect(hWnd, &w);
	GetClientRect(hWnd, &c);
	return (w.right - w.left) - (c.right - c.left);
}

inline std::string StrToLow(std::string str)
{
	for_each(str.begin(), str.end(), [](char& c)
		{
			c = ::tolower(c);
		});
	return str;
}

inline std::wstring WstrToLow(std::wstring str)
{
	for_each(str.begin(), str.end(), [](wchar_t& c)
		{
			c = ::tolower(c);
		});
	return str;
}

inline std::string StrToUp(std::string str)
{
	for_each(str.begin(), str.end(), [](char& c)
		{
			c = ::toupper(c);
		});
	return str;
}

inline std::wstring WstrToUp(std::wstring str)
{
	for_each(str.begin(), str.end(), [](wchar_t& c)
		{
			c = ::toupper(c);
		});
	return str;
}

inline std::wstring GetCurrentYear()
{
	struct tm tm;
	time_t now = time(0);
	localtime_s(&tm, &now);
	return std::to_wstring(tm.tm_year + 1900);
}


inline std::wstring GetCurrentDate(std::wstring sep, bool fullDate = false)
{
	struct tm tm;
	time_t now = time(0);
	localtime_s(&tm, &now);

	int day = tm.tm_mday, month = tm.tm_mon + 1;

	std::wstring currentDate = (month < 10 ? L"0" : L"") + std::to_wstring(month) + sep + (day < 10 ? L"0" : L"") + std::to_wstring(day) + sep + std::to_wstring(tm.tm_year + 1900);

	if (fullDate == true)
	{
		int hours = tm.tm_hour, minutes = tm.tm_min;
		currentDate += L" ";
		currentDate += (hours < 10 ? L"0" : L"") + std::to_wstring(hours) + L":" + (minutes < 10 ? L"0" : L"") + std::to_wstring(minutes);
	}

	return currentDate;
}

inline std::wstring GetFileNameFromPath(std::wstring Path)
{
	size_t slash = Path.rfind(L'\\');
	return (Path.substr(slash + 1));
}

inline std::wstring GetFileName()
{
	wchar_t szPath[MAX_PATH];
	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		return L"";
	}

	return GetFileNameFromPath(std::wstring(szPath));
}

inline std::wstring GetCurrentPath()
{
	wchar_t szPath[MAX_PATH];

	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		return L"";
	}

	std::wstring fileName = GetFileName();

	std::wstring fullPath(szPath);
	fullPath.erase(fullPath.end() - fileName.length(), fullPath.end());
	return fullPath;
}

inline bool DirExists(std::wstring path)
{
	DWORD ftyp = GetFileAttributesW(path.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}


inline bool FileExists(std::wstring path)
{
	DWORD ftyp = GetFileAttributesW(path.c_str());
	return (ftyp != INVALID_FILE_ATTRIBUTES && !(ftyp & FILE_ATTRIBUTE_DIRECTORY));
}

inline std::string RemoveDoulbeSpaces(std::string str)
{
	std::string newstr;

	size_t end;
	size_t beg = 0;
	size_t len = str.length();

	while (true)
	{
		end = str.find_first_of(' ', beg) + 1;
		newstr += str.substr(beg, end - beg);
		beg = str.find_first_not_of(' ', end);
		if (end == 0) break;
	}

	return newstr;
}

inline std::wstring RemoveDoulbeSpaces(std::wstring str)
{
	std::wstring newstr;

	size_t end;
	size_t beg = 0;
	size_t len = str.length();

	while (true)
	{
		end = str.find_first_of(' ', beg) + 1;
		newstr += str.substr(beg, end - beg);
		beg = str.find_first_not_of(' ', end);
		if (end == 0) break;
	}

	return newstr;
}

inline std::string WstringToUTF8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8 < wchar_t>> myconv;
	return myconv.to_bytes(str);
}

inline std::wstring UTF8ToWstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8 < wchar_t>> myconv;
	return myconv.from_bytes(str);
}

inline void StrReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

inline void StrReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}



inline std::vector<std::map<std::string, std::wstring>> CreateVecFromJson(std::wstring json)
{
	std::vector<std::map<std::string, std::wstring>> vec;

	boost::property_tree::ptree pt;
	try
	{
		std::stringstream ss;
		ss << WstringToUTF8(json);
		boost::property_tree::read_json(ss, pt);
		for (boost::property_tree::ptree::iterator it = pt.begin(); it != pt.end(); ++it)
		{

			boost::property_tree::ptree paramsNode = it->second;

			std::map<std::string, std::wstring> mapItem = {};

			for (boost::property_tree::ptree::iterator itP = paramsNode.begin(); itP != paramsNode.end(); ++itP)
			{

				std::string key = itP->first;
				std::wstring value = UTF8ToWstring(itP->second.get_value<std::string>());
				mapItem[key] = value;
			}

			vec.push_back(mapItem);

		}
	}
	catch (const std::exception& err)
	{
		std::cout << err.what() << std::endl;
	}

	return vec;
}


inline std::wstring CreateJsonFormVec(std::vector<std::map<std::string, std::wstring>> vec)
{
	std::wstring json = L"[";

	for (auto& vecIt : vec)
	{
		json += L"{";
		for (auto& mapIt : vecIt)
		{
			json += L"\\\"" + UTF8ToWstring(mapIt.first) + L"\\\":\\\"" + mapIt.second + L"\\\",";
		}
		json.pop_back();
		json += L"},";
	}
	json.pop_back();
	json += L"]";
	return json;
}

inline std::wstring CreateJsonFormMap(std::map<std::string, std::wstring> map)
{
	std::wstring json = L"{";

	for (auto& mapIt : map)
	{
		json += L"\\\"" + UTF8ToWstring(mapIt.first) + L"\\\":\\\"" + mapIt.second + L"\\\",";
	}
	json.pop_back();
	json += L"}";
	return json;
}

inline std::map<std::string, std::wstring> CreateMapFromJson(std::wstring json)
{
	std::map<std::string, std::wstring> map;

	boost::property_tree::ptree pt;
	try
	{
		std::stringstream ss;
		ss << WstringToUTF8(json);
		boost::property_tree::read_json(ss, pt);
		for (boost::property_tree::ptree::iterator it = pt.begin(); it != pt.end(); ++it)
		{

			std::string key = it->first;
			std::wstring value = UTF8ToWstring(it->second.get_value<std::string>());
			map[key] = value;

		}
	}
	catch (const std::exception& err)
	{
		std::cout << err.what() << std::endl;
	}

	return map;
}

inline void UpdateTimeZone()
{
	if (!timeZoneOffsetSet)
	{
		TIME_ZONE_INFORMATION temp;
		GetTimeZoneInformation(&temp);

		if (temp.Bias > 0)
		{
			timeZoneOffset = -(temp.Bias / 60 - 1);
		}
		else timeZoneOffset = temp.Bias / 60 + 1;
		timeZoneOffsetSet = true;
	}
}

inline int GetTimeZoneOffset()
{
	return timeZoneOffset;
}