#include "Recycle.h"
#include "..//Components/PreloaderComponent.h"
#include "..//PBConnection.h"

#define SEARCH_TIMER_ID		122

Recycle::Recycle(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	table = NULL;
	rTableNames = {
		{ L"Family", L"families" },
		{ L"Member", L"members" },
		{ L"Batch", L"batches" }
	};

}

Recycle::~Recycle()
{
	RemoveWindowSubclass(hWnd, Proc, IDC_STATIC);
	delete table;
}

void Recycle::AddControls()
{
	CreateStatic("sTitle", 10, 10, 200, 40, WS_VISIBLE | WS_CHILD, L"Recycle");
	SetFontSize("sTitle", 28);

	CreateStatic("ssearchFamily", 10, 60, 200, 20, WS_VISIBLE | WS_CHILD, L"Search by recycle:");
	CreateEdit("search", 10, 100, 300, 30, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL);
	SetLabel("search", L"Search item...");

	CreateBtn("delete", 10, 40, 200, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Delete Permanently", WS_STICK_RIGHT);
	SetButtonIcon("delete", IDB_DELETE);

	table = new TableCreater(hWnd, GetWindowInstance(hWnd), this);

	table->Create("contTable", 10, 150, -10, -10);
	table->SetRefClass(this);

	table->CreateColumns({
		{ L"ID", 10, "itemId" },
		{ L"Description", 30, "description" },
		{ L"Type", 30, "type" },
		{ L"Date of Deletion", 30, "deletedDate" } 
	});

	table->InsertColumns();
	table->ClearTable();
	UpdateTable();

	SetWindowSubclass(hWnd, Proc, IDC_STATIC, (DWORD_PTR)this);
}

void Recycle::OnSearch()
{
	UpdateTable([&]()
		{
			SetFocus(GetControlHWND("search"));
		}
	);
}

void Recycle::UpdateTable(std::function<void(void)> callback)
{
	std::wstring searchStr = GetWindowTextStr(GetControlHWND("search"));

	PreloaderComponent::Start(hWnd);
	PBConnection::Request("getRecycle", 
		{ {"searchStr", searchStr} },
		[&, callback](PBResponse res)
		{
			table->ClearTable();
			std::vector<TableColumns> columns = table->GetColums();
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			for (size_t i = 0; i < result.size(); i++)
			{
				table->InsertRow(GetInt(result[i]["itemId"]));

				for (size_t j = 0; j < columns.size(); j++)
				{
					table->InsertItem(result[i][columns[j].dbName], static_cast<int> (j));
				}
			}

			PreloaderComponent::Stop();
			if (callback != nullptr) callback();
		}
	);
}

void Recycle::RecoverItem()
{
	int itemId = table->GetSelectedRowId();

	if (itemId >= 0)
	{
		wchar_t itemType[MAX_PATH] = { 0 };

		ListView_GetItemText(table->GetHWND(), table->GetSelectedRow().iItem, 2, itemType, MAX_PATH);

		if (rTableNames.find(itemType) != rTableNames.end())
		{
			std::wstring tableName = rTableNames[itemType];
			std::wstring itemTypeStr(itemType);


			std::wstring recoverAllMembersInFamily = L"false";

			if (itemTypeStr == L"Family")
			{
				int res = MessageDlg(hWnd, L"Confirmation",
					L"Recover all deleted members of this family?",
					MD_YESNO, MD_QUESTION).OpenDlg();

				if (res == IDYES)
				{
					recoverAllMembersInFamily = L"true";
				}
			}

			PBConnection::Request("recoverItemFromRecycle",
				{
					{"itemId", std::to_wstring(itemId) },
					{"type", tableName},
					{"recoverAllMembersInFamily", recoverAllMembersInFamily}
				},
				[&](PBResponse res)
				{
					UpdateTable();

				}
			);

		}
	}
	else
	{
		MessageDlg(hWnd, L"Error",
			L"Select item you want to recover",
			MD_OK, MD_ERR).OpenDlg();
	}
}

void Recycle::DeleteItem()
{
	int itemId = table->GetSelectedRowId();

	if (itemId >= 0)
	{
		wchar_t itemType[MAX_PATH] = { 0 };
		ListView_GetItemText(table->GetHWND(), table->GetSelectedRow().iItem, 2, itemType, MAX_PATH);

		if (rTableNames.find(itemType) != rTableNames.end())
		{
			std::wstring tableName = rTableNames[itemType];

			int res = MessageDlg(hWnd, L"Confirmation",
				L"The data will be permanently deleted, continue?",
				MD_YESNO, MD_WANING).OpenDlg();

			if (res == IDYES)
			{

				std::wstring itemTypeStr(itemType);

				PBConnection::Request("permanentlyDeleteItem",
					{
						{"itemId", std::to_wstring(itemId) },
						{"type", tableName}
					},
					[&](PBResponse res)
					{
						UpdateTable();
					}
				);

			}
		}
		else
		{
			MessageDlg(hWnd, L"Error",
				L"Unknown Item Type",
				MD_OK, MD_ERR).OpenDlg();
		}
	}
	else
	{
		MessageDlg(hWnd, L"Error",
			L"Select item you want to delete",
			MD_OK, MD_ERR).OpenDlg();
	}
}

bool Recycle::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (wId == ID_DELETE_TI)
	{
		DeleteItem();
		return true;
	}
	else if (wId == ID_RECOVER_TI)
	{
		RecoverItem();
		return true;
	}

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("delete"))
		{
			DeleteItem();
			return true;
		}
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		if (wId == GetControlId("search"))
		{
			SetTimer(hWnd, SEARCH_TIMER_ID, 500, nullptr);
			return true;
		}
	}

	return false;
}


LRESULT CALLBACK Recycle::Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	Recycle* lpData = (Recycle*)dwRefData;

	switch (uMsg)
	{
	case WM_TIMER:
	{
		if (wParam == SEARCH_TIMER_ID)
		{
			lpData->OnSearch();
		}
		KillTimer(hWnd, wParam);

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
			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_TABLEMENURECOVER));
			hMenu = GetSubMenu(hMenu, 0);
			TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, lpData->hWnd, NULL);
			return true;
		}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}