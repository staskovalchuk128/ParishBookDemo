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

class FirstCommunionCertificate : public CertCommon {
public:
	FirstCommunionCertificate(HWND hWnd, HINSTANCE hInst);
	~FirstCommunionCertificate();

	void AddControls();
	void ValidateFields();

	void DrawCert(PrinterDrawer* printer);
	void DrawFancyCert(PrinterDrawer* printer);

	bool OnCommand(WPARAM wParam);

	void ClearAllFields();
	void SaveToHistory(std::function<void(void)> callback = nullptr);
	void FillFromHistory(int id);
	void SaveToTempData();
	void FillFromTempData();
	std::vector<std::pair<std::wstring, std::string>> GetFillTableFields();
};