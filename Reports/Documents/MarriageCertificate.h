/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../../Common/PageCommon.h"
#include "..//../Controls/TableCtrl.h"
#include "../../Components/UserSearcher.h"
#include "..//Reports.h"
#include "..//..//Components/Printer/Printer.h"
#include "CertCommon.h"

class MarriageCertificate : public CertCommon
{
public:
	MarriageCertificate(HWND hWnd, HINSTANCE hInst);
	~MarriageCertificate();

	void AddControls();

	void DrawCert(PrinterDrawer* printer);
	void DrawFancyCert(PrinterDrawer* printer);

	bool OnCommand(WPARAM wParam);
	void ValidateFields();


	void ClearAllFields();
	void SaveToHistory(std::function<void(void)> callback = nullptr);
	void FillFromHistory(int id);
	void SaveToTempData();
	void FillFromTempData();
	std::vector<std::pair<std::wstring, std::string>> GetFillTableFields();
};