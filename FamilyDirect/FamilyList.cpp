#include "FamilyList.h"
#include <regex>

#define SEARCH_TIMER_ID		122

FamilyList::FamilyList(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	sortTabsIndexes = { L"Active", L"Inactive", L"Deceased", L"Contributor Only", L"Moved" };
	currentFamilyViewType = sortTabsIndexes[0];
	currentPage = L"1";
	showLimits = { L"25", L"50", L"100" };

	letters = { L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z" };

	currentFamilyEditId = 0;
	currentSortByLetter = -1;
	table = NULL;
}

FamilyList::~FamilyList()
{
	RemoveWindowSubclass(hWnd, familiesProc, IDC_STATIC);
	delete table;
	delete famSortTabs;
}

void FamilyList::OnTableSort(int columnId, int type, DWORD_PTR context)
{
	TableCreater* lpData = (TableCreater*)context;

	try
	{
		TableColumns colInfo = lpData->GetColumnInfo(columnId);
		lpData->SetSortBy(colInfo.dbName);
		lpData->SetSortType(type == 2 ? "ASC" : "DESC");
		UpdateFamiliesTable();
	}

	catch (std::string s) {}

	catch (std::exception e) {}

	// 1= DESC, 2 = ASC
}

void FamilyList::CreateTable()
{
	this->table = new TableCreater(hWnd, hInst, this);

	table->Create("familyTable", 10, 220, -10, -50);
	table->SetRefClass(this);

	table->CreateColumns({
		{ L"ID", 10, "id", false },
		{ L"Family", 20, "family" },
		{ L"Env #", 10, "envelope", false },
		{ L"Primary Phone", 20, "phone" },
		{ L"City", 15, "city", true },
		{ L"Postal Code", 15, "zip" },
		{ L"Plus 4", 10, "plus4" } 
	});

	table->InsertColumns();

	TableCData* cData = new TableCData(hWnd, hInst);
	cData->currentSortBy = "family";
	cData->currentSortType = "DESC";
	table->SetTableCData(cData);

	UpdateFamiliesTable([&]()
		{
			//currentFamilyEditId = 206826;
			//ShowFamilyEdit();
			UpdateWindow(hWnd);
		});

}

void FamilyList::AddControls()
{
	InitCommonControls();
	CreateStatic("ssearchFamily", 10, 30, 200, 20, WS_VISIBLE | WS_CHILD, L"Search family:");

	CreateEdit("searchFamily", 10, 60, 300, 30, WS_VISIBLE | WS_CHILD);
	SetLabel("searchFamily", L"Search family...");

	CreateStatic("ssearchBy", 10, 100, 100, 20, WS_VISIBLE | WS_CHILD, L"Search by:");

	CreateRadioBtn("searchByName", 90, 100, 150, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Family Name");
	CreateRadioBtn("searchByEnvelope", 230, 100, 130, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Envelope");
	CreateRadioBtn("searchByPhone", 350, 100, 100, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Phone");
	CreateRadioGroup("sortGroup",
		{
			"searchByName", "searchByEnvelope", "searchByPhone" }, "searchByName");

	CreateStatic("sshowLimit", 115, 93, 50, 20, WS_VISIBLE | WS_CHILD | ES_RIGHT, L"Show: ", WS_STICK_RIGHT);
	HWND showLimitH = CreateCombobox("showLimit", 10, 90, 100, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED, L"", WS_STICK_RIGHT);

	for (int i = 0; i < showLimits.size(); i++)
	{
		SendMessageW(showLimitH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)showLimits[i].c_str());
	}

	SendMessageW(showLimitH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	CreateBtn("deleteFamily", 10, 40, 150, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Delete family", WS_STICK_RIGHT);
	SetColor("deleteFamily", APP_COLORS::RED);
	CreateBtn("editFamily", 180, 40, 130, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Edit family", WS_STICK_RIGHT);
	CreateBtn("addNewFamily", 330, 40, 165, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Add new family", WS_STICK_RIGHT);
	//CreateBtn("exportFam", 505, 40, 30, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"", WS_STICK_RIGHT);

	SetButtonIcon("deleteFamily", IDB_DELETEMEMBER);
	SetButtonIcon("editFamily", IDB_EDITMEMBER);
	SetButtonIcon("addNewFamily", IDB_ADDFAM);
	//SetButtonIcon("exportFam", IDB_EXPORTW);

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
	CreateLine(10, 170, -10, 1, "bL", APP_COLORS::GRAY_BORDER);

	CreateTable();

	SetWindowSubclass(hWnd, familiesProc, IDC_STATIC, (DWORD_PTR)this);
	SetWindowSubclass(GetControlHWND("familyTable"), familiesProc, GetControlId("familyTable"), (DWORD_PTR)this);

	Tabs tabs;
	for (size_t i = 0; i < sortTabsIndexes.size(); i++)
	{
		tabs.AddTab((int)i, sortTabsIndexes[i]);
	}

	famSortTabs = new TabControl(hWnd, tabs, 0, TCS_FIXEDWIDTH, 10, 190, -10, 30, false);
	famSortTabs->SetBgColor(APP_COLORS::WHITE);
	SendMessage(famSortTabs->GetTabsHWND(), TCM_SETITEMSIZE, 0, MAKELPARAM(150, 30));

}

void FamilyList::ChangeCurrentPage(int page)
{
	currentPage = std::to_wstring(page);
	table->SetCurrentPage(page);
}

void FamilyList::SwitchFamType(int type)
{
	currentFamilyViewType = sortTabsIndexes[type];
	OnSearchFamily();
}

void FamilyList::ShowFamilyEdit()
{
	editFamily = new EditFamily(hWnd, hInst);
	editFamily->Init("family", currentFamilyEditId);

	// Dialog....

	//When dialog closes
	delete editFamily;

	UpdateFamiliesTable();
}

void FamilyList::ShowAddFamily()
{
	addNewFamily = new AddNewFamily(hWnd, hInst);
	addNewFamily->ShowDialog();
	// Dialog....
	std::string actionAfter = addNewFamily->GetActionAfter();
	int familyId = addNewFamily->GetCurrentFamilyId();

	delete addNewFamily;

	UpdateFamiliesTable();

	if (actionAfter == "edit")
	{
		currentFamilyEditId = familyId;
		ShowFamilyEdit();
	}
}

void FamilyList::OnSearchFamily()
{
	ChangeCurrentPage(1);
	UpdateFamiliesTable([&]()
		{
			SetFocus(GetControlHWND("searchFamily"));
		}
	);
}

void FamilyList::UpdateFamiliesTable(std::function<void(void)> onTableUpdateOverCallback)
{
	
	std::wstring showLimits = GetComboBoxValue("showLimit");
	table->SetSearchShowLimit(showLimits);

	table->UpdateTable("getFamilyList",
		{
			{ "type", currentFamilyViewType },
			{ "limit", GetComboBoxValue("showLimit") },
			{ "searchStr", GetWindowTextStr(GetControlHWND("searchFamily")) },
			{ "searchBy", currentSearchBy },
			{ "currentPage", currentPage },
			{ "sortByLetter", currentSortByLetter != -1 ? letters[currentSortByLetter] : L""},
			{ "orderByField", L"family" },
			{ "orderByType", L"ASC" }
		},
		onTableUpdateOverCallback
	);
}

bool FamilyList::HandleEditFamily()
{
	int familyId = table->GetSelectedRowId();

	if (familyId <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Select which Family you want to edit",
			MD_OK, MD_ERR).OpenDlg();
		return false;
	}

	currentFamilyEditId = familyId;

	ShowFamilyEdit();

	return true;
}

void FamilyList::OnDeleteFamily()
{
	int familyId = table->GetSelectedRowId();

	if (familyId <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Select which Family you want to delete.",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}


	std::wstring promptPhrase = L"To delete this family please enter: Delete";

	MessageDlg msg = MessageDlg(hWnd, L"Confirmation",
		promptPhrase.c_str(),
		MD_OKCALNCEL, MD_PROMPT);
	msg.SetPromptStr(L"Delete");
	int res = msg.OpenDlg();

	if (res != IDOK) return;

	PBConnection::Request("deleteFamily", { {"familyId", std::to_wstring(familyId) } },
		[&](PBResponse res)
		{
			MessageDlg(hWnd, L"Success",
				L"Family was deleted successfully.",
				MD_OK, MD_SUCCESS).OpenDlg();

			UpdateFamiliesTable();
		}
	);
}

void FamilyList::OnSetEvnNum()
{
	int familyId = table->GetSelectedRowId();

	if (familyId <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Select Family.",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}

	EditFamily ef(NULL, NULL);

	try
	{
		ef.SetNextEvnNum(familyId);
		UpdateFamiliesTable();
	}

	catch (std::wstring& s)
	{
		MessageDlg(hWnd, L"Error",
			s.c_str(),
			MD_OK, MD_ERR).OpenDlg();
	}

	catch (std::exception& e)
	{
		MessageDlg(hWnd, L"Error",
			StrToWStr(e.what()).c_str(),
			MD_OK, MD_ERR).OpenDlg();
	}
}

void FamilyList::OnExport()
{
	/*
	DBResults data = table->GetSqlData(true, true);
	Exporting ex(hWnd);
	ex.ExportData(data, L"Families", ex.GetSelectedSeparator());
	data = table->GetSqlData(true, true);
	*/
}

void FamilyList::OnSortByLetter(LetterSort item)
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

	OnSearchFamily();
}

bool FamilyList::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);
	if (wId == GetControlId("searchByName"))
	{
		currentSearchBy = L"family";
		OnSearchFamily();
		return true;
	}
	else if (wId == GetControlId("searchByEnvelope"))
	{
		currentSearchBy = L"envelope";
		OnSearchFamily();
		return true;
	}
	else if (wId == GetControlId("searchByPhone"))
	{
		currentSearchBy = L"phone";
		OnSearchFamily();
		return true;
	}
	else if (wId == ID_EDIT_TI)
	{
		HandleEditFamily();
		return true;
	}
	else if (wId == ID_DELETE_TI)
	{
		OnDeleteFamily();
		return true;
	}
	else if (wId == ID_SETNEXTENVNUMBER)
	{
		OnSetEvnNum();
		return true;
	}

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("addNewFamily"))
		{
			ShowAddFamily();
			return true;
		}
		else if (wId == GetControlId("editFamily"))
		{
			HandleEditFamily();
			return true;
		}
		else if (wId == GetControlId("deleteFamily"))
		{
			OnDeleteFamily();
			return true;
		}
		else if (wId == GetControlId("exportFam"))
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
			OnSearchFamily();
			return true;
		}

		break;
	}

	case EN_CHANGE:
	{
		if (wId == GetControlId("searchFamily"))
		{
			SetTimer(hWnd, SEARCH_TIMER_ID, 500, nullptr);
			return true;
		}

		break;
	}
	}

	return false;
}

LRESULT CALLBACK FamilyList::familiesProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	FamilyList* lpData = (FamilyList*)dwRefData;

	switch (uMsg)
	{
	case WM_TIMER:
	{
		if (wParam == SEARCH_TIMER_ID)
		{
			lpData->OnSearchFamily();
		}
		KillTimer(hWnd, wParam);

		break;
	}
	case WM_KEYDOWN:
	{
		if (uIdSubclass == lpData->GetControlId("familyTable"))
		{
			if (wParam == VK_DELETE)
			{
				lpData->OnDeleteFamily();
				return true;
			}
		}

		break;
	}

	case WM_LBUTTONDBLCLK:
	{
		if (uIdSubclass == lpData->GetControlId("familyTable"))
		{
			lpData->HandleEditFamily();
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
			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_FAMILYLISTMENU));
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
				lpData->UpdateFamiliesTable();

				return true;
			}
			else if (formId == lpData->famSortTabs->GetControlId("tab"))
			{
				TCITEM item = { 0 };

				item.mask = TCIF_PARAM;
				int iCurTab = TabCtrl_GetCurSel(lpData->famSortTabs->GetTabsHWND());
				TabCtrl_GetItem(lpData->famSortTabs->GetTabsHWND(), iCurTab, &item);
				int id = static_cast<int> (item.lParam);

				lpData->SwitchFamType(id);
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