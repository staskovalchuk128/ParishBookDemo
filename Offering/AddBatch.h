/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Common/DlgCommon.h"

class AddBatch : public DlgCommon 
{
public:
	AddBatch(HWND hWnd, HINSTANCE hInst, int batchId = 0);
	~AddBatch();

	int GetBatchId();
	void SaveBatch();
	bool AutoFillBatchDesc(std::wstring name);
	void OnInitMainDialog();
	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);
private:
	HWND dialogHwnd;
	int currentBatchId;
	std::map<int, int>fundsIndexes;
	std::vector<std::map<std::string, std::wstring>> batchNames;
	std::vector<std::map<std::string, std::wstring>> funds;
	std::wstring sDesc, sDate, sAmount, sComment;
	int iFundId;

	INT_PTR CALLBACK MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};