/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Common/PageCommon.h"
#include "EditFamily.h"
#include "AddNewFamily.h"
#include "../Exporting/Exporting.h"
#include "../Reports/ContReports.h"

class FamilyEnvAssigner : public PageCommon
{
public:
	FamilyEnvAssigner(HWND hWnd, HINSTANCE hInst);
	~FamilyEnvAssigner();
	void AddControls();
	void OnAutoAssign();
	void OnClearAll();
	void SaveAll();
	void UpdateFamiliesTable();
	bool OnCommand(WPARAM wParam);
private:
	ScrollBar* childScroll;
	Controls* childCon;
	static LRESULT CALLBACK FamilyEnvAssignerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};