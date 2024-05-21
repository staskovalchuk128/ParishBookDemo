#include "Batches.h"
#include "..//PBConnection.h"

#define SEARCH_BY_DESC_TIMER_ID			122

Batches::Batches(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	currentSearchBy = L"desc";
	currentPage = L"1";
	currentBatchViewStatus = L"Open";
	currentDateFrom = currentDateTo = L"";
	showLimits = { L"25", L"50", L"100" };

	table = NULL;
}

Batches::~Batches()
{
	RemoveWindowSubclass(hWnd, batchesProc, IDC_STATIC);

	delete batchesSortTabs;
	delete table;

}

std::wstring Batches::GetCurrentBatchViewStatus()
{
	return currentBatchViewStatus;
}

void Batches::PrintContr(int batchId)
{
	ContReports contReports(GetParent(hWnd));
	contReports.Print(batchId);
}

void Batches::OnTableSort(int columnId, int type, DWORD_PTR context)
{
	TableCreater* lpData = (TableCreater*)context;

	try
	{
		TableColumns colInfo = lpData->GetColumnInfo(columnId);
		table->SetSortBy(colInfo.dbName);
		table->SetSortType(type == 2 ? "ASC" : "DESC");
		UpdateBatchesTable();
	}

	catch (std::string s) {}

	catch (std::exception e) {}

	// 1= DESC, 2 = ASC
}

