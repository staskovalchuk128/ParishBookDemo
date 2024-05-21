/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"

class PBResponse;

class CitySearcher 
{
public:
	static void GetInfoByZip(std::wstring zip, std::function<void(PBResponse res)> callback);
};
