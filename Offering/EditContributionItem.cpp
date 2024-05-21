#include "EditContributionItem.h"
#include "..//PBConnection.h"

EditContributionItem::EditContributionItem(HWND hWnd, std::vector<int> itemIds, std::map<int, std::wstring> fundsList, ContributionData* Data) : DlgCommon(hWnd)
{
	this->hWnd = hWnd;
	this->cData = Data;
	this->itemIds = itemIds;
	this->fundsList = fundsList;
	minDlgWidth = 500;
}

EditContributionItem::~EditContributionItem() {}

int EditContributionItem::ShowDlg()
{
	return OpenDlg(hWnd, "Editing Contribution");
}

bool EditContributionItem::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("btnCancel"))
		{
			EndDialog(hWnd, wId);
			return true;
		}
		else if (wId == GetControlId("btnSave"))
		{
			OnSaveContItem();
			return true;
		}
	}

	return false;
}

bool EditContributionItem::OnSaveContItem()
{
	HWND fundH = GetControlHWND("fund");

	std::wstring nAmount = GetWindowTextStr(GetControlHWND("amount"));
	std::wstring nMemo = GetWindowTextStr(GetControlHWND("memo"));

	float amount = GetFloat(nAmount);

	int fundId = (int)ComboBox_GetCurSel(fundH);
	wchar_t fundNameBuf[1024] = { 0 };
	SendMessage(GetControlHWND("fund"), (UINT)CB_GETLBTEXT, (WPARAM)fundId, (LPARAM)&fundNameBuf);
	int nFundId = fundsIds[*fundNameBuf != NULL ? fundNameBuf : L""];


	if (multiplyEdit)
	{
		for (size_t i = 0; i < cData->editItemsData.size(); i++)
		{
			cData->editItemsData[i].fund = nFundId;
		}
	}
	else
	{
		if (amount <= 0)
		{
			MessageDlg(hWnd, L"Error",
				L"Enter correct amount",
				MD_OK, MD_ERR).OpenDlg();
			return false;
		}

		cData->editItemsData[0].amount = amount;
		cData->editItemsData[0].memo = nMemo;
		cData->editItemsData[0].fund = nFundId;
	}


	EndDialog(hWnd, IDOK);

	return true;

}

void EditContributionItem::OnDlgInit()
{
	try
	{
		LoadData();
	}
	catch (const std::exception&)
	{

	}
	catch (const std::string& str)
	{
		MessageDlg(hWnd, L"Error",
			StrToWStr(str).c_str(),
			MD_OK, MD_ERR).OpenDlg();


		EndDialog(hWnd, IDCANCEL);
	}
}

void EditContributionItem::AddControls()
{
	HWND fundH = NULL;

	if (!multiplyEdit)
	{
		wchar_t cAmount[64] = { 0 };

		swprintf_s(cAmount, L"%.2f", cData->editItemsData[0].amount);

		CreateStatic("sff", 10, 20, -30, 30, WS_CHILD | WS_VISIBLE, cData->editItemsData[0].name.c_str());

		CreateStatic("sAmount", 10, 60, 200, 20, WS_CHILD | WS_VISIBLE, L"Amount:");
		CreateEdit("amount", 10, 80, 200, 30, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_TABSTOP | ES_AUTOHSCROLL, cAmount);

		CreateStatic("sFund", 230, 60, 280, 20, WS_CHILD | WS_VISIBLE, L"Fund:");
		fundH = CreateCombobox("fund", 230, 80, 280, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | CBS_OWNERDRAWFIXED);

		CreateStatic("sMemo", 530, 60, 200, 20, WS_CHILD | WS_VISIBLE, L"Memo:");
		CreateEdit("memo", 530, 80, 200, 30, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_TABSTOP | ES_AUTOHSCROLL, cData->editItemsData[0].memo.c_str());
	}
	else
	{
		CreateStatic("sFund", 10, 60, -10, 20, WS_CHILD | WS_VISIBLE, L"Fund:");
		fundH = CreateCombobox("fund", 10, 80, -10, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | CBS_OWNERDRAWFIXED);

	}


	CreateBtn("btnCancel", 10, 140, 120, 30, WS_CHILD | WS_VISIBLE, L"Cancel", WS_STICK_RIGHT);
	SetColor("btnCancel", APP_COLORS::GRAY);

	CreateBtn("btnSave", 150, 140, 120, 30, WS_CHILD | WS_VISIBLE, L"Save", WS_STICK_RIGHT);

	int sFundId = 0;

	for (auto& it : fundsList)
	{
		int fI = static_cast<int> (SendMessage(fundH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)it.second.c_str()));
		if (it.first == cData->editItemsData[0].fund) sFundId = fI;
		fundsIds[it.second] = it.first;
	}

	SendMessage(fundH, CB_SETCURSEL, (WPARAM)sFundId, (LPARAM)0);

	SetWindowSubclass(GetControlHWND("fund"), ecProc, GetControlId("fund"), (DWORD_PTR)this);
	SetWindowSubclass(GetControlHWND("amount"), ecProc, GetControlId("amount"), (DWORD_PTR)this);
	SetWindowSubclass(GetControlHWND("memo"), ecProc, GetControlId("memo"), (DWORD_PTR)this);

	//Select all amount
	if (!multiplyEdit)
	{
		reinterpret_cast<EditCtrl*>(GetControlPtr("amount"))->SelectAllOnFocus(true);
	}

	DoneLoadingDlg();
}


void EditContributionItem::LoadData()
{
	std::map<std::string, std::wstring> requestParams = { {"batchId", std::to_wstring(cData->id) } };
	int index = 0;
	for (auto& it : itemIds)
	{
		requestParams[std::to_string(index)] = std::to_wstring(it);
		index++;
	}

	PBConnection::Request("getContributionsData", requestParams, [&](PBResponse res)
		{
			for (auto& it : res.GetResultVec("main"))
			{
				ContEditItemData cDataItem = {};

				cDataItem.id = GetInt(it["id"]);
				cDataItem.name = it["Name"];
				cDataItem.memo = it["memo"];

				cDataItem.fund = GetInt(it["fundId"]);
				cDataItem.envelope = GetInt(it["envelope"]);
				cDataItem.amount = GetFloat(it["amount"]);

				cData->editItemsData.push_back(cDataItem);
			}

			multiplyEdit = cData->editItemsData.size() > 1;

			AddControls();
		}
	);
}

LRESULT CALLBACK EditContributionItem::ecProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	EditContributionItem* lpData = (EditContributionItem*)dwRefData;

	switch (uMsg)
	{
	case WM_KEYUP:
	{
		if (wParam == VK_RETURN)
		{
			if (uIdSubclass == lpData->GetControlId("amount") || uIdSubclass == lpData->GetControlId("memo") || uIdSubclass == lpData->GetControlId("fund"))
			{
				lpData->OnSaveContItem();
				break;
			}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}