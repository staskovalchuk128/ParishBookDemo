/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "string"
#include "vector"


struct Months
{
	std::vector<std::wstring> items = { L"January",L"February",L"March",L"April",L"May",L"June",L"July",L"August",L"September",L"October",L"November",L"December" };

	std::vector<std::wstring> get() {
		return items;
	}

	std::wstring GetMonthName(int id) 
	{
		id -= 1;
		if (id < 0 || id > items.size() - 1) return L"Month not found";
		return items[id];
	}

	int operator [] (std::wstring m) 
	{
		std::vector<std::wstring>::iterator it = find(items.begin(), items.end(), m);
		if (it != items.end()) return (int)distance(items.begin(), it);
		return -1;
	}

};