#include "CertCommon.h"

CertCommon::CertCommon(HWND hWnd, HINSTANCE hInst, std::wstring certType) : PageCommon(hWnd, hInst), certificateTypeStr(certType)
{
	selectionConfigName = L"SacramentsReports";
}

CertCommon::~CertCommon()
{
}

std::wstring CertCommon::GetCertificateTypeStr()
{
	return certificateTypeStr;
}

void CertCommon::SaveToTempData()
{
}

void CertCommon::FillFromTempData()
{
}

void CertCommon::DrawCert(PrinterDrawer* printer)
{

}

void CertCommon::DrawFancyCert(PrinterDrawer* printer)
{

}

void CertCommon::DrawGodParentCert(PrinterDrawer* printer)
{

}


void CertCommon::ValidateFields()
{

}

void CertCommon::ClearAllFields()
{

}
void CertCommon::SaveToHistory(std::function<void(void)> callback)
{

}
void CertCommon::FillFromHistory(int id)
{

}

std::vector<std::pair<std::wstring, std::string>> CertCommon::GetFillTableFields()
{
	return {};
}
void CertCommon::LinkTempData(CertPrintTempData* tempDataPtr)
{
	this->tempDataPtr = tempDataPtr;
}

void CertCommon::OnPrinted(Printer* printerPtr)
{
	printerPtr->SavePrintSelections(selectionConfigName);
}