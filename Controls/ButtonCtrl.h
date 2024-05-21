/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Control.h"

#define BTN_STYLE_EMPTY_SQR 1

class ButtonCtrl : public Control
{
public:
	ButtonCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles = WS_CHILD | WS_VISIBLE, DWORD exStyles = 0);
	~ButtonCtrl();

	void Draw(LPDRAWITEMSTRUCT pdis);
	void DrawEmptySqr(Graphics& gr, RECT rc);

	void OnMouseHover();
	void OnMouseLeave();
private:
	static LRESULT CALLBACK CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

