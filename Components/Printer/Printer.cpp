#include "Printer.h"
#include "..//..//Common/MessageDlg.h"

Printer::Printer(HWND h) : hWnd(h), PrinterDrawer()
{
    awaitForDataToLoad = true;
    loadPrintingSelectionName = L"";
    onPrintedCallback = nullptr;
    orientation = DMORIENT_PORTRAIT;
    paperSize = DMPAPER_LETTER;
    
    devNames = { 0 };
    deviceMode = { 0 };
    hdc = NULL;
    docInfo = { sizeof(DOCINFO), TEXT("Document") };

};

Printer::~Printer()
{
    delete printerDlg;
}

bool Printer::SetPrinterDC(void)
{
    printerDlg = new PrinterDlg(this, awaitForDataToLoad, orientation, paperSize);
    if (savedPrintingConfig.length() > 0)
    {
        printerDlg->SetSavedPrintingConfig(savedPrintingConfig);
    }

    printerDlg->SetShowContentOffset(showContentOffsets, contentOffsetsConfigSection);
    int res = (int)printerDlg->OpenDlg(hWnd);
    if (res == IDCANCEL) return false;

    return true;
}


void Printer::CreatePredrawDC()
{
    hdc = printerDlg->GetPrinterDC();
}

void Printer::SetDocName(const wchar_t* docName) 
{
    docInfo = { sizeof(DOCINFO), docName };
}
void Printer::SetOrientation(short orient) 
{
    this->orientation = orient;
};
void Printer::SetPaperSize(short ps) 
{
    this->paperSize = ps;
};
void Printer::SetSavedPrintingConfig(std::wstring savedPrintingConfig)
{
    this->savedPrintingConfig = savedPrintingConfig;
}


bool Printer::InitPriner(std::function<void(PrinterDrawer*)> dCallback, bool awaitForLoadingData)
{
    awaitForDataToLoad = awaitForLoadingData;
    this->drawCallback = dCallback;

    if (!SetPrinterDC()) 
    {
        return false;
    }

    if (!awaitForLoadingData)
    {
        CreatePredrawDC();

        InitDrawing(hdc);
    }

    return true;
}

void Printer::OnFinishedLoadingReport()
{
    CreatePredrawDC();
    InitDrawing(hdc);
    printerDlg->OnFinishedLoadingData();
}

void Printer::DonePredraw() 
{
    DeleteDC(hdc);
    std::wstring lastErr = GetLastErrorAsString();

    printerDlg->RegeneratePrinterDC();
    hdc = printerDlg->GetPrinterDC();
    InitDrawing(hdc);
}


void Printer::Print()
{
    EndPreview();
    ResetPages();
    SetPrintFromToPages(printerDlg->GetPrintFrom(), printerDlg->GetPrintTo());

    StartDrawing(true);

    try
    {
        drawCallback(this);
    }
    catch (std::string res) // stops drawing if page out of nFromPage and nToPage
    {
    }

    std::wstring lastErr = GetLastErrorAsString();

    DonePredraw();

    lastErr = GetLastErrorAsString();

    StartDoc(hdc, &docInfo);

    lastErr = GetLastErrorAsString();
    StartDrawing(false);

    try
    {
        drawCallback(this);
    }
    catch (std::string res) // stops drawing if page out of nFromPage and nToPage
    {
    }

    if (onPrintedCallback != nullptr) onPrintedCallback(this);

    EndPage(hdc);
    EndDoc(hdc);

    DeleteDC(hdc);
}

void Printer::CreatePreviewDC(HDC& hdcPtr)
{
    hdc =  hdcPtr;

    InitDrawing(hdc);
}

void Printer::PrintPreview(bool predraw, int pageNumber)
{
    currentPageNumberForPreview = pageNumber;


    if (!predraw)
    {

    }

    
    StartDrawing(predraw);

    try
    {
        drawCallback(this);
    }
    catch (std::string res) // stops drawing if page out of nFromPage and nToPage
    {
    }

}

void Printer::SetShowContentOffset(bool v, std::wstring offsetSection)
{
    showContentOffsets = v;
    contentOffsetsConfigSection = offsetSection;
}
void Printer::SetPrintedCallback(std::function<void(Printer*)> _callback)
{
    onPrintedCallback = _callback;
}

void Printer::SavePrintSelections(std::wstring configSectionName)
{
    printerDlg->SaveSelectionsToConfig(configSectionName);
}

HDC& Printer::GetPrinterHDC()
{
    return hdc;
}
