#include "core.h"
#include <lm.h>
#include "AppTabsManager.hpp"

#include "PageIds.hpp"

/*OFFERING*/

#include "Offering/Batches.h"
#include "Offering/Funds.h"

/*FAMILY DIRECT*/

#include "FamilyDirect/FamilyList.h"
#include "FamilyDirect/MemberList.h"
#include "FamilyDirect/FamilyEnvAssigner.h"
#include "FamilyDirect/FamDeactivator.h"

/*REPORTS*/
#include "Reports/FamilyReports.h"
#include "Reports/Documents/PrintNameLabels.h"
#include "Reports/Documents/BabtismCertificate.h"
#include "Reports/Documents/MarriageCertificate.h"
#include "Reports/Documents/ConfirmationCertificate.h"
#include "Reports/Documents/FirstCommunionCertificate.h"
#include "Reports/Documents/FullCommunionCertificate.h"
#include "Reports/Documents/CertificateEnvelope.h"
#include "Reports/Documents/CertificatePrintPage.h"

/*CEMETERY*/
#include "Cemetery/CemeteryPage.h"
#include "Cemetery/CemeteryEditor.h"

/*RECYCLE*/
#include "Recycle/Recycle.h"

/*SETTINGS*/
#include "Settings/OrgSettings.h"
#include "Settings/UsabilitySettings.h"
#include "Settings/PrivacySettings.h"

Core* Core::_this = NULL;

Core::Core(HWND hWnd, HINSTANCE inst) : Controls(hWnd)
{
	currentPage = 0;
	hWndPage = NULL;
	tc = NULL;
	_this = this;
	this->hWnd = hWnd;
	this->hInst = inst;
	currentPageType = MAIN_MENU_IDS::OFFERING;
	currentPageId = PAGE_IDS::BATCHES;

	AppTabsManager::InitTabs();	// Create app tabs
}

Core::~Core()
{
	delete tc;
	delete currentPage;
}

void Core::AddControls()
{
	SetWindowSubclass(hWnd, CoreProces, IDC_STATIC, (DWORD_PTR)this);

	Tabs currentTabs = AppTabsManager::GetTabs(currentPageType);

	tc = new TabControl(hWnd, currentTabs, 0);
	tc->SetFixedSize(200, 30);
	hWndPage = tc->GetBodyHWND();
}

void Core::SwitchTab(int id)
{
	Display(id);
}

PageCommon* Core::InitPage(int pageId)
{
	switch (pageId)
	{
	/*OFFERING*/
	case PAGE_IDS::BATCHES:
		return new Batches(hWndPage, hInst);
	case PAGE_IDS::FUNDS:
		return new Funds(hWndPage, hInst);

	/*FAMILY DIRECT*/
	case PAGE_IDS::FAMILIES:
		return new FamilyList(hWndPage, hInst);
	case PAGE_IDS::MEMBERS:
		return new MemberList(hWndPage, hInst);
	case PAGE_IDS::ENV_NUM_ASSIGNER:
		return new FamilyEnvAssigner(hWndPage, hInst);
	case PAGE_IDS::FAM_DEACTIVATOR:
		return new FamDeactivator(hWndPage, hInst);

	/*REPORTS*/
	case PAGE_IDS::GENERAL_REPORTS:
		return new FamilyReports(hWndPage, hInst);
	case PAGE_IDS::FAMILY_LABELS:
		return new PrintNameLabels(hWndPage, hInst);
	case PAGE_IDS::SACRAMENT_CERTIFICATES:
		return new CertificatePrintPage(hWndPage, hInst);
	case PAGE_IDS::CERTIFICATE_ENVELOPE:
		return new CertificateEnvelope(hWndPage, hInst);
		
	/*RECYCLE*/
	case PAGE_IDS::RECYCLE_PAGE:
		return new Recycle(hWndPage, hInst);

	/*CEMETERY*/
	case PAGE_IDS::CEMETERY_PAGE:
		return new CemeteryPage(hWndPage, hInst);
	case PAGE_IDS::CEMETERY_EDITOR:
		return new CemeteryEditor(hWndPage, hInst);

	/*SETTINGS*/
	case PAGE_IDS::SETTINGS_ORGANIZATION:
		return new OrgSettings(hWndPage, hInst);
	case PAGE_IDS::SETTINGS_USABILITY:
		return new UsabilitySettings(hWndPage, hInst);
	case PAGE_IDS::SETTINGS_PRIVACY:
		return new PrivacySettings(hWndPage, hInst);

	default:
		return new Batches(hWndPage, hInst);
	}
}

void Core::Display(int pageId)
{
	currentPageId = pageId;

	LockWindowUpdate(hWnd);

	if (currentPage) delete currentPage;

	currentPage = InitPage(pageId);
	currentPage->InitPage();

	try
	{
		currentPage->AddControls();
	}
	catch (std::wstring s)
	{
		MessageBox(NULL, s.c_str(), L"Error", MB_OK);
	}
	catch (std::exception& e)
	{
		MessageBox(NULL, StrToWStr(e.what()).c_str(), L"Error", MB_OK);
	}

	LockWindowUpdate(NULL);

}

void Core::OnSwitchPageType(int pageTypeId, int pageId)
{
	currentPageType = pageTypeId;
	currentPageId = pageId;

	Tabs currentTabs = AppTabsManager::GetTabs(currentPageType);

	tc->DeleteAllTabs();
	tc->AddTabs(currentTabs, 0);

	try
	{
		Display(currentPageId);
	}

	catch (std::wstring s)
	{
		MessageBox(NULL, s.c_str(), L"Error", MB_OK);
	}

	catch (std::exception& e)
	{
		MessageBox(NULL, StrToWStr(e.what()).c_str(), L"Error", MB_OK);
	}
}

void Core::OpenPageDlg(int id)
{
	if (id == MAIN_MENU_IDS::EXPORT)
	{
		Exporting ex(hWnd);
		ex.OpenExportDialog();
	}
}

Core* Core::GetCorePtr()
{
	return _this;
}

HWND Core::GetCoreHWND() 
{
	return hWnd;
}

HWND Core::GetHWNDPage()
{
	return hWndPage;
}

LRESULT CALLBACK Core::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	Core* lpData = (Core*)dwRefData;

	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case SWITCH_TO_TAB:
		{
			int tabId = static_cast<int> (((LPNMHDR)lParam)->idFrom);

			lpData->SwitchTab(tabId);

			return TRUE;
		}

		case TCN_SELCHANGING:
		{
			return FALSE;
		}

		case TCN_SELCHANGE:
		{
			if (((LPNMHDR)lParam)->idFrom == lpData->tc->GetControlId("tab"))
			{
				TCITEM item = { 0 };

				item.mask = TCIF_PARAM;
				int iCurTab = TabCtrl_GetCurSel(lpData->tc->GetTabsHWND());
				TabCtrl_GetItem(lpData->tc->GetTabsHWND(), iCurTab, &item);
				int id = static_cast<int> (item.lParam);

				lpData->SwitchTab(id);
				InvalidateRect(((LPNMHDR)lParam)->hwndFrom, NULL, FALSE);
				return TRUE;
			}

			break;
		}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}