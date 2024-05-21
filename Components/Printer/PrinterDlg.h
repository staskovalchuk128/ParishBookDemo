/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "..//..//Controls/Controls.h"
#include "..//..//Common/Config.h"

class Printer;


struct PreviewPageItem
{
	Gdiplus::Bitmap* bitmap;
	HWND hWnd;
};

class PrinterDlg : private Controls
{
public:
	PrinterDlg(Printer* printerPtr, bool awaitForDataToLoad = true, short orient = DMORIENT_PORTRAIT, short paperSize = DMPAPER_LETTER);
	~PrinterDlg();
	INT_PTR OpenDlg(HWND hWnd);
	HDC GetPrinterDC();
	PRINTER_INFO_2& GetPrinterInfo();
	int GetPrintFrom();
	int GetPrintTo();
	void SetShowContentOffset(bool v, std::wstring offsetSection);
	void SaveSelectionsToConfig(std::wstring configSectionName);
	void LoadSelectionsFromConfig(std::wstring configSectionName);
	void SetSavedPrintingConfig(std::wstring savedPrintingConfig);
	void RegeneratePrinterDC();

	void OnFinishedLoadingData();
private:
	bool awaitForDataToLoad;
	std::map<int, PreviewPageItem> previewPagesItems;
	ScrollBar* previewScrollbar;
	Printer* printerPtr;
	Controls* settingsControls, *previewControls;
	Config config;
	int selectedPaperSize, selectedTray;
	bool duplexChecked;
	bool showContentOffsets;
	std::wstring contentOffsetsConfigSection;
	int printPagesFrom, printPagesTo;
	short defOrientation, defPaperSize;
	PRINTER_INFO_2 currentPrinter;
	std::vector<PRINTER_INFO_2> printersList;
	std::wstring savedPrintingConfig;

	std::vector<std::wstring> printerBinNames;
	std::vector<WORD> printerBinValues;

	std::vector<std::wstring> printerPapersNames;
	std::vector<WORD> printerPapersValues;
	HWND dlgHwnd;

	HDC generatedPrinterHDC, screenHDC;
	int totalPreviewPages;
private:
	INT_PTR CALLBACK PrinterProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK PreviewSectionProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK PreviewPageProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK SettingsSectionProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	template <typename T, INT_PTR(T::* P)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>
	INT_PTR DialogBoxThis(T* pThis, HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent);

	void LoadPrinters();

	void AddControls();
	void UpdatePrinterSettings();
	void UpdateBins();
	void UpdatePapers();
	void UpdateDlgFields();


	std::wstring GetDefaultPrinterName();
	bool OnComman(WPARAM wParam);

	void SetPagesFromTo();
	void SavePrinterSettings();


	void OnDrawPreview(HWND& hWnd, HDC& hdc);
	void OnDrawPreviewPage(HWND& hWnd, HDC& hdc);

	void ResetPreview();
};