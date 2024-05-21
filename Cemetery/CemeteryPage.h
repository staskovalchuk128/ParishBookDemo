/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Common/PageCommon.h"
#include "CemeteryCommon.h"

class CemeteryPage : public CemeteryCommon, public PageCommon
{
	enum struct PageState
	{
		GRAVE_NOT_SELECTED, VIEW_GRAVE_INFO, EDIT_GRAVE
	};

public:
	CemeteryPage(HWND hWnd, HINSTANCE hInst);
	~CemeteryPage();

	void AddControls();
	bool OnCommand(WPARAM wParam);

	void OnEditGrave(int id);
	void ShowGraveInfo(int id);

	void OnGraveSelected(int graveId);

private:
	PageState pageState;
	Controls* cellInfoControls, *cellEditControls;
	std::wstring currentSearchBy;
	std::vector<std::wstring> statuses;
private:
	void SwitchState(PageState newState);
	void AddGraveCotrols();
	void CreateGraveInfoControls();
	void CreateGraveEditControls();
	void CreateGraveNotSelectedControls();
	void OnSaveGraveInfo(bool closeAfterSave);
	static LRESULT CALLBACK PageMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

