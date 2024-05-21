#include "FamilyDonationsList.h"
#include "..//Offering/EnterContribution.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

FamilyDonationsList::FamilyDonationsList(HWND hWnd, FamilyInfo* fi, int memId) : DlgCommon(hWnd)
{
	table = NULL;
	this->familyInfo = fi;
	this->hWnd = hWnd;
	this->currentMemberId = 0;
	familySearcher = new UserSearcher(hWnd, { 0 }, UST_FAMILIESONLY);
	currentFundId = 0;
	minDlgWidth = 600;
	currentYear = 0;
}

FamilyDonationsList::~FamilyDonationsList()
{
	delete familySearcher;
}

int FamilyDonationsList::ShowDlg()
{
	return OpenDlg(hWnd, "Family Donations");
}

void FamilyDonationsList::AddControls()
{
	int cY = 20;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int dlgWidth = screenWidth / 2;

	int itemsOffsetX = dlgWidth / 100 * 5;
	int selectMembersWidth = dlgWidth / 100 * 30;
	int selectYearWidth = dlgWidth / 100 * 20;
	int selectBatchWidth = dlgWidth / 100 * 45;
	int itemX = 10;

	CreateStatic("sm", itemX, 10, 300, 20, WS_VISIBLE | WS_CHILD, L"Select member:");
	HWND membersH = CreateCombobox("selectMembers", itemX, 40, selectMembersWidth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL);

	itemX += selectMembersWidth + itemsOffsetX;

	CreateStatic("sm2", itemX, 10, selectYearWidth, 20, WS_VISIBLE | WS_CHILD, L"Year:");
	HWND yearsH = CreateCombobox("selectYear", itemX, 40, selectYearWidth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL);

	CreateStatic("sm3", 10, 10, selectBatchWidth, 20, WS_VISIBLE | WS_CHILD, L"Select fund:", WS_STICK_RIGHT);
	HWND selectFundH = CreateCombobox("selectFund", 10, 40, selectBatchWidth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL, L"", WS_STICK_RIGHT);

	


	int yTotalD = screenHeight / 2;
	CreateStatic("totalDonated", 10, 90 + yTotalD + 10, 300, 20, WS_VISIBLE | WS_CHILD);


	SendMessage(membersH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Whole Family");
	for (auto it : familyInfo->members)
	{
		std::wstring memberName = it.second.firstName + L" ID:" + std::to_wstring(it.first);
		SendMessage(membersH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)memberName.c_str());
		memberIds[memberName] = it.first;
	}

	SendMessage(membersH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);


	int sContType = 0, sFamLookUp = 0, sFundId = 0;

	SendMessage(selectFundH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"All funds");

	for (auto& it : fundsList)
	{
		int fI = static_cast<int> (SendMessage(selectFundH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)it.second.c_str()));
		fundsIds[it.second] = it.first;
	}

	SendMessage(selectFundH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	CreateTable();
}

void FamilyDonationsList::CreateTable()
{

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	table = new TableCreater(hWnd, GetWindowInstance(hWnd), this);

	table->Create("contTable", 10, 90, screenWidth / 2, screenHeight / 2);

	table->CreateColumns({
		{ L"Batch", 20, "batchName" },
		{ L"Amount", 10, "amount" },
		{ L"Fund", 20, "fund" },
		{ L"Member first name", 30, "memberFirstName" },
		{ L"Memo", 20, "memo" }
		});

	table->InsertColumns();

	SetWindowSubclass(table->GetHWND(), MainProc, -1, (DWORD_PTR)this);
}

void FamilyDonationsList::OnDlgInit()
{
	AddControls();

	DoneLoadingDlg();
	table->ClearTable();

	PBConnection::Request("getFunds", {},
		[&](PBResponse res)
		{

			// Load funds list
			{
				std::vector<std::map<std::string, std::wstring>> result = res.GetResultVec("main");
				for (auto& it : result)
				{
					fundsList[GetInt(it["id"])] = it["name"];
				}
			}

			UpdateYearField([&]()
				{
					UpdateTable([&]()
						{
						}
					);
				}
			);
		}
	);

}

