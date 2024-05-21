#include "MemberList.h"

#define SEARCH_TIMER_ID		122


MemberList::MemberList(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	sortTabsIndexes = { L"Active", L"Inactive", L"Deceased", L"Unknown"};
	currentMemberViewType = sortTabsIndexes[0];
	currentPage = L"1";
	letters = { L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z" };

	showLimits = { L"25", L"50", L"100" };

	editFamily = NULL;
	currentSortByLetter = -1;
	table = NULL;
}

MemberList::~MemberList()
{
	RemoveWindowSubclass(hWnd, membersProc, IDC_STATIC);

	delete table;
	delete memSortTabs;
}

void MemberList::OnTableSort(int columnId, int type, DWORD_PTR context)
{
	TableCreater* lpData = (TableCreater*)context;

	try
	{
		TableColumns colInfo = lpData->GetColumnInfo(columnId);
		lpData->SetSortBy(colInfo.dbName);
		lpData->SetSortType(type == 2 ? "ASC" : "DESC");
		UpdateMembersTable();
	}

	catch (std::string s) {}

	catch (std::exception e) {}

	// 1= DESC, 2 = ASC
}

void MemberList::AddControls()
{
	CreateStatic("ssearchMember", 10, 30, 120, 20, WS_VISIBLE | WS_CHILD, L"Search member:");
	CreateEdit("searchMember", 10, 60, 300, 30, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL);
	SetLabel("searchMember", L"Search member...");

	CreateStatic("ssearchBy", 10, 100, 100, 20, WS_VISIBLE | WS_CHILD, L"Search by:");

	CreateRadioBtn("searchByLastName", 90, 100, 150, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Last Name");
	CreateRadioBtn("searchByFirstName", 230, 100, 150, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"First Name");
	CreateRadioBtn("searchByPhone", 380, 100, 100, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Phone");
	CreateRadioGroup("sortGroup",
		{
			"searchByLastName", "searchByFirstName", "searchByPhone" }, "searchByLastName");

	CreateStatic("sshowLimit", 115, 93, 50, 20, WS_VISIBLE | WS_CHILD | ES_RIGHT, L"Show: ", WS_STICK_RIGHT);

	HWND showLimitH = CreateCombobox("showLimit", 10, 90, 100, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED, L"", WS_STICK_RIGHT);

	for (int i = 0; i < showLimits.size(); i++)
	{
		SendMessage(showLimitH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)showLimits[i].c_str());
	}

	SendMessage(showLimitH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	CreateBtn("deleteMember", 10, 40, 170, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Delete Member", WS_STICK_RIGHT);
	SetColor("deleteMember", APP_COLORS::RED);

	CreateBtn("editMember", 200, 40, 150, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Edit Member", WS_STICK_RIGHT);
	CreateBtn("exportMem", 360, 40, 30, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"", WS_STICK_RIGHT);

	SetButtonIcon("deleteMember", IDB_DELETEMEMBER);
	SetButtonIcon("editMember", IDB_EDITMEMBER);
	SetButtonIcon("exportMem", IDB_EXPORTW);

	int letterMarginX = 10;
	for (size_t i = 0; i < letters.size(); i++)
	{
		int lmx = letterMarginX + ((int)i * (10 + 20));
		std::string id = "ls_" + std::to_string(i);
		HWND item = CreateBtn(id.c_str(), lmx, 140, 20, 20, WS_VISIBLE | WS_CHILD, letters[i].c_str());
		SetStyle(id, BTN_STYLE_EMPTY_SQR);
		lettersIds[GetControlId(id)] = LetterSort({ (int)i, id, item });
	}

	CreateLine(10, 130, -10, 1, "tL", APP_COLORS::GRAY_BORDER);

	CreateTable();

	SetWindowSubclass(hWnd, membersProc, IDC_STATIC, (DWORD_PTR)this);
	SetWindowSubclass(GetControlHWND("memberTable"), membersProc, GetControlId("memberTable"), (DWORD_PTR)this);


	Tabs tabs;
	for (size_t i = 0; i < sortTabsIndexes.size(); i++)
	{
		tabs.AddTab((int)i, sortTabsIndexes[i]);
	}

	memSortTabs = new TabControl(hWnd, tabs, 0, TCS_FIXEDWIDTH, 10, 190, -10, 30, false);
	memSortTabs->SetBgColor(APP_COLORS::WHITE);
	SendMessage(memSortTabs->GetTabsHWND(), TCM_SETITEMSIZE, 0, MAKELPARAM(100, 30));

	/*
	editFamily = new EditFamily(hWnd, hInst);
	editFamily->init("member", 826770);

	// Dialog....

	//When dialog closes
	delete editFamily;
	*/
}


void MemberList::SwitchMemType(int type)
{
	currentMemberViewType = sortTabsIndexes[type];
	OnSearchMember();
}

void MemberList::CreateTable()
{
	table = new TableCreater(hWnd, hInst, this);

	table->Create("memberTable", 10, 220, -10, -50);

	table->SetRefClass(this);

	table->CreateColumns({
		{ L"ID", 10, "id" },
		{ L"Last name", 20, "lastName" },
		{ L"First name", 20, "firstName" },
		{ L"Type", 10, "type" },
		{ L"Mobile Phone", 20, "mobilePhone" },
		{ L"Gender", 10, "gender" },
		{ L"Birth date", 10, "birthDate" }
		}
	);

	table->InsertColumns();

	TableCData* cData = new TableCData(hWnd, hInst);
	cData->currentSortBy = "lastName";
	cData->currentSortType = "DESC";
	table->SetTableCData(cData);

	UpdateMembersTable([&]()
		{
			UpdateWindow(hWnd);
		}
	);

}

void MemberList::OnExport()
{
	/*
	DBResults data = table->GetSqlData(true, true);
	Exporting ex(hWnd);
	ex.ExportData(data, L"Members", ex.GetSelectedSeparator());
	data = table->GetSqlData(true, true);
	*/
}

void MemberList::UpdateMembersTable(std::function<void(void)> onTableUpdateOverCallback)
{
	std::wstring showLimits = GetComboBoxValue("showLimit");
	table->SetSearchShowLimit(showLimits);

	table->UpdateTable("getMembersList",
		{
			{ "type", currentMemberViewType },
			{ "limit", GetComboBoxValue("showLimit") },
			{ "searchStr", GetWindowTextStr(GetControlHWND("searchMember")) },
			{ "searchBy", currentSearchBy },
			{ "currentPage", currentPage },
			{ "sortByLetter", currentSortByLetter != -1 ? letters[currentSortByLetter] : L""},
			{ "orderByField", L"family" },
			{ "orderByType", L"ASC" }
		},
		onTableUpdateOverCallback
	);
};

void MemberList::OnSearchMember()
{
	ChangeCurrentPage(1);
	UpdateMembersTable([&]()
		{
			SetFocus(GetControlHWND("searchMember"));
		}
	);
}


void MemberList::ChangeCurrentPage(int page)
{
	currentPage = std::to_wstring(page);
	table->SetCurrentPage(page);
}

void MemberList::OnSortByLetter(LetterSort item)
{
	for (auto it : lettersIds)
	{
		SetStatus(it.second.id, false);
	}


	if (currentSortByLetter == item.letterIndex)
	{
		currentSortByLetter = -1;
	}
	else
	{
		SetStatus(item.id, true);
		currentSortByLetter = item.letterIndex;
	}

	OnSearchMember();
}

bool MemberList::HandleEditMember()
{
	int memberId = table->GetSelectedRowId();

	if (memberId <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Select which member you want to edit.",
			MD_OK, MD_ERR).OpenDlg();
		return false;
	}

	editFamily = new EditFamily(hWnd, hInst);
	editFamily->Init("member", memberId);

	// Dialog....

	//When dialog closes
	delete editFamily;

	UpdateMembersTable();

	return true;
}

bool MemberList::OnDeleteMember()
{
	int memberId = table->GetSelectedRowId();

	if (memberId <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Select which member you want to delete.",
			MD_OK, MD_ERR).OpenDlg();
		return false;
	}

	EditFamily ef(NULL, NULL);
	/*
	bool result = ef.OnDeleteMember(memberId);

	if (result == true)
	{
		UpdateMembersTable();
	}
	return result;
	*/
	return true;

}

bool MemberList::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (wId == GetControlId("searchByLastName"))
	{
		currentSearchBy = L"lastName";
		OnSearchMember();
		return true;
	}
	else if (wId == GetControlId("searchByFirstName"))
	{
		currentSearchBy = L"firstName";
		OnSearchMember();
		return true;
	}
	else if (wId == GetControlId("searchByPhone"))
	{
		currentSearchBy = L"mobilePhone";
		OnSearchMember();
		return true;
	}
	else if (wId == ID_EDIT_TI)
	{
		HandleEditMember();
		return true;
	}
	else if (wId == ID_DELETE_TI)
	{
		OnDeleteMember();
		return true;
	}

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("deleteMember"))
		{
			OnDeleteMember();
			return true;
		}
		else if (wId == GetControlId("editMember"))
		{
			HandleEditMember();
			return true;
		}
		else if (wId == GetControlId("exportMem"))
		{
			OnExport();
			return true;
		}
		else if (lettersIds.find(wId) != lettersIds.end())
		{
			OnSortByLetter(lettersIds[wId]);
			return true;
		}

		break;
	}

	case CBN_SELCHANGE:
	{
		if (wId == GetControlId("showLimit"))
		{
			table->SetSearchShowLimit(GetComboBoxValue("showLimit"));
			OnSearchMember();
			return true;
		}

		break;
	}

	case EN_CHANGE:
	{
		if (wId == GetControlId("searchMember"))
		{
			SetTimer(hWnd, SEARCH_TIMER_ID, 500, nullptr);
			return true;
		}

		break;
	}
	}

	return false;
}

