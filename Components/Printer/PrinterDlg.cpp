#include "PrinterDlg.h"
#include <windowsx.h>
#include "..//..//resource.h"
#include "Printer.h"
#include "..//..//Common/MessageDlg.h"
#include "..//PreloaderComponent.h"

PrinterDlg::PrinterDlg(Printer* printerPtr, bool awaitForDataToLoad, short orient, short paperSize)
    : Controls(NULL), printerPtr(printerPtr), awaitForDataToLoad(awaitForDataToLoad)
{
    screenHDC = NULL;
    generatedPrinterHDC = NULL;
    showContentOffsets = false;
    selectedPaperSize = selectedTray = 0;
    printPagesFrom = printPagesTo = 0;

    defOrientation = orient;
    defPaperSize = paperSize;

    currentPrinter = {};
    dlgHwnd = NULL;
    savedPrintingConfig = L"";
}

PrinterDlg::~PrinterDlg()
{

    for (auto &it: previewPagesItems)
    {
        delete it.second.bitmap;
    }

}

INT_PTR PrinterDlg::OpenDlg(HWND hWnd)
{
    HINSTANCE hInst = GetWindowInstance(hWnd);
   
    return DialogBoxThis<PrinterDlg, &PrinterDlg::PrinterProc >(this, hInst,
        MAKEINTRESOURCEA(IDD_COMMONDLG), hWnd);
}

void PrinterDlg::SetSavedPrintingConfig(std::wstring savedPrintingConfig)
{
    this->savedPrintingConfig = savedPrintingConfig;
}

void PrinterDlg::SavePrinterSettings()
{
    selectedPaperSize = (int)printerPapersValues[settingsControls->GetComboBoxSelectedIndex("paperSize")];
    selectedTray = printerBinValues.size() > 0 ? (int)printerBinValues[settingsControls->GetComboBoxSelectedIndex("tray")] : (int)DM_DEFAULTSOURCE;
}

void PrinterDlg::SaveSelectionsToConfig(std::wstring configSectionName)
{
    config.SetValue(configSectionName, L"paperSize", std::to_wstring(selectedPaperSize));
    config.SetValue(configSectionName, L"tray", std::to_wstring(selectedTray));
}

void PrinterDlg::LoadSelectionsFromConfig(std::wstring configSectionName)
{
    std::wstring paperSizeStr = L"", trayStr = L"";
    config.GetValue(configSectionName, L"paperSize", paperSizeStr);
    config.GetValue(configSectionName, L"tray", trayStr);
    if (defPaperSize > 0)
    {
        selectedPaperSize = defPaperSize;
    }
    else
    {
        selectedPaperSize = GetInt(paperSizeStr) > 0 ? GetInt(paperSizeStr) : selectedPaperSize;
    }
    selectedTray = GetInt(trayStr) > 0 ? GetInt(trayStr) : selectedTray;
}

void PrinterDlg::SetPagesFromTo()
{

    int gbVal = settingsControls->GetCheckedRadioBtnId("rBtnAllPages");
    if (gbVal == settingsControls->GetControlId("rBtnAllPages")) return;

    std::wstring pagesVal = GetWindowTextStr(settingsControls->GetControlHWND("pagesToPrint"));

    std::vector<std::string> strArr = SplitStr(WstrToStr(pagesVal), '-');
    if (strArr.size() >= 2)
    {
        printPagesFrom = GetInt(strArr[0]);
        printPagesTo = GetInt(strArr[1]);
    }
    else {
        printPagesFrom = GetInt(pagesVal);
    }
}

int PrinterDlg::GetPrintFrom()
{
    return printPagesFrom;
}

int PrinterDlg::GetPrintTo()
{
    return printPagesTo;

}

PRINTER_INFO_2& PrinterDlg::GetPrinterInfo()
{
    return currentPrinter;
}

HDC PrinterDlg::GetPrinterDC()
{
    return generatedPrinterHDC;
}


