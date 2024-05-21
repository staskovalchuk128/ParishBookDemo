/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Control.h"

#define BTN_STYLE_EMPTY_SQR 1

class LineCtrl : public Control
{
public:
	LineCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles = WS_CHILD | WS_VISIBLE, DWORD exStyles = 0);
	~LineCtrl();

	void Draw(LPDRAWITEMSTRUCT pdis);
};

