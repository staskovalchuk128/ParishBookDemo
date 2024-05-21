/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Common/PageCommon.h"
#include "..//Reports/FamReports/BirthdayReport.h"
#include "..//Reports//FamReports//WeddingReport.h"
#include "..//Reports//FamReports//EndYearReport.h"
#include "..//Reports//FamReports//FamReport.h"
#include "Reports.h"


class FamilyReports : public PageCommon, public Reports {
public:
	FamilyReports(HWND hWnd, HINSTANCE hInst);
	~FamilyReports();

	void AddControls();

	void OnShowReportView();


	void DrawReport(PrinterDrawer* printer);

	void ShowTotalFamReport();


	bool OnPrint();

	bool OnCommand(WPARAM wParam);


};