void Batches::AddControls()
{
	CreateTable();

	CreateStatic("ssearchBatch", 10, 30, 100, 20, WS_VISIBLE | WS_CHILD, L"Search batch:");
	CreateEdit("searchBatch", 10, 60, 200, 30);
	SetLabel("searchBatch", L"Search batch...");

	CreateStatic("ssearchBy", 10, 105, 100, 20, WS_VISIBLE | WS_CHILD, L"Search by:");

	CreateRadioBtn("searchByDesc", 90, 105, 150, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Description");
	CreateRadioBtn("searchByComment", 230, 105, 130, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Comment");
	CreateRadioBtn("searchByMemo", 350, 105, 100, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Memo");
	CreateRadioGroup("sortGroup",
		{
			"searchByDesc", "searchByComment", "searchByMemo" }, "searchByDesc");

	CreateStatic("sdf", 230, 30, 100, 20, WS_VISIBLE | WS_CHILD, L"Date From:");
	CreateEdit("dateFrom", 230, 60, 200, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");

	CreateStatic("sdt", 440, 30, 100, 20, WS_VISIBLE | WS_CHILD, L"Date To:");
	CreateEdit("dateTo", 440, 60, 200, 30);

	SetEditMask("dateFrom", ES_DATE_MASK);
	SetEditMask("dateTo", ES_DATE_MASK);

	CreateBtn("deleteBatch", 10, 40, 150, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Delete Batch", WS_STICK_RIGHT);
	SetColor("deleteBatch", APP_COLORS::RED);
	CreateBtn("editBatch", 170, 40, 130, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Edit Batch", WS_STICK_RIGHT);
	CreateBtn("addNewBatch", 310, 40, 130, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Add Batch", WS_STICK_RIGHT);
	//CreateBtn("export", 450, 40, 30, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"", WS_STICK_RIGHT);

	SetButtonIcon("deleteBatch", IDB_DELETE);
	SetButtonIcon("editBatch", IDB_EDIT);
	SetButtonIcon("addNewBatch", IDB_ADD);
	//SetButtonIcon("export", IDB_EXPORTW);

	CreateStatic("sshowLimit", 115, 97, 50, 20, WS_VISIBLE | WS_CHILD | ES_RIGHT, L"Show: ", WS_STICK_RIGHT);
	HWND showLimitH = CreateCombobox("showLimit", 10, 90, 100, 30, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED, L"", WS_STICK_RIGHT);

	for (int i = 0; i < showLimits.size(); i++)
	{
		SendMessage(showLimitH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)showLimits[i].c_str());
	}

	SendMessage(showLimitH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	SetWindowSubclass(hWnd, batchesProc, IDC_STATIC, (DWORD_PTR)this);
	SetWindowSubclass(GetControlHWND("batchesTable"), batchesProc, GetControlId("batchesTable"), (DWORD_PTR)this);

	Tabs tabs;
	tabs.AddTab(0, L"Active");
	tabs.AddTab(1, L"Archive");

	batchesSortTabs = new TabControl(hWnd, tabs, 0, TCS_FIXEDWIDTH, 10, 140, -10, 30, false);
	batchesSortTabs->SetBgColor(APP_COLORS::WHITE);
	SendMessage(batchesSortTabs->GetTabsHWND(), TCM_SETITEMSIZE, 0, MAKELPARAM(100, 30));


}

void Batches::CreateTable()
{
	table = new TableCreater(hWnd, hInst, this);
	table->SetRefClass(this);
	table->Create("batchesTable", 10, 170, -10, -50);

	table->CreateColumns({
		{ L"ID", 10, "id" },
		{ L"Descrtiption", 30, "description" },
		{ L"Date", 20, "date" },
		{ L"Amount", 20, "amount" },
		{ L"Comment", 20, "comment" } 
	});

	table->InsertColumns();

	TableCData* cData = new TableCData(hWnd, hInst);
	cData->currentSortBy = "date";
	cData->currentSortType = "DESC";
	table->SetTableCData(cData);

	UpdateBatchesTable([&]()
		{
			//DELETE THIS
			//OnEnterContr(144199);
			UpdateWindow(hWnd);
		});

}

void Batches::OnExport()
{
	/*\
	DBResults data = table->GetSqlData(true, true);
	Exporting ex(hWnd);
	ex.ExportData(data, L"Batches", ex.GetSelectedSeparator());
	data = table->GetSqlData(true, true);
	*/
}

void Batches::OnSearchByDescBatch()
{
	ChangeCurrentPage(1);
	UpdateBatchesTable([&]()
		{
			SetFocus(GetControlHWND("searchBatch"));
		}
	);
}

void Batches::OnSearchByDateBatch(bool isFrom)
{
	ChangeCurrentPage(1);
	UpdateBatchesTable([&, isFrom]()
		{
			if (isFrom)
			{
				SetFocus(GetControlHWND("dateFrom"));
			}
			else
			{
				SetFocus(GetControlHWND("dateTo"));
			}
		}
	);
}

void Batches::ChangeCurrentPage(int page)
{
	currentPage = std::to_wstring(page);
	table->SetCurrentPage(page);
}

void Batches::UpdateBatchesTable(std::function<void(void)> onTableUpdateOverCallback)
{
	/*REFRESH DATE FIELDS*/
	std::wstring fromVal = GetWindowTextStr(GetControlHWND("dateFrom"));
	std::wstring toVal = GetWindowTextStr(GetControlHWND("dateTo"));

	if (fromVal.length() > 0 && fromVal.length() != 10)
	{
		currentDateFrom = L"";
		SetWindowText(GetControlHWND("dateFrom"), L"");
	}
	if (toVal.length() > 0 && toVal.length() != 10)
	{
		currentDateTo = L"";
		SetWindowText(GetControlHWND("dateTo"), L"");
	}

	std::wstring showLimits = GetComboBoxValue("showLimit");
	table->SetSearchShowLimit(showLimits);


	table->UpdateTable("getBatches",
		{
			{ "type", GetCurrentBatchViewStatus() },
			{ "limit", GetComboBoxValue("showLimit") },
			{ "searchIn", currentSearchBy },
			{ "searchStr", GetWindowTextStr(GetControlHWND("searchBatch")) },
			{ "orderByField", L"date" },
			{ "orderByType", L"ASC" },
			{ "currentPage", currentPage },
			{ "dateFrom",  currentDateFrom},
			{ "dateTo", currentDateTo }
		},
		onTableUpdateOverCallback
	);
}

bool Batches::HandleEditBatch()
{
	int batchId = table->GetSelectedRowId();

	if (batchId <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Select which batch you want to edit",
			MD_OK, MD_ERR).OpenDlg();
		return false;
	}

	OnAddBatch(batchId);
	return true;
}

void Batches::DeleteBatch()
{
	int deleteItemId = table->GetSelectedRowId();

	if (deleteItemId > 0)
	{
		PBConnection::Request("deleteBatch", { {"batchId", std::to_wstring(deleteItemId)} },
			[&](PBResponse res)
			{
				UpdateBatchesTable([&]()
					{
						SetFocus(GetControlHWND("batchesTable"));
					}
				);
			}
		);
	}
}

void Batches::ShowDeleteBatchDialog()
{
	int selectedBatch = table->GetSelectedRowId();
	if (selectedBatch < 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Select which batch you want to delete",
			MD_OK, MD_ERR).OpenDlg();
	}
	else
	{

		std::wstring batchName = table->GetTextFromCell(table->GetSelectedRow().iItem, 1);
		batchName = batchName.length() == 0 ? L"blank" : batchName;
		std::wstring promptPhrase = L"To delete this batch please enter: " + batchName;

		MessageDlg msg = MessageDlg(hWnd, L"Confirmation",
			promptPhrase.c_str(),
			MD_OKCALNCEL, MD_PROMPT);
		msg.SetPromptStr(batchName.c_str());
		int res = msg.OpenDlg();

		if (res == IDOK)
		{
			DeleteBatch();
		}
	}
}

void Batches::OnEnterContr(int batchId)
{
	EnterContribution enterContr(batchId, hWnd, hInst);
	//DIALOG

	int a = 0;
}

void Batches::OnAddBatch(int batchId)
{
	int enterContrBId = 0;
	AddBatch* addBatch = new AddBatch(hWnd, hInst, batchId);
	//DIALOG
	enterContrBId = batchId == 0 ? addBatch->GetBatchId() : 0;
	delete addBatch;

	UpdateBatchesTable();

	if (enterContrBId > 0)
	{
		int msg = MessageDlg(hWnd, L"Message",
			L"Do you want to switch to enter contribution?",
			MD_YESNO, MD_QUESTION).OpenDlg();

		if (msg == IDYES)
		{
			OnEnterContr(enterContrBId);
		}
	}
}

void Batches::SwitchBathesType(int type)
{
	ChangeCurrentPage(1);
	currentBatchViewStatus = type == 0 ? L"Open" : L"Closed";
	UpdateBatchesTable();
}

bool Batches::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (wId == GetControlId("searchByDesc"))
	{
		currentSearchBy = L"desc";
		UpdateBatchesTable();
		return true;
	}
	else if (wId == GetControlId("searchByComment"))
	{
		currentSearchBy = L"comment";
		UpdateBatchesTable();
		return true;
	}
	else if (wId == GetControlId("searchByMemo"))
	{
		currentSearchBy = L"memo";
		UpdateBatchesTable();
		return true;
	} else if (wId == ID_EDIT_TI)
	{
		HandleEditBatch();
		return true;
	}
	else if (wId == ID_CLOSEBATCH)
	{
		int batchId = table->GetSelectedRowId();
		UpdateBatchesTable();
		return true;
	}
	else if (wId == ID_OPENBATCH)
	{
		int batchId = table->GetSelectedRowId();
		UpdateBatchesTable();
		return true;
	}
	else if (wId == ID_CONTEXTMENU_PRINTCONTRIBUTION)
	{
		int batchId = table->GetSelectedRowId();
		PrintContr(batchId);
		return true;
	}
	else if (wId == ID_CONTEXTMENU_ENTERCONTRIBUTION)
	{
		int batchId = table->GetSelectedRowId();
		OnEnterContr(batchId);
		return true;
	}
	else if (wId == ID_DELETE_TI)
	{
		ShowDeleteBatchDialog();
		return true;
	}

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("addNewBatch"))
		{
			OnAddBatch();
			return true;
		}
		else if (wId == GetControlId("editBatch"))
		{
			HandleEditBatch();
			return true;
		}
		else if (wId == GetControlId("deleteBatch"))
		{
			ShowDeleteBatchDialog();
			return true;
		}
		else if (wId == GetControlId("export"))
		{
			OnExport();
			return true;
		}
		break;
	}

	case EN_CHANGE:
	{
		if (wId == GetControlId("searchBatch"))
		{
			SetTimer(hWnd, SEARCH_BY_DESC_TIMER_ID, 500, nullptr);
			return true;
		}
		else if (wId == GetControlId("dateFrom") || wId == GetControlId("dateTo"))
		{
			OnSearchByDate();
		}

		break;
	}

	case CBN_SELCHANGE:
	{
		if (wId == GetControlId("showLimit"))
		{
			UpdateBatchesTable();
			return true;
		}

		break;
	}
	}

	return false;
}


void Batches::OnSearchByDate()
{
	std::wstring fromVal = GetWindowTextStr(GetControlHWND("dateFrom"));
	std::wstring toVal = GetWindowTextStr(GetControlHWND("dateTo"));

	if (fromVal.length() == 10)
	{
		currentDateFrom = ChangeDateFormat(fromVal);
		OnSearchByDateBatch(true);
	}
	else if(currentDateFrom.size() == 10 && fromVal == L"")
	{
		currentDateFrom = L"";
		OnSearchByDateBatch(true);
	}
	
	if (toVal.length() == 10)
	{
		currentDateTo = ChangeDateFormat(toVal);
		OnSearchByDateBatch(false);
	}
	else if(currentDateTo.length() == 10 && toVal == L"")
	{
		currentDateTo = L"";
		OnSearchByDateBatch(false);
	}
	

}

LRESULT CALLBACK Batches::batchesProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	Batches* lpData = (Batches*)dwRefData;

	switch (uMsg)
	{
	case WM_TIMER:
	{
		if (wParam == SEARCH_BY_DESC_TIMER_ID)
		{
			lpData->OnSearchByDescBatch();
		}
		KillTimer(hWnd, wParam);

		break;
	}
	case WM_KEYDOWN:
	{
		if (uIdSubclass == lpData->GetControlId("batchesTable"))
		{
			if (wParam == VK_DELETE)
			{
				lpData->ShowDeleteBatchDialog();
				return true;
			}
		}

		break;
	}

	case WM_LBUTTONDBLCLK:
	{
		if (uIdSubclass == lpData->GetControlId("batchesTable"))
		{
			int batchId = lpData->table->GetSelectedRowId();
			if (batchId == 0) return true;
			lpData->OnEnterContr(batchId);
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
			int index = lpData->table->GetSelectedRowId();
			if (index < 0) return true;

			POINT cursor;
			GetCursorPos(&cursor);
			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(lpData->GetCurrentBatchViewStatus() == L"Open" ? IDR_OPENEDBATCHMENU : IDR_CLOSEDBATCHMENU));
			hMenu = GetSubMenu(hMenu, 0);
			TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, lpData->hWnd, NULL);
			return true;
		}

		case TCN_SELCHANGE:
		{
			TabControl* cDataTabs = lpData->table->GetTableCData()->spBtns;

			int formId = static_cast<int> (((LPNMHDR)lParam)->idFrom);
			if (formId == cDataTabs->GetControlId("tab"))
			{
				TCITEM item = { 0 };

				item.mask = TCIF_PARAM;
				int iCurTab = TabCtrl_GetCurSel(cDataTabs->GetTabsHWND());
				TabCtrl_GetItem(cDataTabs->GetTabsHWND(), iCurTab, &item);

				lpData->ChangeCurrentPage((int)item.lParam);
				lpData->UpdateBatchesTable();

				return true;
			}
			else if (formId == lpData->batchesSortTabs->GetControlId("tab"))
			{
				TCITEM item = { 0 };

				item.mask = TCIF_PARAM;
				int iCurTab = TabCtrl_GetCurSel(lpData->batchesSortTabs->GetTabsHWND());
				TabCtrl_GetItem(lpData->batchesSortTabs->GetTabsHWND(), iCurTab, &item);
				int id = static_cast<int> (item.lParam);

				lpData->SwitchBathesType(id);
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