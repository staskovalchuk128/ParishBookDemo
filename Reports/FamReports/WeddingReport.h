/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../../Controls/TableCtrl.h"
#include "../../Common/DlgCommon.h"
#include "../Reports.h"
#include "../../Components/MonthsStruct.h"


class WeddingReport : public DlgCommon, public Reports 
{
public:
	WeddingReport(HWND hWnd);
	~WeddingReport();

	void ShowDlg();
	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);
	void OnDlgResized();

	void UpdateTable(std::function<void(void)> callback = nullptr);


	void Print();

	void DrawReport(PrinterDrawer* printer);
private:
	HWND dialogHwnd;
	std::wstring currentMonth;
	TableCreater* table;

	void OnDataLoaded(PBResponse res);
};