void PrinterDlg::AddControls()
{
    /*PREVIEW, LEFT SIDE*/

    HWND previewSHwnd = CreateStatic("previewSection", 10, 10, -600, -10, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN);

    previewScrollbar = new ScrollBar(previewSHwnd, GetInstanceModule(previewSHwnd));
    previewScrollbar->CreateScroll(21, -1, 400);
    previewScrollbar->SetHScrollRange(-1);
    previewScrollbar->SetVScrollRange(4000);

    previewControls = new Controls(previewSHwnd);

    SetWindowSubclass(previewSHwnd, PreviewSectionProc, IDC_STATIC, (DWORD_PTR)this);


    /*SETTING, RIGHT SIDE*/

    HWND settingsSHwnd = CreateStatic("settingsSection", 10, 10, 590, 600, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, L"", WS_STICK_RIGHT);
    SetWindowSubclass(settingsSHwnd, SettingsSectionProc, IDC_STATIC, (DWORD_PTR)this);

    settingsControls = new Controls(settingsSHwnd);


    settingsControls->CreateStatic("sPrinterName", 10, 16, 60, 30, WS_VISIBLE | WS_CHILD, L"Printer: ");
    HWND hPrintersList = settingsControls->CreateCombobox("printersList", 80, 10, -10, 30, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED);

    std::wstring defPrinterName = GetDefaultPrinterName();
    int selectedPrinterIndex = 0;
    for (size_t i = 0; i < printersList.size(); i++)
    {
        if (defPrinterName == std::wstring(printersList[i].pPrinterName)) selectedPrinterIndex = (int)i;
        SendMessage(hPrintersList, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)(WCHAR*)printersList[i].pPrinterName);
    }
    SendMessage(hPrintersList, CB_SETCURSEL, (WPARAM)selectedPrinterIndex, (LPARAM)0);

    SetWindowSubclass(hPrintersList, SettingsSectionProc, IDC_STATIC, (DWORD_PTR)this);


    settingsControls->CreateLine(10, 70, -10, 1, "psl", APP_COLORS::GRAY_BORDER);

    int itemsOffsetY = 20;
    int itemsSizeY = 30;

    int offsetY = itemsOffsetY + itemsSizeY;

    int nextY = 70 + itemsOffsetY;

    settingsControls->CreateStatic("sPages", 10, nextY, 60, itemsSizeY, WS_VISIBLE | WS_CHILD, L"Pages ");
    nextY += offsetY;
    settingsControls->CreateRadioBtn("rBtnAllPages", 10, nextY + 6, 120, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"All pages");
    settingsControls->CreateRadioBtn("rBtnSomePages", 130, nextY + 6, 150, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Some pages");
    settingsControls->CreateEdit("pagesToPrint", 290, nextY, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");
    settingsControls->CreateStatic("pagesToPrintInfo", 290, nextY + 35, -10, 30, WS_VISIBLE | WS_CHILD, L"Enter the number of the page or range, for example 1-5.");
    settingsControls->SetLabel("pagesToPrint", L"1-5");
    
    settingsControls->CreateRadioGroup("rGroupPages", { "rBtnAllPages", "rBtnSomePages"}, "rBtnAllPages");

    nextY += offsetY + itemsOffsetY;

    settingsControls->CreateLine(10, nextY, -10, 1, "psl2", APP_COLORS::GRAY_BORDER);

    nextY += itemsOffsetY;

    settingsControls->CreateCheckBox("printOnBothSides", 10, nextY, -1, 20, WS_VISIBLE | WS_CHILD | BS_CHECKBOX, L"Print on both sides");

    nextY += offsetY;

    settingsControls->CreateStatic("sOrientation", 20, nextY + 3, 100, itemsSizeY, WS_VISIBLE | WS_CHILD, L"Orientation: ");

    HWND btnPortrait = settingsControls->CreateRadioBtn("rPortOrient", 110, nextY, 120, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Portrait");
    HWND btnLandscape = settingsControls->CreateRadioBtn("rLandOrient", 230, nextY, 150, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Landscape");
    settingsControls->CreateRadioGroup("rGroupOrient", { "rPortOrient", "rLandOrient" }, defOrientation == DMORIENT_PORTRAIT ? "rPortOrient" : "rLandOrient");

    SetWindowSubclass(btnPortrait, SettingsSectionProc, IDC_STATIC, (DWORD_PTR)this);
    SetWindowSubclass(btnLandscape, SettingsSectionProc, IDC_STATIC, (DWORD_PTR)this);


    nextY += offsetY;

    settingsControls->CreateLine(10, nextY, -10, 1, "psl2", APP_COLORS::GRAY_BORDER);

    nextY += itemsOffsetY;

    settingsControls->CreateStatic("sPaperSize", 20, nextY + 3, 100, itemsSizeY, WS_VISIBLE | WS_CHILD, L"Paper size: ");
    HWND hPaperSize = settingsControls->CreateCombobox("paperSize", 120, nextY, -10, 30, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL);
    SetWindowSubclass(hPaperSize, SettingsSectionProc, IDC_STATIC, (DWORD_PTR)this);


    nextY += offsetY;

    settingsControls->CreateStatic("sTray", 20, nextY + 3, 100, itemsSizeY, WS_VISIBLE | WS_CHILD, L"Tray: ");
    HWND hTray = settingsControls->CreateCombobox("tray", 120, nextY, -10, 30, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED);
   
    nextY += offsetY;

    settingsControls->CreateLine(10, nextY, -10, 1, "psl2", APP_COLORS::GRAY_BORDER);

    nextY += itemsOffsetY;

    if (showContentOffsets)
    {
        std::wstring xOffsetVal, yOffsetVal;
        
        config.GetValue(contentOffsetsConfigSection, L"xOffset", xOffsetVal);
        config.GetValue(contentOffsetsConfigSection, L"yOffset", yOffsetVal);

        settingsControls->CreateStatic("sxOffset", 20, nextY + 3, 150, itemsSizeY, WS_VISIBLE | WS_CHILD, L"Content offset left: ");
        settingsControls->CreateEdit("xOffset", 170, nextY, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, xOffsetVal.c_str());
        settingsControls->SetLabel("xOffset", L"mm");


        nextY += offsetY;

        settingsControls->CreateStatic("syOffset", 20, nextY + 3, 150, itemsSizeY, WS_VISIBLE | WS_CHILD, L"Content offset top: ");
        settingsControls->CreateEdit("yOffset", 170, nextY, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, yOffsetVal.c_str());
        settingsControls->SetLabel("yOffset", L"mm");

        nextY += offsetY;

        settingsControls->CreateLine(10, nextY, -10, 1, "psl2", APP_COLORS::GRAY_BORDER);
    }
    /*
    settingsControls->CreateBtn("btnPrint", 10, -nextY + 30, 200, 30, WS_VISIBLE | WS_CHILD, L"Print", WS_STICK_RIGHT | WS_STICK_BOTTOM);
    settingsControls->CreateBtn("btnCancel", 230, -nextY + 30, 200, 30, WS_VISIBLE | WS_CHILD, L"Cancel", WS_STICK_RIGHT | WS_STICK_BOTTOM);
    */

    CreateBtn("btnPrint", 10, 10, 200, 30, WS_VISIBLE | WS_CHILD, L"Print", WS_STICK_RIGHT | WS_STICK_BOTTOM);
    CreateBtn("btnCancel", 230, 10, 200, 30, WS_VISIBLE | WS_CHILD, L"Cancel", WS_STICK_RIGHT | WS_STICK_BOTTOM);

    SetColor("btnCancel", APP_COLORS::GRAY);

    // Load selections from config if there is any
    if (savedPrintingConfig.length() > 0)
    {
        LoadSelectionsFromConfig(savedPrintingConfig);
    }

    SetFocus(GetControlHWND("btnPrint"));


    RegeneratePrinterDC();

}


std::wstring PrinterDlg::GetDefaultPrinterName()
{
    WCHAR buf[512];
    DWORD bufSize = _countof(buf);
    GetDefaultPrinter(buf, &bufSize);
    return std::wstring(buf);
}

void PrinterDlg::UpdateBins()
{
    printerBinValues.clear();
    printerBinNames.clear();

    const WCHAR* printerName = currentPrinter.pPrinterName;
    const WCHAR* printerPort = currentPrinter.pPortName;

    std::wstring err = GetLastErrorAsString();
    DWORD bins = DeviceCapabilities(printerName, printerPort, DC_BINS, nullptr, nullptr);
    err = GetLastErrorAsString();
    if (bins == 0) return;
    DWORD binNames = DeviceCapabilities(currentPrinter.pPrinterName, currentPrinter.pPortName, DC_BINNAMES, nullptr, nullptr);
    if (bins != binNames) throw("Unable to get printer bins");

    printerBinValues.resize(bins);

    DeviceCapabilities(currentPrinter.pPrinterName, currentPrinter.pPortName, DC_BINS, (WCHAR*)&printerBinValues[0],
        nullptr);

    WCHAR (*binNameValues)[24] = new WCHAR[bins][24];
    DeviceCapabilities(currentPrinter.pPrinterName, currentPrinter.pPortName, DC_BINNAMES, (LPWSTR)binNameValues, nullptr);

    for (DWORD i = 0; i < bins; i++)
    {
        printerBinNames.push_back(std::wstring(binNameValues[i]));
    }
}


void PrinterDlg::UpdatePapers()
{
    printerPapersNames.clear();
    printerPapersValues.clear();

    const WCHAR* printerName = currentPrinter.pPrinterName;
    const WCHAR* printerPort = currentPrinter.pPortName;

    DWORD papers = DeviceCapabilities(printerName, printerPort, DC_PAPERS, nullptr, nullptr);
    if (papers == 0) return;
    DWORD papersNames = DeviceCapabilities(currentPrinter.pPrinterName, currentPrinter.pPortName, DC_PAPERNAMES, nullptr, nullptr);
    if (papers != papersNames) throw("Unable to get printer papers");

    printerPapersValues.resize(papers);

    DeviceCapabilities(currentPrinter.pPrinterName, currentPrinter.pPortName, DC_PAPERS, (WCHAR*)&printerPapersValues[0],
        nullptr);

    WCHAR (*papaerNameValues)[64] = new WCHAR[papers][64];
    DeviceCapabilities(currentPrinter.pPrinterName, currentPrinter.pPortName, DC_PAPERNAMES, (LPWSTR)papaerNameValues, nullptr);

    for (DWORD i = 0; i < papers; i++)
    {
        printerPapersNames.push_back(std::wstring(papaerNameValues[i]));
    }
}

void PrinterDlg::UpdateDlgFields()
{
    HWND hPaperSize = settingsControls->GetControlHWND("paperSize"), hTray = settingsControls->GetControlHWND("tray");


    SendMessage(hTray, CB_RESETCONTENT, 0, 0);
    SendMessage(hPaperSize, CB_RESETCONTENT, 0, 0);

    
    int selectedTrayIndex = 0;
    for (size_t i = 0; i < printerBinNames.size(); i++)
    {
        SendMessage(hTray, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)printerBinNames[i].c_str());
        if (printerBinValues[i] == selectedTray)
        {
            selectedTrayIndex = (int)i;
        }
    }
    SendMessage(hTray, CB_SETCURSEL, (WPARAM)selectedTrayIndex, (LPARAM)0);

    int pnIndex = 0;
    for (size_t i = 0; i < printerPapersNames.size(); i++)
    {
        SendMessage(hPaperSize, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)printerPapersNames[i].c_str());
    }

    for (size_t i = 0; i < printerPapersNames.size(); i++)
    {
        if (printerPapersValues[i] == selectedPaperSize)
        {
            pnIndex = (int)i;
            break;
        }
        else if (printerPapersValues[i] == defPaperSize) {
            pnIndex = (int)i;
            break;
        }
    }

    
    SendMessage(hPaperSize, CB_SETCURSEL, (WPARAM)pnIndex, (LPARAM)0);


}

void PrinterDlg::RegeneratePrinterDC()
{

    int checkedOrientation = settingsControls->GetCheckedRadioBtnId("rGroupOrient");
    short orientation = checkedOrientation == settingsControls->GetControlId("rLandOrient") ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;
    short duplex = DMDUP_SIMPLEX;

    bool duplexChecked = settingsControls->IsBtnChecked("");
    if (duplexChecked) duplex = DMDUP_HORIZONTAL;

    LPDEVMODE deviceMode = currentPrinter.pDevMode;
    if (!deviceMode) {
        MessageDlg(NULL, L"Error",
            L"Cant get printer device mode.",
            MD_OK, MD_ERR).OpenDlg();
        generatedPrinterHDC = CreateDC(L"WINSPOOL", currentPrinter.pPrinterName, currentPrinter.pPortName, NULL);
    }
    else
    {
        deviceMode->dmFields = DM_PAPERSIZE | DM_DEFAULTSOURCE | DM_DUPLEX | DM_ORIENTATION;
        deviceMode->dmPaperSize = selectedPaperSize;
        deviceMode->dmDefaultSource = selectedTray;
        deviceMode->dmDuplex = duplex;
        deviceMode->dmOrientation = orientation;
        generatedPrinterHDC = CreateDC(currentPrinter.pDriverName, currentPrinter.pPrinterName, currentPrinter.pPortName, deviceMode);
    }

    screenHDC = GetDC(NULL);

    printerPtr->SetPreviewPageSize(GetDeviceCaps(generatedPrinterHDC, HORZSIZE), GetDeviceCaps(generatedPrinterHDC, VERTSIZE));
}

void PrinterDlg::ResetPreview()
{
    for (auto it: previewPagesItems)
    {
        delete it.second.bitmap;
    }
    previewPagesItems.clear();

    //Clear controls
    previewControls->Clear();
}

void PrinterDlg::UpdatePrinterSettings()
{
    try
    {
        UpdateBins();
        UpdatePapers();


        UpdateDlgFields();

        //Reset preview
        RegeneratePrinterDC();
        ResetPreview();
    }
    catch (std::wstring err)
    {
        MessageDlg(NULL, L"Error",
            err.c_str(),
            MD_OK, MD_ERR).OpenDlg();
    }

}

bool PrinterDlg::OnComman(WPARAM wParam)
{
    UINT wId = LOWORD(wParam);

    switch (HIWORD(wParam))
    {
    case BN_CLICKED:
    {
        if (wId == GetControlId("btnCancel"))
        {
            EndDialog(dlgHwnd, IDCANCEL);
            return true;
        }
        else if (wId == GetControlId("btnPrint"))
        {
            printerPtr->ResetPages();
            SavePrinterSettings();
            EndDialog(dlgHwnd, IDOK);
            return true;
        }
        else if (wId == settingsControls->GetControlId("rPortOrient"))
        {
            SavePrinterSettings();
            RegeneratePrinterDC();
            ResetPreview();
            return true;
        }
        else if (wId == settingsControls->GetControlId("rLandOrient"))
        {
            SavePrinterSettings();
            RegeneratePrinterDC();
            ResetPreview();
            return true;
        }
        break;
    }
    case CBN_SELCHANGE:
    {
        if (wId == settingsControls->GetControlId("printersList"))
        {
            int printerIndex = settingsControls->GetComboBoxSelectedIndex("printersList");

            if ((int)printersList.size() - 1 < printerIndex) 
            {
                MessageDlg(NULL, L"Error",
                    L"Printer not found",
                    MD_OK, MD_ERR).OpenDlg();
                return false;
            }

            currentPrinter = printersList[printerIndex];
            UpdatePrinterSettings();

        }
        else if (wId == settingsControls->GetControlId("paperSize"))
        {
            SavePrinterSettings();
            RegeneratePrinterDC();
            ResetPreview();
            return true;
        }
        break;
    }
    case EN_CHANGE:
    {
        if (wId == settingsControls->GetControlId("xOffset"))
        {
            config.SetValue(contentOffsetsConfigSection, L"xOffset", GetWindowTextStr(GetControlHWND("xOffset")));
        }
        else if (wId == settingsControls->GetControlId("yOffset"))
        {
            config.SetValue(contentOffsetsConfigSection, L"yOffset", GetWindowTextStr(GetControlHWND("yOffset")));
        }
        break;
    }
    }

    return false;
}

void PrinterDlg::LoadPrinters()
{
    DWORD flags = PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_NETWORK;

    std::wstring defPrinterName = GetDefaultPrinterName();

    PRINTER_INFO_2* printersInfo = NULL;
    DWORD bufSize = 0, printersCount;

    bool fOk = EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, nullptr, 2, nullptr,
        bufSize, &bufSize, &printersCount);

    if (fOk || GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
    {
        printersInfo = (PRINTER_INFO_2*)malloc(bufSize);
        fOk = EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, nullptr, 2,
            (LPBYTE)printersInfo, bufSize, &bufSize, &printersCount);
    }
    else
    {
        throw(GetLastErrorAsString());
    }


    bool curPrinterSet = false;
    for (int i = 0; i < (int)printersCount; i++)
    {
        if (defPrinterName == printersInfo[(size_t)i].pPrinterName) 
        {
            currentPrinter = printersInfo[(size_t)i];
            curPrinterSet = true;
        }
        printersList.push_back(printersInfo[(size_t)i]);
    }

    if (!curPrinterSet)
    {
        currentPrinter = printersList.size() > 0 ? printersList[0] : PRINTER_INFO_2();
    }

}

