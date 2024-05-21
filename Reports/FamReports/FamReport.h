/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../../Controls/TableCtrl.h"
#include "../../Common/DlgCommon.h"
#include "../Reports.h"

class FamReport : public DlgCommon, public Reports 
{
public:
	FamReport(HWND hWnd);
	~FamReport();

	int GetRTInd(std::wstring type);

	void OnOpenMember(std::string type);
	bool HandleOpenMember();
	std::vector<TableColumns> GetDialogTableColumns(std::wstring selectedReport);

	void ShowDlg();
	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);
	void OnDlgResized();

	void UpdateTable(std::function<void(void)> callback = nullptr);


	bool Print();

	bool DrawReport();
protected:
	std::vector<std::wstring> reportTypes;
	std::wstring currentReportType;
private:
	int currentOpenedItemId;
	std::string currentLookupItemType;
	TableCreater* table;

	static LRESULT CALLBACK ReportsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};