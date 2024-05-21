#include "TransferMember.h"
#include "..//PBConnection.h"

#define SEARCH_TIMER 1228


TransferMember::TransferMember(HWND hWnd, FamilyInfo* fi, int memId) : DlgCommon(hWnd)
{
	this->familyInfo = fi;
	this->hWnd = hWnd;
	this->currentMemberId = memId;
	familySearcher = new UserSearcher(hWnd,
		{ 0 }, UST_FAMILIESONLY);

	minDlgWidth = 600;
}

TransferMember::~TransferMember()
{
	delete familySearcher;
}

int TransferMember::ShowDlg()
{
	return OpenDlg(hWnd, "Transfering Member");
}

void TransferMember::OnDlgInit()
{
	int cY = 20;

	CreateStatic("sff", 10, 20, 200, 30, WS_CHILD | WS_VISIBLE, L"Search for family:", 0);
	CreateEdit("tdSearch", 10, 50, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP);

	CreateStatic("ssb", 10, 100, 80, 30, WS_CHILD | WS_VISIBLE, L"Search by:", 0);

	CreateRadioBtn("searchByLastName", 70, 100, 150, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Last Name");
	CreateRadioBtn("searchByFirstName", 200, 100, 150, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"First Name");
	CreateRadioBtn("searchByEnv", 330, 100, 120, 20, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, L"Envelope");
	CreateRadioGroup("searchByGroup",
		{
			"searchByLastName", "searchByFirstName", "searchByEnv" }, "searchByLastName");

	CreateBtn("btnTransferToNewFamily", 10, 10, 200, 30, WS_CHILD | WS_VISIBLE, L"Transfer To New Family", WS_STICK_RIGHT);

	cY += 100;

	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int lbHeight = screenHeight / 4;

	CreateListBox("tmFamilyList", 10, 140, -10, lbHeight, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED);

	cY += lbHeight + 10;

	CreateBtn("btnCancel", 10, cY, 120, 30, WS_CHILD | WS_VISIBLE, L"Cancel", WS_STICK_RIGHT);
	SetColor("btnCancel", APP_COLORS::GRAY);

	CreateBtn("btnTransferToFamily", 150, cY, 230, 30, WS_CHILD | WS_VISIBLE, L"Transfer To Selected Family", WS_STICK_RIGHT);

	//familySearcher->currentSearchBy = "lastName";

	SetWindowSubclass(GetControlHWND("tdSearch"), ecProc, GetControlId("tdSearch"), (DWORD_PTR)this);
	SetWindowSubclass(GetControlHWND("tmFamilyList"), ecProc, GetControlId("tmFamilyList"), (DWORD_PTR)this);


	DoneLoadingDlg();
}

bool TransferMember::OnDlgCommand(WPARAM wParam)
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
			OnTransferMember();
			return true;
		}
		else if (wId == GetControlId("btnTransferToNewFamily"))
		{
			OnTransferMember(0, true, [&](int s)
				{
					EndDialog(hWnd, s);
				}
			);	// New Family

			return true;
		}
		else if (wId == GetControlId("searchByLastName"))
		{
			familySearcher->SetSearchBy("lastName");
			SearchFamily();
			return true;
		}
		else if (wId == GetControlId("searchByFirstName"))
		{
			familySearcher->SetSearchBy("firstName");
			SearchFamily();
			return true;
		}
		else if (wId == GetControlId("searchByEnv"))
		{
			familySearcher->SetSearchBy("envelope");
			SearchFamily();
			return true;
		}
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		if (wId == GetControlId("tdSearch"))
		{
			SetTimer(GetControlHWND("tdSearch"), SEARCH_TIMER, 500, nullptr);
			return true;
		}
	}

	return false;
}

