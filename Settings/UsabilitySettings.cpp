#include "UsabilitySettings.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

UsabilitySettings::UsabilitySettings(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	currentBatchEdit = -1;
}

UsabilitySettings::~UsabilitySettings() {}

void UsabilitySettings::AddControls()
{
	PreloaderComponent::Start(hWnd);

	CreateStatic("sTitle", 10, 30, -10, 40, WS_VISIBLE | WS_CHILD, L"Usability Settings");
	SetFontSize("sTitle", 28);


	HWND batchNamesList = CreateListBox("batchNamesList", 35, 110, -50, 150, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED);

	SetWindowSubclass(batchNamesList, BatchesNamesProc, GetControlId("batchNamesList"), (DWORD_PTR)this);

	CreateBtn("addNewBatchName", 35, 260, 150, 30, WS_CHILD | WS_VISIBLE, L"Add new name", 0);

	CreateGroupBox("sBB", 10, 70, -20, 240, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, L"Batch names", 0);


	UpdateBatchNames();
}

bool UsabilitySettings::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (wId == ID_EDIT_TI)
	{
		int index = (int)SendMessageW(GetControlHWND("batchNamesList"), LB_GETCURSEL, 0, 0);

		wchar_t strName[_MAX_DIR];
		SendMessageW(GetControlHWND("batchNamesList"), LB_GETTEXT, index, (LPARAM)strName);

		OnEditBatchName(std::wstring(strName),  batchNamesNums[index]);
		return true;
	}
	else if (wId == ID_DELETE_TI)
	{
		int index = (int)SendMessageW(GetControlHWND("batchNamesList"), LB_GETCURSEL, 0, 0);
		DeleteBatchName(batchNamesNums[index]);
		return true;
	}


	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("addNewBatchName"))
		{
			OnAddBatchName();
			return true;
		}
	}
	}
	return false;
}

void UsabilitySettings::UpdateBatchNames(std::function<void(void)> callback)
{
	PreloaderComponent::Start(hWnd);


	PBConnection::Request("getUsabilitySettings", {},
		[&, callback](PBResponse res)
		{

			std::vector<std::map<std::string, std::wstring>> bNames = res.GetResultVec("batchNames");

			HWND batchNamesList = GetControlHWND("batchNamesList");

			batchNamesNums.clear();

			SendMessage(batchNamesList, LB_RESETCONTENT, 0, 0);
			for (int i = 0; i < bNames.size(); i++)
			{
				LRESULT fI = SendMessageW(batchNamesList, LB_ADDSTRING, 0, (LPARAM)(bNames[i]["name"].c_str()));
				batchNamesNums[(unsigned int)fI] = GetInt(bNames[i]["id"].c_str());
			}

			UpdateWindow(batchNamesList);

			PreloaderComponent::Stop();
			if (callback != nullptr) callback();
		}
	);



	
}

void UsabilitySettings::OnAddBatchName()
{
	std::wstring bnStr = L"";
	MessageDlg dlg = MessageDlg(hWnd, L"Enter batch name",
		L"Enter name",
		MD_OKCALNCEL, MD_TEXTBOX);
	dlg.SetTextBoxStr(bnStr);
	if (dlg.OpenDlg() == MD_OK)
	{
		SaveBatchName(bnStr);
	}
}
void UsabilitySettings::OnEditBatchName(std::wstring name, int id)
{
	std::wstring bnStr = name;
	MessageDlg dlg = MessageDlg(hWnd, L"Enter batch name",
		L"Enter name",
		MD_OKCALNCEL, MD_TEXTBOX);
	dlg.SetTextBoxStr(bnStr);
	if (dlg.OpenDlg() == MD_OK)
	{
		SaveBatchName(bnStr, id);
	}
}

void UsabilitySettings::SaveBatchName(std::wstring name, int id)
{
	if (name.size() == 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Enter name",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}

	PBConnection::Request("saveUsabilitySettingsBatchName",
		{ 
			{ "itemId", std::to_wstring(id)},
			{ "name", name},
		},
		[&](PBResponse res)
		{
			UpdateBatchNames();
		}
	);
}

void UsabilitySettings::DeleteBatchName(int id)
{
	PBConnection::Request("deleteUsabilitySettingsBatchName",
		{ { "itemId", std::to_wstring(id)}},
		[&](PBResponse res)
		{
			UpdateBatchNames();
		}
	);
}

LRESULT CALLBACK UsabilitySettings::BatchesNamesProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UsabilitySettings* lpData = (UsabilitySettings*)dwRefData;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		if (uIdSubclass == lpData->GetControlId("fundList"))
		{
			if (wParam == VK_DELETE)
			{
				int index = (int)SendMessageW(hWnd, LB_GETCURSEL, 0, 0);

				lpData->DeleteBatchName(index);
				return true;
			}
		}

		break;
	}
	case WM_RBUTTONDOWN:
	{
		int index = (int)SendMessageW(hWnd, LB_GETCURSEL, 0, 0);
		if (index != LB_ERR)
		{
			
			POINT cursor;
			GetCursorPos(&cursor);
			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_TABLEMENU));
			hMenu = GetSubMenu(hMenu, 0);
			TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, lpData->hWnd, NULL);
			return true;


		}
		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}