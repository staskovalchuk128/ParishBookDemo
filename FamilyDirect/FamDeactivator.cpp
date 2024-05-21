#include "FamDeactivator.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

FamDeactivator::FamDeactivator(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	table = NULL;
	currentFamiliesList = {};
}

FamDeactivator::~FamDeactivator()
{
	RemoveWindowSubclass(hWnd, Proc, IDC_STATIC);
	delete table;
}

void FamDeactivator::AddControls()
{
	CreateStatic("sTitle", 10, 10, 500, 40, WS_VISIBLE | WS_CHILD, L"Deactivate Families");
	SetFontSize("sTitle", 28);

	std::wstring currentYear = GetCurrentYear();

	CreateStatic("ssearchFamily", 10, 60, 300, 20, WS_VISIBLE | WS_CHILD, std::wstring(L"Year offset from current year(" + currentYear + L"):").c_str());
	CreateEdit("yearOffset", 10, 100, 300, 30, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE, L"2");

	CreateStatic("ssIncludeWithoutDonations", 330, 106, 200, 20, WS_VISIBLE | WS_CHILD, L"Families without donations");
	CreateCheckBox("includeWithoutDonations", 520, 105, 50, 30, WS_VISIBLE | WS_CHILD | BS_CHECKBOX);


	CreateBtn("findFams", 590, 100, 250, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Find families to deactivate");
	SetButtonIcon("findFams", IDB_SEARCH);

	CreateBtn("deactivateFamilies", 10, 40, 200, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Deactivate families", WS_STICK_RIGHT);
	SetButtonIcon("deactivateFamilies", IDB_DELETE);
	SetColor("deactivateFamilies", APP_COLORS::RED);

	table = new TableCreater(hWnd, GetWindowInstance(hWnd), this);

	table->Create("contTable", 10, 150, -10, -10);
	table->SetRefClass(this);

	table->CreateColumns({
		{ L"ID", 10, "famId" },
		{ L"Family", 30, "famName" },
		{ L"Last batch", 40, "lastBatch" }
		});

	table->InsertColumns();

	table->ClearTable();
	SetWindowSubclass(hWnd, Proc, IDC_STATIC, (DWORD_PTR)this);
}

void FamDeactivator::UpdateTable(std::function<void(void)> onUpdated)
{

	int yearOffset = GetInt(GetWindowTextStr(GetControlHWND("yearOffset")));
	int includeWIthoutDonations = IsBtnChecked("includeWithoutDonations");

	if (yearOffset <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Enter valid year offset",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}

	PreloaderComponent::Start(hWnd);

	PBConnection::Request("findFamiliesToDeactivate",
		{ 
			{ "yearOffset", std::to_wstring(yearOffset) },
			{ "includeWIthoutDonations", std::to_wstring(includeWIthoutDonations) }
		},
		[&](PBResponse res)
		{

			currentFamiliesList.clear();
			table->ClearTable();

			std::vector<TableColumns> columns = table->GetColums();

			std::vector<std::map<std::string, std::wstring>> result = res.GetResultVec("main");

			for (size_t i = 0; i < result.size(); i++)
			{
				table->InsertRow(GetInt(result[i]["famId"]));

				currentFamiliesList.push_back(GetInt(result[i]["famId"]));

				for (size_t j = 0; j < columns.size(); j++)
				{
					table->InsertItem(result[i][columns[j].dbName], static_cast<int> (j));
				}
			}

			PreloaderComponent::Stop();

		}
	);


}

void FamDeactivator::OnFindFamilies()
{
	UpdateTable([&]()
		{
			int foundFamsNum = (int)currentFamiliesList.size();

			MessageDlg(hWnd, L"Info",
				std::wstring(L"Found " + std::to_wstring(foundFamsNum) + L" families.").c_str(),
				MD_OK, MD_OK).OpenDlg();
		}
	);
	
	
}

void FamDeactivator::RemoveFamilyFromList()
{
	int itemId = table->GetSelectedRowId();
	std::vector<int>::iterator it = std::find_if(currentFamiliesList.begin(), currentFamiliesList.end(), [itemId](int& id)
		{
			return id == itemId;
		}
	);
	if (it != currentFamiliesList.end())
	{
		currentFamiliesList.erase(it);
		table->DeleteRow();

	}
}

void FamDeactivator::DeactivateFamilies()
{
	int famsDeactivated = (int)currentFamiliesList.size();

	if (famsDeactivated <= 0)
	{

		MessageDlg(hWnd, L"Info",
			std::wstring(L"No families to deactivate.").c_str(),
			MD_OK, MD_WANING).OpenDlg();

		return;
	}

	int res = MessageDlg(hWnd, L"Info",
		std::wstring(L"Are you sure you want to deactivate selected families?").c_str(),
		MD_YESNO, MD_QUESTION).OpenDlg();

	if (res != IDYES) return;

	PreloaderComponent::Start(hWnd);

	std::map<std::string, std::wstring> requestParams = {};

	for (size_t i = 0; i < currentFamiliesList.size(); i++)
	{
		requestParams[std::to_string(i)] = std::to_wstring(currentFamiliesList[i]);
	}

	PBConnection::Request("deactivateFamilies", requestParams,
		[&](PBResponse res)
		{
			int famsDeactivated = (int)currentFamiliesList.size();

			currentFamiliesList.clear();
			table->ClearTable();

			PreloaderComponent::Stop();

			MessageDlg(hWnd, L"Info",
				std::wstring(L"Deactivated " + std::to_wstring(famsDeactivated) + L" families.").c_str(),
				MD_OK, MD_OK).OpenDlg();
		}
	);	
}

bool FamDeactivator::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (wId == ID_DELETE_TI)
	{
		RemoveFamilyFromList();
		return true;
	}

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("deactivateFamilies"))
		{
			DeactivateFamilies();
			return true;
		}
		else if (wId == GetControlId("findFams"))
		{
			OnFindFamilies();
			return true;
		}
	}


	return false;
}


LRESULT CALLBACK FamDeactivator::Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	FamDeactivator* lpData = (FamDeactivator*)dwRefData;

	switch (uMsg)
	{
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
			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_TABLEMENUFAMDEACTIVATE));
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