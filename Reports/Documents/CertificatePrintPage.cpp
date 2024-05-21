#include "CertificatePrintPage.h"

#include "BabtismCertificate.h"
#include "MarriageCertificate.h"
#include "ConfirmationCertificate.h"
#include "FirstCommunionCertificate.h"
#include "FullCommunionCertificate.h"


#include "CertificatesHistoryDlg.h"

CertificatePrintPage::CertificatePrintPage(HWND hWnd, HINSTANCE hInst) 
	: PageCommon(hWnd, hInst)
{
	tempCertData = {};
	currentPage = NULL;
	currentPageType = PRINT_BAPTIZM_CERT;

	menuItems = {
		{
			PRINT_BAPTIZM_CERT, L"Baptizm certificate"
		},
		{
			PRINT_MAR_CERT, L"Marriage certificate"
		},
		{
			PRINT_CONF_CERT, L"Confirmation Cerificate"
		},
		{
			PRINT_FIRST_COM_CERT, L"First Communion Cerificate"
		},
		{
			PRINT_FULL_COM_CERT, L"Full Communion Cerificate"
		}
	};

}

CertificatePrintPage::~CertificatePrintPage()
{
	delete currentPage;
}

void CertificatePrintPage::AddControls()
{
	CreateStatic("sm", 10, 10, 300, 20, WS_VISIBLE | WS_CHILD, L"Certificate type:");
	HWND ct = CreateCombobox("certType", 10, 40, 300, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL);

	CreateBtn("clearFileds", 10, 40, 140, 30, WS_CHILD | WS_VISIBLE, L"Clear all fields", WS_STICK_RIGHT);
	CreateBtn("saveToHist", 160, 40, 150, 30, WS_CHILD | WS_VISIBLE, L"Save to history", WS_STICK_RIGHT);
	CreateBtn("fillFromHist", 320, 40, 150, 30, WS_CHILD | WS_VISIBLE, L"Fill from history", WS_STICK_RIGHT);
	SetColor("clearFileds", APP_COLORS::RED);

	childHwnd = CreateStatic("sm", 10, 110, -10, -10, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, L"");

	for (auto &it: menuItems)
	{
		SendMessage(ct, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)it.second.c_str());
	}
	SendMessage(ct, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	CreateLine(10, 100, -10, 3, "sepLine", APP_COLORS::GRAY_BORDER);

	SwitchPage(currentPageType);
}

void CertificatePrintPage::SwitchPage(int pageType)
{
	if (currentPage)
	{
		currentPage->SaveToTempData();
		delete currentPage;
		currentPage = NULL;
	}

	LockWindowUpdate(childHwnd);
	

	switch (pageType)
	{
	case PRINT_BAPTIZM_CERT:
		currentPage = new BabtismCertificate(childHwnd, NULL);
		break;
	case PRINT_MAR_CERT:
		currentPage = new MarriageCertificate(childHwnd, NULL);
		break;
	case PRINT_CONF_CERT:
		currentPage = new ConfirmationCertificate(childHwnd, NULL);
		break;
	case PRINT_FIRST_COM_CERT:
		currentPage = new FirstCommunionCertificate(childHwnd, NULL);
		break;
	case PRINT_FULL_COM_CERT:
		currentPage = new FullCommunionCertificate(childHwnd, NULL);
		break;
	default:
		break;
	}

	currentPageType = pageType;
	currentPage->InitPage();
	currentPage->LinkTempData(&tempCertData);
	currentPage->AddControls();
	currentPage->FillFromTempData();

	LockWindowUpdate(NULL);

	UnInitPage();
	InitPage();
}
bool CertificatePrintPage::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);
	
	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("clearFileds"))
		{
			currentPage->ClearAllFields();
			tempCertData = {};
			return true;
		}
		else if (wId == GetControlId("fillFromHist"))
		{
			CertificatesHistoryDlg(hWnd, currentPage).ShowDlg();
			return true;
		}
		else if (wId == GetControlId("saveToHist"))
		{
			currentPage->SaveToHistory([&]()
				{
					MessageDlg(NULL, L"Success",
						L"Certificate saved",
						MD_OK, MD_SUCCESS).OpenDlg();
				}
			);

			return true;
		}
	}
	else if(HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (wId == GetControlId("certType"))
		{
			std::wstring selectedStr = GetWindowTextStr(GetControlHWND("certType"));
			std::map<int, std::wstring>::iterator it = std::find_if(menuItems.begin(), menuItems.end(),
				[selectedStr](const std::pair<int, std::wstring>& item)
				{
					return item.second == selectedStr;
				}
			);
			if (it != menuItems.end())
			{
				SwitchPage((*it).first);
				return true;
			}
		}
	}

	return false;
}