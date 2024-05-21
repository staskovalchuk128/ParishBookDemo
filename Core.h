/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "resource.h"
#include "Windows.h"
#include "Controls/Controls.h"
#include "Controls/TabCtrl.h"
#include "Controls/ScrollBarCtrl.h"
#include "Common/PageCommon.h"
#include "Exporting/Exporting.h"

class Core : private Controls 
{
public:

	Core(HWND wnw, HINSTANCE inst);
	~Core();

	PageCommon* InitPage(int page);

	void AddControls();
	void SwitchTab(int id);

	void OnSwitchPageType(int pageTypeId, int pageId);
	void OpenPageDlg(int id);

	void Display(int pageId);
	HWND GetCoreHWND();
	HWND GetHWNDPage();

	static Core* GetCorePtr();
private:
	HWND hWnd, hWndPage;
	HINSTANCE hInst;
	TabControl* tc;
	int currentPageType;
	int currentPageId;
	PageCommon* currentPage;
	static Core* _this;

	static LRESULT CALLBACK CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};