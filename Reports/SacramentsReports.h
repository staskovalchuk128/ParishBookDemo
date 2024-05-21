/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../resource.h"
#include "../Common/PageCommon.h"
#include "Reports.h"

class SacramentsReports : public PageCommon, public Reports {
private:
	static INT_PTR CALLBACK TestDraw(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SacramentsReportsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
public:

	SacramentsReports();
	~SacramentsReports();

	SacramentsReports(HWND hWnd, HINSTANCE hInst);

	void AddControls();

	void OnShowReportView();


	void DrawReport();


	bool OnPrint();

};