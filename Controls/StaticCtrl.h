/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Control.h"

class StaticCtrl : public Control
{
private:

public:
	StaticCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles = WS_CHILD | WS_VISIBLE, DWORD exStyles = 0, DWORD exStyle = 0);
	~StaticCtrl();
};