template < typename T, INT_PTR(T::* P)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) >
INT_PTR PrinterDlg::DialogBoxThis(T* pThis, HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent)
{
    return ::DialogBoxParamA(hInstance, lpTemplateName, hWndParent, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->INT_PTR
        {
            if (uMsg == WM_INITDIALOG) SetWindowLongPtr(hWnd, DWLP_USER, lParam);
            T* pThis = reinterpret_cast<T*> (GetWindowLongPtr(hWnd, DWLP_USER));
            return pThis ? (pThis->*P)(hWnd, uMsg, wParam, lParam) : FALSE;
        }, reinterpret_cast<LPARAM> (pThis));
}


void PrinterDlg::OnDrawPreviewPage(HWND& hWnd, HDC& hdc)
{
    RECT rc;

    GetWindowRect(hWnd, &rc);

    int bodyX = 0;
    int bodyY = 0;
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;


    printerPtr->BeginPreview();

    int pageWidth = GetDeviceCaps(generatedPrinterHDC, HORZRES);
    int pageHeight = GetDeviceCaps(generatedPrinterHDC, VERTRES);


    int pageWidthMM = GetDeviceCaps(generatedPrinterHDC, HORZSIZE);
    int pageHeightMM = GetDeviceCaps(generatedPrinterHDC, VERTSIZE);

    int pixelsPerMMW = GetDeviceCaps(screenHDC, HORZRES) / GetDeviceCaps(screenHDC, HORZSIZE);  // pixels per millimeter
    int pixelsPerMMH = GetDeviceCaps(screenHDC, VERTRES) / GetDeviceCaps(screenHDC, VERTSIZE);  // pixels per millimeter

    int scaledWidth = pageWidth / pixelsPerMMW;
    int scaledHeight = pageHeight / pixelsPerMMH;

    std::string controlName = previewControls->GetControlPtr(hWnd)->GetName();
    int pageNumber = GetInt(SplitStr(controlName, '_')[1]);


    if (previewPagesItems.find(pageNumber) != previewPagesItems.end() && previewPagesItems[pageNumber].bitmap)
    {

        Graphics gr(hdc);
        gr.DrawImage(previewPagesItems[pageNumber].bitmap, Rect(0, 0, width , height));
    }
}

