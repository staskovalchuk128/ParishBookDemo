/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Common/DlgCommon.h"
#include "..//Reports/Reports.h"


class FundsStatsDlg : public DlgCommon, private Reports
{
public:
	FundsStatsDlg(HWND hWnd, int fundId, std::wstring fundName);
	~FundsStatsDlg();

	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);

	int ShowDlg();

	void UpdateTable(std::function<void(void)> onUpdatedCallback = nullptr);
	void OnDlgResized();

private:
	std::shared_ptr<Printer> printer;
	TableCreater* table;
	int fundId;
	std::vector<std::map<std::string, std::wstring>> printingData;

	float totalDonated;
	std::wstring fundName;

	static LRESULT CALLBACK ecProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	void DrawReport(PrinterDrawer* pritner);
	void Print();

	void OnReadyToPrint(PBResponse res);
};

