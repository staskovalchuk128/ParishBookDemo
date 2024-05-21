/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Common/PageCommon.h"


class FamDeactivator : public PageCommon
{
public:

	FamDeactivator(HWND hWnd, HINSTANCE hInst);
	~FamDeactivator();

	void AddControls();

	bool OnCommand(WPARAM wParam);

	void UpdateTable(std::function<void(void)> onUpdated);

	void DeactivateFamilies();
	void OnFindFamilies();

	void RemoveFamilyFromList();
private:
	TableCreater* table;

	static LRESULT CALLBACK Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	std::vector<int> currentFamiliesList;
};