void PrinterDlg::OnDrawPreview(HWND& hWnd, HDC& hdc)
{
    if (!generatedPrinterHDC) return;

    RECT rc;

    GetWindowRect(hWnd, &rc);


    int bodyX = 0;
    int bodyY = 0;
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    HDC hdcMem = CreateCompatibleDC(hdc);

    HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);


    Graphics gr(hdcMem);
    //gr.SetPageUnit(Unit::UnitMillimeter);

    SolidBrush bgBrush(Color(218, 220, 224));
    Pen borderPen(Color(156, 156, 156));
  
    gr.FillRectangle(&bgBrush, 0, 0, width, height);


    gr.DrawRectangle(&borderPen, 0, 0, width - 1, height - 1);


    BOOL res = BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(hdcMem);

    int pageWidthMM = GetDeviceCaps(generatedPrinterHDC, HORZSIZE);
    int pageHeightMM = GetDeviceCaps(generatedPrinterHDC, VERTSIZE);
    float fov = (float)pageWidthMM / (float)pageHeightMM;


    //Calculate control page size
    int ctrlPageWidth = static_cast<int>((width - 140));
    int ctrlPageHeight = static_cast<int>(ctrlPageWidth / fov);

    int scrollOffset = previewScrollbar->GetScrollBarPos(SB_VERT);

    int offsetBetweenPages = 50;

    int totalHeight = 0;

    //Offset the from top and bottom of scroll area
    int initialOffsetY = 10;

    //If need to create preview, for example if printer setting are changed or its a first draw
    if (previewPagesItems.size() == 0)
    {

        printerPtr->BeginPreview();

        HDC tempHdc = CreateCompatibleDC(generatedPrinterHDC);
        printerPtr->CreatePreviewDC(tempHdc);

        //Calculate number of total pages
        printerPtr->ResetPages();
        printerPtr->PrintPreview(true);
        totalPreviewPages = printerPtr->GetTotalPagesNum();


        int pageWidth = GetDeviceCaps(generatedPrinterHDC, HORZRES);
        int pageHeight = GetDeviceCaps(generatedPrinterHDC, VERTRES);

        float pixelsPerMMW = (float)GetDeviceCaps(generatedPrinterHDC, LOGPIXELSX) / (float)GetDeviceCaps(screenHDC, LOGPIXELSX);
        float pixelsPerMMH = (float)GetDeviceCaps(generatedPrinterHDC, LOGPIXELSY) / (float)GetDeviceCaps(screenHDC, LOGPIXELSX);

        int scaledWidth = static_cast<int>((float)pageWidth / pixelsPerMMW);
        int scaledHeight = static_cast<int>((float)pageHeight / pixelsPerMMH);

        //Create control for each page
        for (int i = 0; i < totalPreviewPages; i++)
        {
          
            int pageNum = i + 1;
            std::string controlName = "previewPage_" + std::to_string(pageNum);

            int offsetY = (ctrlPageHeight + offsetBetweenPages) * i;
            int pageY = initialOffsetY + offsetY - scrollOffset;

            HWND pageHwnd = previewControls->CreateStatic(controlName.c_str(), 70, pageY, ctrlPageWidth, ctrlPageHeight, WS_VISIBLE | WS_CHILD);
            previewPagesItems[pageNum] = PreviewPageItem({ NULL, pageHwnd });

            SetWindowSubclass(pageHwnd, PreviewPageProc, i, (DWORD_PTR)this);


            /*DRAW PAGES*/

            if (i == 0)
            {

                HDC pageHDC = GetDC(pageHwnd);

                HDC _hdcMem = CreateCompatibleDC(pageHDC);

                HBITMAP _bitmap = CreateCompatibleBitmap(pageHDC, pageWidth, pageHeight);
                SelectObject(_hdcMem, _bitmap);
                printerPtr->CreatePreviewDC(_hdcMem);
                printerPtr->PrintPreview(false);

                DeleteObject(_bitmap);
                DeleteDC(pageHDC);


            }

            HDC printerPageHDC = printerPtr->GetPageDc(pageNum);

            HDC _hdcMem = CreateCompatibleDC(printerPageHDC);

            HBITMAP _bitmap = CreateCompatibleBitmap(printerPageHDC, scaledWidth, scaledHeight);
            HBITMAP _oldBitmap = (HBITMAP)SelectObject(_hdcMem, _bitmap);
            

            Graphics gr(_hdcMem);

            Pen borderPen(Color(156, 156, 156));

            BitBlt(_hdcMem, 0, 0, scaledWidth, scaledHeight, printerPageHDC, 0, 0, SRCCOPY);

            gr.DrawRectangle(&borderPen, 0, 0, scaledWidth - 1, scaledHeight - 1);

            BitBlt(printerPageHDC, 0, 0, scaledWidth, scaledHeight, _hdcMem, 0, 0, SRCCOPY);

            previewPagesItems[pageNum].bitmap = new Gdiplus::Bitmap(_bitmap, gr.GetHalftonePalette());

            SelectObject(_hdcMem, _oldBitmap);
            DeleteObject(_bitmap);
            DeleteDC(_hdcMem);


            /*END DRAW PAGES*/

            totalHeight = pageY + ctrlPageHeight + initialOffsetY;
        }

        previewScrollbar->SetVScrollRange(totalHeight);

        printerPtr->EndPreview();


        UpdateWindow(hWnd);
    }



}

