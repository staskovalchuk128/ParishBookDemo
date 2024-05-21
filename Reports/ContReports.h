/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Common/PageCommon.h"
#include "Reports.h"

class ContReports : public Reports
{
public:

	ContReports(HWND hWnd);
	~ContReports();
	void Print(int batchId);
private:
	HWND hWnd;
	int currentBatchId;
	std::map<std::string, std::wstring> batchInfo;
	std::vector<std::map<std::string, std::wstring>> batchItems;
	
	void OnDataLoaded(PBResponse res);
	void DrawReport(PrinterDrawer* printer);
	static LRESULT CALLBACK ContReportsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};