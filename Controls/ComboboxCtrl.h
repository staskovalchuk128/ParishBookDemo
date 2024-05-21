/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Control.h"


class ComboboxCtrl : public Control
{
public:
	ComboboxCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles = WS_CHILD | WS_VISIBLE, DWORD exStyles = 0);
	~ComboboxCtrl();
	void Draw(LPDRAWITEMSTRUCT pdis);
private:
	int comboBoxOffset = 10;
	static LRESULT CALLBACK CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

