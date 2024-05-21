/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <algorithm>
#include "Control.h"

#define BTN_STYLE_EMPTY_SQR 1

struct RadioBtnGroup;

class RadioBtnCtrl : public Control
{
public:
	RadioBtnCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles = WS_CHILD | WS_VISIBLE, DWORD exStyles = 0);
	~RadioBtnCtrl();

	void SetGroupPtr(RadioBtnGroup* groupPtr);
	bool IsChecked();
	void SetCheck(bool v);
	void Draw(LPDRAWITEMSTRUCT pdis);

	RadioBtnGroup* GetGroupPtr();
private:
	bool checked;
	RadioBtnGroup* groupPtr;
	static LRESULT CALLBACK CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK ParentProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};


struct RadioBtnGroup {
	std::vector<RadioBtnCtrl*> items;
	void UncheckAll() {
		std::for_each(items.begin(), items.end(), [](RadioBtnCtrl* item) {
			item->SetCheck(false);
			InvalidateRect(item->GetHWND(), NULL, FALSE);
			});
	}
	int GetCheckedId() {
		std::vector<RadioBtnCtrl*>::iterator it = std::find_if(items.begin(), items.end(), [](RadioBtnCtrl* item) {
			return item->IsChecked();
			});
		if (it != items.end()) return (*it)->GetId();
		return -1;
	}
};