void FamilyDonationsList::OnDlgResized()
{
	table->ResetScrollSize();
}

void FamilyDonationsList::UpdateYearField(std::function<void(void)> callback)
{

	PBConnection::Request("getYearsForDonationHistory", {
			{"familyId", std::to_wstring(familyInfo->familyId) },
			{"memberId", std::to_wstring(currentMemberId) },
			{"fundId",  std::to_wstring(currentFundId) }
		},
		[&, callback](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResultVec("main");

			HWND yearsH = GetControlHWND("selectYear");
			SendMessage(yearsH, (UINT)CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

			SendMessage(yearsH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"All");
			for (int i = 0; i < result.size(); i++)
			{
				SendMessage(yearsH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)result[i]["year"].c_str());
			}

			SendMessage(yearsH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

			if (callback != nullptr) callback();

		}
	);

	
}

void FamilyDonationsList::UpdateTable(std::function<void(void)> onUpdated)
{
	PreloaderComponent::Start(hWnd);

	PBConnection::Request("getDonationsHistory", {
			{"familyId", std::to_wstring(familyInfo->familyId) },
			{"memberId", std::to_wstring(currentMemberId) },
			{"fundId",  std::to_wstring(currentFundId) },
			{"year", std::to_wstring(currentYear) }
		},
		[&, onUpdated](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResultVec("main");

			table->ClearTable();

			std::vector<TableColumns> columns = table->GetColums();

			float totalSum = 0.0f;
			for (size_t i = 0; i < result.size(); i++)
			{
				table->InsertRow(GetInt(result[i]["batchId"]));

				for (size_t j = 0; j < columns.size(); j++)
				{
					table->InsertItem(result[i][columns[j].dbName], static_cast<int> (j));
					if (columns[j].dbName == "amount")
					{
						totalSum += GetFloat(result[i][columns[j].dbName]);
					}
				}
			}

			std::wstring str = L"Total donated: $" + GetFloatFormatStr(std::to_wstring(totalSum));
			SetWindowText(GetControlHWND("totalDonated"), str.c_str());

			PreloaderComponent::Stop();

			if (onUpdated) onUpdated();

		}
	);
}

void FamilyDonationsList::ShowBatchDesc(int batchId)
{
	EnterContribution contDlg(batchId, hWnd, hInst);
}

bool FamilyDonationsList::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("btnCancel"))
		{
			EndDialog(hWnd, IDCANCEL);
			return true;
		}
		else if (wId == GetControlId("btnTransferToFamily"))
		{
			return true;
		}
		else if (wId == GetControlId("btnTransferToNewFamily")) {}
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		if (wId == GetControlId("tdSearch"))
		{
			return true;
		}
	}
	else if (HIWORD(wParam) == CBN_SELCHANGE)
	{
		if (wId == GetControlId("selectFund"))
		{
			currentFundId = fundsIds[GetWindowTextStr(GetControlHWND("selectFund"))];
			UpdateTable();
			return true;
		}
		else if (wId == GetControlId("selectMembers"))
		{
			currentMemberId = memberIds[GetWindowTextStr(GetControlHWND("selectMembers"))];
			UpdateTable();
			return true;
		}
		else if (wId == GetControlId("selectYear"))
		{
			currentYear = GetInt(GetWindowTextStr(GetControlHWND("selectYear")));
			UpdateTable();
			return true;
		}
	}

	return false;
}

LRESULT CALLBACK FamilyDonationsList::MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	FamilyDonationsList* lpData = (FamilyDonationsList*)dwRefData;

	switch (uMsg)
	{
	case WM_LBUTTONDBLCLK:
	{
		int rowId = lpData->table->GetSelectedRowId();
		if (rowId > 0)
		{
			lpData->ShowBatchDesc(rowId);
			return true;
		}
		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}