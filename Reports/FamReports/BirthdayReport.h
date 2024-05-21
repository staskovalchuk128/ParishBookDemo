/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../../Controls/TableCtrl.h"
#include "../../Common/DlgCommon.h"
#include "../Reports.h"
#include "../../Components/MonthsStruct.h"

class BirthdayReport : public DlgCommon, public Reports 
{
public:
	BirthdayReport(HWND hWnd);
	~BirthdayReport();

	void ShowDlg();
	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);
	void OnDlgResized();

	void UpdateTable(std::function<void(void)> callback = nullptr);


	void Print();

	void DrawReport(PrinterDrawer* pritner);

private:
	HWND dialogHwnd;
	std::wstring currentMonth;
	TableCreater* table;

	void OnDataLoaded(PBResponse res);
};