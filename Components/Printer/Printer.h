/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include <Winspool.h>
#include "PrinterDlg.h"
#include "PrinterDrawer.h"

class Printer : public PrinterDrawer 
{

public:
	Printer(HWND hWnd);
	~Printer();

	bool InitPriner(std::function<void(PrinterDrawer*)> dCallback, bool awaitForLoadingData = true);

	void Print();

	void SetDocName(const wchar_t* docName);
	void SetOrientation(short orient);
	void SetPaperSize(short ps);
	void SetSavedPrintingConfig(std::wstring savedPrintingConfig);


	void CreatePredrawDC();

	void SavePrintSelections(std::wstring configSectionName);
	void SetPrintedCallback(std::function<void(Printer*)> _callback);
	void DonePredraw();
	void SetShowContentOffset(bool v, std::wstring offsetSection = L"");
	bool SetPrinterDC();


	void PrintPreview(bool predraw, int pageNumber = -1);
	void CreatePreviewDC(HDC& hdc);
	HDC& GetPrinterHDC();

	void OnFinishedLoadingReport();
private:
	bool awaitForDataToLoad;
	bool showContentOffsets;
	std::wstring contentOffsetsConfigSection;
	std::wstring loadPrintingSelectionName;

	PrinterDlg* printerDlg;
	LPDEVNAMES devNames;
	HDC hdc;
	HWND hWnd;
	DOCINFO docInfo;
	LPDEVMODE deviceMode;
	short orientation, paperSize;
	std::wstring savedPrintingConfig;
	std::function<void(Printer*)> onPrintedCallback;
	std::function<void(PrinterDrawer*)> drawCallback;

};