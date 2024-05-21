/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Control.h"

class ScrollBar;

class ListBoxCtrl : public Control
{
public:
	ListBoxCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles = WS_CHILD | WS_VISIBLE, DWORD exStyles = 0);
	~ListBoxCtrl();

	void OnSelChange();
	void Draw(LPDRAWITEMSTRUCT pdis);

private:
	ScrollBar* scroll;
	static LRESULT CALLBACK CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK ParentProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

};

