/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "shlobj_core.h"
#include "../Common/PageCommon.h"

class UsabilitySettings : public PageCommon
{
public:
	UsabilitySettings(HWND hw, HINSTANCE hI);
	~UsabilitySettings();

	void AddControls();
	bool OnCommand(WPARAM wParam);

private:
	std::map<int, int> batchNamesNums;
	int currentBatchEdit;

	void OnAddBatchName();
	void OnEditBatchName(std::wstring name, int id);

	void SaveBatchName(std::wstring name, int id = -1);

	void DeleteBatchName(int id);

	void UpdateBatchNames(std::function<void(void)> callback = nullptr);

	static LRESULT CALLBACK BatchesNamesProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};