bool TransferMember::SearchFamily()
{
	std::wstring searchStr = GetWindowTextStr(GetControlHWND("tdSearch"));
	SendMessage(GetControlHWND("tmFamilyList"), LB_RESETCONTENT, 0, 0);

	if (searchStr.length() == 0)
	{
		return false;
	}


	PBConnection::Request("findFamilyOrMember",
		{
			{"searchIn", UTF8ToWstring("families") },
			{"searchBy", UTF8ToWstring(familySearcher->GetSearchBy()) },
			{"searchByVal", searchStr},
		},
		[&](PBResponse res)
		{
			HWND list = GetControlHWND("suList");
			std::vector<std::map<std::string, std::wstring>> result = res.GetResultVec("main");

			for (int i = 0; i < result.size(); i++)
			{

				std::wstring name = result[i]["familyName"];

				LRESULT pos = SendMessage(GetControlHWND("tmFamilyList"), LB_ADDSTRING, 0, (LPARAM)(name.c_str()));
				searchUserIndexes[(unsigned int)pos] = GetInt(result[i]["id"].c_str());
			}
		}
	);


	return true;

}

void TransferMember::OnTransferMember()
{
	int iSelected = ListBox_GetCurSel(GetControlHWND("tmFamilyList"));

	if (iSelected == -1)
	{
		MessageDlg(hWnd, L"Error",
			L"Select Family",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}

	int familyId = searchUserIndexes[iSelected];

	if (familyId <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Family not found",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}

	OnTransferMember(familyId, false, [&](int check)
		{
			if (check > 0) EndDialog(hWnd, check);
		}
	);
}

void TransferMember::OnTransferMember(int familyId, bool newFamily, std::function<void(int)> callback)
{
	if (newFamily == true)
	{
		int msgboxID = MessageDlg(hWnd, L"Confirmation",
			L"Are you sure you want to create a new family?",
			MD_YESNO, MD_QUESTION).OpenDlg();

		if (msgboxID != IDYES) return;
	}

	FamilyInfo::MemberItem* member = familyInfo->currentMember;

	std::wstring familyName = member->lastName + L", " + member->firstName;
	std::wstring informalName = member->firstName + L" " + member->lastName;
	std::wstring mailingName = member->prefix + L" " + informalName;
	std::wstring formalS = member->prefix + L" " + member->lastName;


	PBConnection::Request("transferMember",
		{
			{"memberId", std::to_wstring(currentMemberId)},
			{"familyId", std::to_wstring(familyId) },
			{"newFamily", newFamily ? L"1" : L"0"},
			{"familyName", familyName },
			{"informalName", informalName},
			{"mailingName", mailingName},
			{"formalS", formalS},
			{"memberEmail", member->email},
			{"memberFirstName", member->firstName},
			{"memberLastName", member->lastName},
			{"memberPhone", member->homePhone}
		},
		[&, callback](PBResponse res)
		{
			std::map<std::string, std::wstring> data = res.GetResultMap("main");
			bool removeFamily = data["removeFamily"] == L"1";
			familyId = GetInt(data["familyId"]);

			int msgSwitchBox = -1;
			
			if (!removeFamily)
			{
				msgSwitchBox = MessageDlg(hWnd, L"Confirmation",
					L"Member transfered successfully, switch to the family you just transfered this member to?",
					MD_YESNO, MD_QUESTION).OpenDlg();
			}

			if (msgSwitchBox == IDYES || removeFamily)
			{
				// SWITCH PAGE
				familyInfo->transferToFamId = familyId;
				callback(TMM_SWITCHFAMILY);
				return;
			}
			callback(IDOK);

		}
	);
}

LRESULT CALLBACK TransferMember::ecProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	TransferMember* lpData = (TransferMember*)dwRefData;

	switch (uMsg)
	{
	case WM_TIMER:
	{
		if (wParam == SEARCH_TIMER)
		{
			lpData->SearchFamily();
		}
		KillTimer(hWnd, wParam);

		break;
	}
	case WM_KEYUP:
	{
		if (wParam == VK_RETURN)
		{
			if (uIdSubclass == lpData->GetControlId("tmFamilyList"))
			{
				lpData->OnTransferMember();
				break;
			}
		}

		break;
	}

	case WM_KEYDOWN:
	{
		if (uIdSubclass == lpData->GetControlId("tdSearch"))
		{
			if (wParam == VK_DOWN)
			{
				HWND listBoxItem = lpData->GetControlHWND("tmFamilyList");
				int itemsCount = ListBox_GetCount(listBoxItem);

				if (itemsCount == 0) break;

				SetFocus(listBoxItem);
				ListBox_SetCurSel(listBoxItem, 0);
				return TRUE;
			}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}