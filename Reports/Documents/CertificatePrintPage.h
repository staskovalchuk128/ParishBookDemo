/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CertCommon.h"

enum CertPages
{
	PRINT_BAPTIZM_CERT, PRINT_MAR_CERT, PRINT_CONF_CERT, PRINT_FIRST_COM_CERT, PRINT_FULL_COM_CERT
};



class CertificatePrintPage : public PageCommon
{
public:
	CertificatePrintPage(HWND hWnd, HINSTANCE hInst);
	~CertificatePrintPage();

	void AddControls();
	void SwitchPage(int pageType);
	bool OnCommand(WPARAM wParam);
private:
	HWND childHwnd;
	int currentPageType;
	CertCommon* currentPage;
	std::map<int, std::wstring> menuItems;

	CertPrintTempData tempCertData;
};