void PrinterDlg::OnFinishedLoadingData()
{
    PreloaderComponent::Stop();
    ResetPreview();
    InvalidateRect(GetControlHWND("previewSection"), NULL, TRUE);
}

LRESULT CALLBACK PrinterDlg::PreviewPageProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    PrinterDlg* lpData = (PrinterDlg*)dwRefData;

    switch (uMsg)
    {
    case WM_ERASEBKGND:
    {
        return TRUE;
    }
    case WM_PAINT:
    {

        HDC hdc;
        PAINTSTRUCT ps;

        hdc = BeginPaint(hWnd, &ps);

        lpData->OnDrawPreviewPage(hWnd, hdc);

        EndPaint(hWnd, &ps);


        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

LRESULT CALLBACK PrinterDlg::PreviewSectionProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    PrinterDlg* lpData = (PrinterDlg*)dwRefData;
    
    switch (uMsg)
    {
    case WM_ERASEBKGND:
    {
        return TRUE;
    }
    case WM_PAINT:
    {
        HDC hdc;
        PAINTSTRUCT ps;

        hdc = BeginPaint(hWnd, &ps);

        lpData->OnDrawPreview(hWnd, hdc);

        EndPaint(hWnd, &ps);

        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

LRESULT CALLBACK PrinterDlg::SettingsSectionProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    PrinterDlg* lpData = (PrinterDlg*)dwRefData;

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        lpData->OnComman(wParam);
        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

INT_PTR CALLBACK PrinterDlg::PrinterProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SETFOCUS:
    {
        SetFocus(GetNextDlgTabItem(GetParent((HWND)wParam), (HWND)wParam, FALSE));
        return true;
    }
    case WM_INITDIALOG:
    {
        dlgHwnd = hWnd;

        if (awaitForDataToLoad)
        {
            PreloaderComponent::Start(hWnd);
        }
        
        SetWindowTextW(hWnd, L"Printing");

        ReinitControls(dlgHwnd);	// reinit controls


        // Load available pritners
        try
        {
            LoadPrinters();
        }
        catch (std::wstring err)
        {
            MessageDlg(NULL, L"Error",
                err.c_str(),
                MD_OK, MD_ERR).OpenDlg();
        }

        AddControls();

        UpdatePrinterSettings();


        int dlgPaddingX = 10;
        int minDlgWidth = 0, minDlgHeight = 0;



        int parentWidth = GetSystemMetrics(SM_CXSCREEN);
        int parentHeight = GetSystemMetrics(SM_CYSCREEN);

        int extraHeight = GetExtraWindowHeight(hWnd);
        int extraWidth = GetExtraWindowWidth(hWnd);
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

                        // 70 % of program size
        int dlgWidth = (parentWidth * 70 / 100) + extraWidth + dlgPaddingX;
                        // 80 % of program size
        int dlgHeight = (parentHeight * 80 / 100) + extraHeight + dlgPaddingX;

        dlgWidth = minDlgWidth > 0 && dlgWidth < minDlgWidth ? minDlgWidth : dlgWidth;
        dlgHeight = minDlgHeight > 0 && dlgHeight < minDlgHeight ? minDlgHeight : dlgHeight;


        int top = (screenHeight - dlgHeight) / 2, left = (screenWidth - dlgWidth) / 2;

        SetWindowPos(GetHWND(), NULL, left, top, dlgWidth, dlgHeight, NULL);


        OnResize();

        UpdateWindow(hWnd);


        return (INT_PTR)TRUE;
    }

    case WM_CTLCOLORDLG:
    {
        return (INT_PTR)GetStockObject(WHITE_BRUSH);
    }

    case WM_DESTROY:
    {
        SetPagesFromTo();
        break;
    }

    case WM_COMMAND:
    {
        UINT wId = LOWORD(wParam);
        if (wId == IDCANCEL)
        {
            EndDialog(hWnd, wId);
            return (INT_PTR)TRUE;
        }
        else
        {
            if (OnComman(wParam)) return true;
        }

        break;
    }
    }

    return (INT_PTR)FALSE;
}

void PrinterDlg::SetShowContentOffset(bool v, std::wstring offsetSection)
{
    showContentOffsets = v;
    contentOffsetsConfigSection = offsetSection;
}