LRESULT CALLBACK MemberList::membersProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	MemberList* lpData = (MemberList*)dwRefData;

	switch (uMsg)
	{
	case WM_TIMER:
	{
		if (wParam == SEARCH_TIMER_ID)
		{
			lpData->OnSearchMember();
		}
		KillTimer(hWnd, wParam);

		break;
	}
	case WM_KEYDOWN:
	{
		if (uIdSubclass == lpData->GetControlId("memberTable"))
		{
			if (wParam == VK_DELETE)
			{
				lpData->OnDeleteMember();
				return true;
			}
		}

		break;
	}

	case WM_LBUTTONDBLCLK:
	{
		if (uIdSubclass == lpData->GetControlId("memberTable"))
		{
			lpData->HandleEditMember();
			return true;
		}

		break;
	}

	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_RCLICK:
		{
			int index = 0;
			LV_ITEM lvi = { 0 };

			index = ListView_GetNextItem(lpData->table->GetHWND(),
				-1, LVNI_SELECTED);
			if (index < 0) return true;

			POINT cursor;
			GetCursorPos(&cursor);
			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_TABLEMENU));
			hMenu = GetSubMenu(hMenu, 0);
			TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, lpData->hWnd, NULL);
			return true;
		}

		case TCN_SELCHANGE:
		{
			TabControl* cDataTabs = lpData->table->GetTableCData()->spBtns;

			int formId = static_cast<int> (((LPNMHDR)lParam)->idFrom);
			if (((LPNMHDR)lParam)->idFrom == cDataTabs->GetControlId("tab"))
			{
				TCITEM item = { 0 };

				item.mask = TCIF_PARAM;
				int iCurTab = TabCtrl_GetCurSel(cDataTabs->GetTabsHWND());
				TabCtrl_GetItem(cDataTabs->GetTabsHWND(), iCurTab, &item);

				lpData->ChangeCurrentPage((int)item.lParam);
				lpData->UpdateMembersTable();

				return true;
			}
			else if (formId == lpData->memSortTabs->GetControlId("tab"))
			{
				TCITEM item = { 0 };

				item.mask = TCIF_PARAM;
				int iCurTab = TabCtrl_GetCurSel(lpData->memSortTabs->GetTabsHWND());
				TabCtrl_GetItem(lpData->memSortTabs->GetTabsHWND(), iCurTab, &item);
				int id = static_cast<int> (item.lParam);

				lpData->SwitchMemType(id);
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