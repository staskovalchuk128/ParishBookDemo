#include "CemeteryPage.h"

#include "..//PBConnection.h"

CemeteryPage::CemeteryPage(HWND hWnd, HINSTANCE hInst)
	: PageCommon(hWnd, hInst)
{
	cellInfoControls = NULL;
	currentGraveId = -1;
	drawer = NULL;
	pageState = PageState::GRAVE_NOT_SELECTED;
	currentSearchBy = L"lastName";

	statuses = {
		L"Free",
		L"Paid",
		L"Reserved"
	};
}

CemeteryPage::~CemeteryPage()
{
	delete cellInfoControls;
}

void CemeteryPage::AddControls()
{


	CreateStatic("ssearchSpot", 10, 10, 200, 20, WS_VISIBLE | WS_CHILD, L"Search:");

	CreateEdit("searchSpot", 10, 40, 300, 30, WS_VISIBLE | WS_CHILD);
	SetLabel("searchSpot", L"Search by Last Name ...");

	CreateStatic("ssearchBy", 10, 80, 100, 20, WS_VISIBLE | WS_CHILD, L"Search by:");

	CreateRadioBtn("searchByLastName", 90, 80, 130, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Last Name");
	CreateRadioBtn("searchByRowCol", 210, 80, 130, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Row-Col");
	CreateRadioGroup("sortGroup", 
		{
			"searchByLastName", "searchByRowCol" 
		}
	, "searchByLastName");


	HWND cemBody = CreateStatic("cemBody", 10, 120, -500, -10, WS_VISIBLE | WS_CHILD | SS_NOTIFY | WS_CLIPCHILDREN, L"");
	
	HWND infoHwnd = CreateStatic("cemCellInfo", 10, 10, 480, -10, WS_VISIBLE | WS_CHILD | SS_NOTIFY | WS_CLIPCHILDREN, L"", WS_STICK_RIGHT);
	cellInfoControls = new Controls(infoHwnd);
	SetWindowSubclass(infoHwnd, PageMainProc, IDC_STATIC, (DWORD_PTR)this);


	CreateLine(490, 10, 1, -10, "sepLine1", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

	AddGraveCotrols();

	InitDrawer(cemBody, CemeteryDrawerMode::SIMPLE);

	/*Info about colors of cells*/

	cellInfoControls->CreateStatic("sInfos", 10, 70, -1, 20, WS_VISIBLE | WS_CHILD, L"Status colors", WS_STICK_BOTTOM);

	cellInfoControls->CreateLine(10, 60, -1, 2, "sepLine1", APP_COLORS::GRAY_BORDER, WS_STICK_BOTTOM);

	cellInfoControls->CreateStatic("greenStatus", 10, 30, -1, 20, WS_VISIBLE | WS_CHILD, L"- Paid status", WS_STICK_BOTTOM);
	cellInfoControls->SetColor("greenStatus", APP_COLORS::GREEN);

	cellInfoControls->CreateStatic("orangeStatus", 10, 10, -1, 20, WS_VISIBLE | WS_CHILD, L"- Reserved status", WS_STICK_BOTTOM);
	cellInfoControls->SetColor("orangeStatus", APP_COLORS::ORANGE);

	//CreateLine(10, -70, -10, 1, "sepLine1", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);


}

void CemeteryPage::AddGraveCotrols()
{
	cellInfoControls->Clear();
	LockWindowUpdate(cellInfoControls->GetHWND());
	if (pageState == PageState::EDIT_GRAVE)
	{
		CreateGraveEditControls();
	}
	else if (pageState == PageState::VIEW_GRAVE_INFO)
	{
		CreateGraveInfoControls();
	}
	else
	{
		CreateGraveNotSelectedControls();
	}
	LockWindowUpdate(NULL);
}

void CemeteryPage::CreateGraveInfoControls()
{
	PBConnection::Request("getGraveInfo",
		{
			{ "graveId", std::to_wstring(currentGraveId) }
		},
		[&](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			std::wstring firstName = L"", lastName = L"", middleName = L"", dateOfBirth = L"", dateOfDeath = L"", comment = L"",
				status = L"", amountPaid = L"", totalAmount = L"";
			if (result.size() > 0)
			{
				firstName = result[0]["firstName"];
				lastName = result[0]["lastName"];
				middleName = result[0]["middleName"];
				dateOfBirth = result[0]["dateOfBirth"];
				dateOfDeath = result[0]["dateOfDeath"];
				comment = result[0]["comment"];
				status = result[0]["status"];
				amountPaid = GetFloatFormatStr(result[0]["amountPaid"]);
				totalAmount = GetFloatFormatStr(result[0]["totalAmount"]);
			}

			dateOfBirth = dateOfBirth.size() == 0 ? L"Unknown" : dateOfBirth;
			dateOfDeath = dateOfDeath.size() == 0 ? L"Unknown" : dateOfDeath;

			cellInfoControls->CreateStatic("cemCellInfoText", 10, 10, -1, 30, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_CLIPCHILDREN, L"Grave Information");
			cellInfoControls->SetFontSize("cemCellInfoText", 22);

			cellInfoControls->CreateLine(10, 50, -10, 1, "sepLine2", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

			int itemsOffsetY = 30;
			int itemX = 10, itemY = 60;

			cellInfoControls->CreateStatic("sgraveStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, std::wstring(L"STATUS: " + status).c_str());

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, std::wstring(L"Name: " + lastName + L" " + firstName + L" " + middleName).c_str());

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, std::wstring(L"Date of birth: " + dateOfBirth).c_str());

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, std::wstring(L"Date of desease: " + dateOfDeath).c_str());

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, std::wstring(L"Comment: " + comment).c_str());

			if (status == L"Reserved" || status == L"Paid")
			{
				itemY += itemsOffsetY;
				cellInfoControls->CreateLine(10, itemY, -10, 1, "sepLine3", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

				if (status == L"Paid")
				{
					itemY += 15;
					cellInfoControls->CreateStatic("sPaidStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, std::wstring(L"Paid: $" + amountPaid + L" out of $" + totalAmount).c_str());
					cellInfoControls->SetColor("sPaidStatus", APP_COLORS::GREEN);
				}
				else
				{
					float remainPay = GetFloat(totalAmount) - GetFloat(amountPaid);

					itemY += 15;

					if (GetFloat(totalAmount) > 0 || GetFloat(amountPaid) > 0)
					{
						cellInfoControls->CreateStatic("sPaidStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, std::wstring(L"Paid: $" + amountPaid + L" out of $" + totalAmount).c_str());

						itemY += itemsOffsetY;
						cellInfoControls->CreateStatic("sPaidRemain", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, std::wstring(L"Remainig payment is: $" + GetFloatFormatStr(std::to_wstring(remainPay))).c_str());
						cellInfoControls->SetColor("sPaidRemain", APP_COLORS::RED);
					}
					else
					{
						cellInfoControls->CreateStatic("sPaidStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, L"No payment data");
					}

				}
			}

			itemY += itemsOffsetY;
			cellInfoControls->CreateLine(10, itemY, -10, 1, "sepLine4", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

			itemY += itemsOffsetY;
			cellInfoControls->CreateBtn("btnEditGrave", itemX, itemY, 150, 30, WS_VISIBLE | WS_CHILD, L"Edit information", WS_STICK_RIGHT);

		}
	);
}
void CemeteryPage::CreateGraveEditControls()
{

	PBConnection::Request("getGraveInfo",
		{
			{ "graveId", std::to_wstring(currentGraveId) }
		},
		[&](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			std::wstring firstName = L"", lastName = L"", middleName = L"", dateOfBirth = L"", dateOfDeath = L"", comment = L"", status = L"", totalAmount = L"", amountPaid = L"";

			if (result.size() > 0)
			{
				firstName = result[0]["firstName"];
				lastName = result[0]["lastName"];
				middleName = result[0]["middleName"];
				dateOfBirth = result[0]["dateOfBirth"];
				dateOfDeath = result[0]["dateOfDeath"];
				comment = result[0]["comment"];
				status = result[0]["status"];
				totalAmount = GetFloatFormatStr(result[0]["totalAmount"]);
				amountPaid = GetFloatFormatStr(result[0]["amountPaid"]);
			}

			dateOfBirth = dateOfBirth.size() == 0 ? L"" : dateOfBirth;
			dateOfDeath = dateOfDeath.size() == 0 ? L"" : dateOfDeath;

			cellInfoControls->CreateStatic("cemCellInfoText", 10, 10, -1, 30, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_CLIPCHILDREN, L"Editing Grave Information");
			cellInfoControls->SetFontSize("cemCellInfoText", 22);

			cellInfoControls->CreateLine(10, 50, -10, 1, "sepLine2", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

			int itemsOffsetY = 50;
			int itemX = 10, itemY = 60;
			int lablesOffsetY = 10;
			int lableWidth = 130;
			int editWidth = 300;

			cellInfoControls->CreateStatic("sgraveStatus", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"STATUS:");
			HWND hStatus = cellInfoControls->CreateCombobox("graveStatus", 10, itemY, editWidth, 30, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED, L"", WS_STICK_RIGHT);

			int selectedStatus = 0;
			for (int i = 0; i < statuses.size(); i++)
			{
				int itemId = (int)SendMessageW(hStatus, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)statuses[i].c_str());
				if (statuses[i].c_str() == status) selectedStatus = itemId;
			}

			SendMessageW(hStatus, CB_SETCURSEL, (WPARAM)selectedStatus, (LPARAM)0);



			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveLastName", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"Last Name");
			cellInfoControls->CreateEdit("graveLastName", 10, itemY, editWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, lastName.c_str(), WS_STICK_RIGHT);

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveFirstName", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"First Name");
			cellInfoControls->CreateEdit("graveFirstName", 10, itemY, editWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, firstName.c_str(), WS_STICK_RIGHT);

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveMiddleName", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"Middle Name");
			cellInfoControls->CreateEdit("graveMiddleName", 10, itemY, editWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, middleName.c_str(), WS_STICK_RIGHT);

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveDateOfBirth", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"Date of birth");
			cellInfoControls->CreateEdit("graveDateOfBirth", 10, itemY, editWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, dateOfBirth.c_str(), WS_STICK_RIGHT);

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveDateOfDeath", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"Date of desease");
			cellInfoControls->CreateEdit("graveDateOfDeath", 10, itemY, editWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, dateOfDeath.c_str(), WS_STICK_RIGHT);

			cellInfoControls->SetEditMask("graveDateOfBirth", ES_DATE_MASK);
			cellInfoControls->SetEditMask("graveDateOfDeath", ES_DATE_MASK);

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveComment", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"Comment");
			cellInfoControls->CreateEdit("graveComment", 10, itemY, editWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, comment.c_str(), WS_STICK_RIGHT);


			itemY += itemsOffsetY;
			cellInfoControls->CreateLine(10, itemY, -10, 1, "sepLine3", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

			itemY += 20;
			cellInfoControls->CreateStatic("sgraveTotalAmount", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"Total amount");
			cellInfoControls->CreateEdit("graveTotalAmount", 10, itemY, editWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, totalAmount.c_str(), WS_STICK_RIGHT);

			itemY += itemsOffsetY;
			cellInfoControls->CreateStatic("sgraveAmountPaid", itemX, itemY + lablesOffsetY, lableWidth, 30, WS_VISIBLE | WS_CHILD, L"Amount paid");
			cellInfoControls->CreateEdit("graveAmountPaid", 10, itemY, editWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, amountPaid.c_str(), WS_STICK_RIGHT);

			itemY += itemsOffsetY;
			cellInfoControls->CreateLine(10, itemY, -10, 1, "sepLine4", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

			itemY += itemsOffsetY;
			cellInfoControls->CreateBtn("btnCloseGrave", itemX, itemY, 150, 30, WS_VISIBLE | WS_CHILD, L"Close");
			cellInfoControls->SetColor("btnSaveAndCloseGrave", APP_COLORS::RED);
			cellInfoControls->CreateBtn("btnSaveGrave", itemX, itemY, 150, 30, WS_VISIBLE | WS_CHILD, L"Save information", WS_STICK_RIGHT);

		}
	);
}
void CemeteryPage::CreateGraveNotSelectedControls()
{
	cellInfoControls->CreateStatic("cemCellInfoText", 10, 10, -1, 30, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_CLIPCHILDREN, L"Nothing Selected");
	cellInfoControls->SetFontSize("cemCellInfoText", 22);

	cellInfoControls->CreateLine(10, 50, -10, 1, "sepLine2", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

	int itemsOffsetY = 30;
	int itemX = 10, itemY = 60;

	cellInfoControls->CreateStatic("sgraveStatus", itemX, itemY, -1, 30, WS_VISIBLE | WS_CHILD, L"Select grave on the graph to see information");
}

void CemeteryPage::SwitchState(PageState newState)
{
	pageState = newState;
	AddGraveCotrols();
}

void CemeteryPage::ShowGraveInfo(int id)
{
	currentGraveId = id;
	SwitchState(PageState::VIEW_GRAVE_INFO);
}

void CemeteryPage::OnEditGrave(int id)
{
	currentGraveId = id;
	SwitchState(PageState::EDIT_GRAVE);
}

void CemeteryPage::OnGraveSelected(int graveId)
{
	ShowGraveInfo(graveId);
}

void CemeteryPage::OnSaveGraveInfo(bool closeAfterSave)
{

	std::wstring status = GetWindowTextStr(cellInfoControls->GetControlHWND("graveStatus")), lastName = GetWindowTextStr(cellInfoControls->GetControlHWND("graveLastName")),
		firstName = GetWindowTextStr(cellInfoControls->GetControlHWND("graveFirstName")), middleName = GetWindowTextStr(cellInfoControls->GetControlHWND("graveMiddleName")),
		dateOfBirth = GetWindowTextStr(cellInfoControls->GetControlHWND("graveDateOfBirth")), dateOfDeath = GetWindowTextStr(cellInfoControls->GetControlHWND("graveDateOfDeath")),
		comment = GetWindowTextStr(cellInfoControls->GetControlHWND("graveComment")), totalAmount = GetWindowTextStr(cellInfoControls->GetControlHWND("graveTotalAmount")),
		amountPaid = GetWindowTextStr(cellInfoControls->GetControlHWND("graveAmountPaid"));
	
	PBConnection::Request("saveGraveInfo",
		{
			{ "graveId", std::to_wstring(currentGraveId) },
			{ "graveStatus", status },
			{ "graveLastName", lastName },
			{ "graveFirstName", firstName },
			{ "graveMiddleName", middleName },
			{ "graveDateOfBirth", dateOfBirth },
			{ "graveDateOfDeath", dateOfDeath },
			{ "graveComment", comment },
			{ "graveTotalAmount", totalAmount },
			{ "graveAmountPaid", amountPaid }
		},
		[&, closeAfterSave, firstName, lastName, middleName, status](PBResponse res)
		{

			MessageDlg(hWnd, L"Success",
				L"Information updated.",
				MD_OK, MD_SUCCESS).OpenDlg();

			//Update information about the cell in cemetery drawer
			CemeteryDrawer::Cell* cellPtr = drawer->GetCellById(currentGraveId);
			cellPtr->graveInfo.firstName = firstName;
			cellPtr->graveInfo.lastName = lastName;
			cellPtr->graveInfo.middleName = middleName;
			cellPtr->graveInfo.status = status;

			if (closeAfterSave)
			{
				SwitchState(PageState::VIEW_GRAVE_INFO);
			}
		}
	);

}

bool CemeteryPage::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (wId == GetControlId("searchByLastName"))
	{
		currentSearchBy = L"lastName";
		SetLabel("searchSpot", L"Search by Last Name ...");
		return true;
	}
	else if (wId == GetControlId("searchByRowCol"))
	{
		currentSearchBy = L"rowcol";
		SetLabel("searchSpot", L"Row, Column");
		return true;
	}

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		break;
	}
	case EN_CHANGE:
	{
		if (wId == GetControlId("searchSpot"))
		{
			std::wstring searchStr = GetWindowTextStr(GetControlHWND("searchSpot"));
			drawer->Search(searchStr, currentSearchBy);
			return true;
		}

		break;
	}
	}
	return false;


}



LRESULT CALLBACK CemeteryPage::PageMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CemeteryPage* lpData = (CemeteryPage*)dwRefData;
	HDC hdc = (HDC)wParam;
	
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);

		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			if (wId == lpData->cellInfoControls->GetControlId("btnEditGrave"))
			{
				lpData->SwitchState(PageState::EDIT_GRAVE);
				return true;
			}
			else if (wId == lpData->cellInfoControls->GetControlId("btnCloseGrave"))
			{
				lpData->SwitchState(PageState::VIEW_GRAVE_INFO);
				return true;
			}
			else if (wId == lpData->cellInfoControls->GetControlId("btnSaveGrave"))
			{
				lpData->OnSaveGraveInfo(false);
				return true;
			}

			break;
		}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}