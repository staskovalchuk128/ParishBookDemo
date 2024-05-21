/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../../Common/DlgCommon.h"
#include "../Reports.h"

class EndYearReport : public DlgCommon, public Reports 
{
	enum struct NameFormat
	{
		FORMAL, INFORMAL
	};
public:
	
	EndYearReport(HWND hWnd);

	~EndYearReport();

	void ShowDlg();
	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);
	void OnDlgResized();

	void Print();

	void DrawPageNum(std::vector<std::map<std::string, std::wstring>>& data, PrinterDrawer* printer);
	void DrawBottomPageInfo(PrinterDrawer* printer);
	void DrawReport(PrinterDrawer* printer);

	void DrawReportItem(std::map<std::string, std::wstring> &reportItem, PrinterDrawer* printer);

	int GetTotalPages(std::vector<std::map<std::string, std::wstring>>& data);

	bool OnPrintReport();

private:
	int printersPageHeight;
	int firstPageFits;
	int secondPageFits;
	int currentPageNum;
	int currentPrintingItemId;

	std::wstring periodFrom, periodTo;
	std::wstring priestName, thanksMsg;
	NameFormat currentNameFormat;
	std::map<NameFormat, std::wstring> nameFormats;
	void OnDataLoaded(PBResponse res);
};