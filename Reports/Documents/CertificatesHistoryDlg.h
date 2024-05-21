/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../../Controls/TableCtrl.h"
#include "../../Common/DlgCommon.h"
#include "../Reports.h"

class CertCommon;


class CertificatesHistoryDlg : public DlgCommon, public Reports
{
public:
	CertificatesHistoryDlg(HWND hWnd, CertCommon* certPagePtr);
	~CertificatesHistoryDlg();

	void ShowDlg();
	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);
	void OnDlgResized();

	void UpdateTable(std::function<void(void)> onUpdatedCallback = nullptr);

	void FillSelected(int id);
	void DeleteSelected(int id);

private:
	CertCommon* certPagePtr;
	HWND dialogHwnd;
	TableCreater* table;
	int currentBatchId;
	std::wstring certTypeStr;
	std::vector<std::pair<std::wstring, std::string>> tableFields;
private:
	static LRESULT CALLBACK Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};