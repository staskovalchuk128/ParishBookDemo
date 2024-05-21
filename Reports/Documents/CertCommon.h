/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "..//Reports.h"

struct CertPrintTempData
{
	std::wstring mainName;
	std::wstring fathersName;
	std::wstring mothersName;
	std::wstring birthDate;
	std::wstring baptizeDate;
	std::wstring dated;
	std::wstring pastor;
};

class CertCommon : public PageCommon, protected Reports
{
public:
	CertCommon(HWND hWnd, HINSTANCE hInst, std::wstring certType);
	virtual ~CertCommon();

	virtual void DrawCert(PrinterDrawer* printer);
	virtual void DrawFancyCert(PrinterDrawer* printer);
	virtual void DrawGodParentCert(PrinterDrawer* printer);

	virtual void ValidateFields();
	virtual void ClearAllFields();
	virtual void SaveToHistory(std::function<void(void)> callback = nullptr);
	virtual void FillFromHistory(int id);
	virtual std::vector<std::pair<std::wstring, std::string>> GetFillTableFields();

	std::wstring GetCertificateTypeStr();

	virtual void SaveToTempData();
	virtual void FillFromTempData();

	void OnPrinted(Printer* printerPtr);

	void LinkTempData(CertPrintTempData* tempDataPtr);
protected:
	std::vector<std::pair<std::wstring, std::string>> fillTableFields;
	const std::wstring certificateTypeStr;

	// defined in CertificatePrintPage.h
	CertPrintTempData* tempDataPtr;

	std::wstring certConfigName;
	std::wstring selectionConfigName;
};

