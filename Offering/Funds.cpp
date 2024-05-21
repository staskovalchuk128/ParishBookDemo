#include "Funds.h"
#include "FundsStatsDlg.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

Funds::Funds(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	currentFundId = 0;
}

Funds::~Funds() {}

bool Funds::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("deleteFund"))
		{
			DeleteFund();
			return true;
		}
		else if (wId == GetControlId("addFund"))
		{
			SaveFund();
			return true;
		}
		else if (wId == GetControlId("editFund"))
		{
			if (currentFundId > 0)
			{
				CancelEditFund();
			}
			else
			{
				ShowSaveFund();
			}

			return true;
		}

		break;
	}
	}

	return false;
}

void Funds::ShowFunds(std::function<void(void)> onFinished)
{
	PreloaderComponent::Start(hWnd);
	PBConnection::Request("getFunds", {},
		[&](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> funds = res.GetResultVec("main");

			HWND fundsList = GetControlHWND("fundList");

			fundsNum.clear();

			SendMessage(fundsList, LB_RESETCONTENT, 0, 0);
			for (int i = 0; i < funds.size(); i++)
			{
				LRESULT fI = SendMessageW(fundsList, LB_ADDSTRING, 0, (LPARAM)(funds[i]["name"].c_str()));
				fundsNum[(unsigned int)fI] = GetInt(funds[i]["id"].c_str());
			}

			UpdateWindow(fundsList);
			PreloaderComponent::Stop();

			if (onFinished != nullptr)
			{

			}
		}
	);

}

void Funds::AddControls()
{
	SetListBoxItemHeight(30);

	HWND fundsList = CreateListBox("fundList", 10, 40, -350, -10, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED);

	CreateStatic("tFundName", 10, 40, 300, 20, WS_CHILD | WS_VISIBLE, L"Fund name:", WS_STICK_RIGHT);
	CreateEdit("fundInput", 10, 65, 300, 40, WS_CHILD | ES_MULTILINE | WS_VISIBLE | ES_AUTOHSCROLL, L"", WS_STICK_RIGHT);

	CreateBtn("addFund", 10, 120, 300, 40, WS_CHILD | WS_VISIBLE, L"Add New Fund", WS_STICK_RIGHT);

	CreateBtn("deleteFund", 10, 180, 140, 40, WS_CHILD | WS_VISIBLE, L"Delete Fund", WS_STICK_RIGHT);
	CreateBtn("editFund", 170, 180, 140, 40, WS_CHILD | WS_VISIBLE, L"Edit Fund", WS_STICK_RIGHT);

	ShowFunds();
	
	SetWindowSubclass(GetControlHWND("fundInput"), FundsProc, GetControlId("fundInput"), (DWORD_PTR)this);
	SetWindowSubclass(fundsList, FundsProc, GetControlId("fundList"), (DWORD_PTR)this);
}

void Funds::ShowSaveFund()
{
	HWND fundsList = GetControlHWND("fundList");
	HWND textBox = GetControlHWND("fundInput");

	if (fundsList)
	{
		LRESULT count = SendMessage(fundsList, LB_GETCOUNT, 0, 0);
		int iSelected = -1;
		for (int i = 0; i < count; i++)
		{
			if (SendMessage(fundsList, LB_GETSEL, i, 0) > 0)
			{
				iSelected = i;
				break;
			}
		}

		if (iSelected >= 0)
		{
			currentFundId = fundsNum[iSelected];
			if (currentFundId <= 0)
			{
				MessageDlg(hWnd, L"Error",
					L"Fund not found",
					MD_OK, MD_ERR).OpenDlg();
			}

			wchar_t chBuffer[MAX_PATH] = { 0 };

			LRESULT dwSel = SendDlgItemMessage(hWnd, GetControlId("fundList"), LB_GETCURSEL, 0, 0);

			if (dwSel != LB_ERR)
			{
				SendDlgItemMessage(hWnd, GetControlId("fundList"), LB_GETTEXT, dwSel, (LPARAM)(LPSTR)chBuffer);
			}

			SetWindowText(GetControlHWND("addFund"), L"Save Fund");
			SetWindowText(GetControlHWND("editFund"), L"Cancel");

			if (*chBuffer != NULL)
			{
				SetWindowText(textBox, chBuffer);
			}

			SetFocus(textBox);
		}
	}
}

void Funds::SaveFund()
{
	HWND textBox = GetControlHWND("fundInput");

	std::wstring fundName = GetWindowTextStr(textBox);

	if (fundName.length() <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Fund name cannot be empty",
			MD_OK, MD_ERR).OpenDlg();
	}
	else
	{
		PBConnection::Request("saveFund", { {"fundId", std::to_wstring(currentFundId) }, {"name", fundName} },
			[&](PBResponse res)
			{
				HWND textBox = GetControlHWND("fundInput");
				if (currentFundId)
				{
					SetWindowText(GetControlHWND("addFund"), L"Add New Fund");
					SetWindowText(GetControlHWND("editFund"), L"Edit Fund");
					currentFundId = 0;
				}
				ShowFunds();
				SetWindowText(textBox, L"");
			}
		);

	}
}

void Funds::CancelEditFund()
{
	HWND textBox = GetControlHWND("fundInput");
	SetWindowText(textBox, L"");
	currentFundId = 0;
	SetWindowText(GetControlHWND("addFund"), L"Add New Fund");
	SetWindowText(GetControlHWND("editFund"), L"Edit Fund");
}

bool Funds::DeleteFund()
{
	HWND fundsList = GetControlHWND("fundList");

	if (fundsList)
	{
		LRESULT count = SendMessage(fundsList, LB_GETCOUNT, 0, 0);
		int iSelected = -1;
		for (int i = 0; i < count; i++)
		{
			if (SendMessage(fundsList, LB_GETSEL, i, 0) > 0)
			{
				iSelected = i;
				break;
			}
		}

		if (iSelected >= 0)
		{
			std::wstring fundID = std::to_wstring(fundsNum[iSelected]);

			std::wstring agree = L"Yes";
			std::wstring promptPhrase = L"To delete this fund please enter: " + agree;

			MessageDlg msg = MessageDlg(hWnd, L"Confirmation",
				promptPhrase.c_str(),
				MD_OKCALNCEL, MD_PROMPT);
			msg.SetPromptStr(agree.c_str());
			int res = msg.OpenDlg();

			if (res != IDOK) return false;


			PBConnection::Request("deleteFund", { {"fundId", fundID } },
				[&](PBResponse res)
				{
					CancelEditFund();
					ShowFunds();
				}
			);
		}
	}

	return true;
}

void Funds::ShowDonationsByFundId(int fundId)
{
	wchar_t chBuffer[MAX_PATH] = { 0 };
	LRESULT dwSel = SendDlgItemMessage(hWnd, GetControlId("fundList"), LB_GETCURSEL, 0, 0);
	if (dwSel != LB_ERR)
	{
		SendDlgItemMessage(hWnd, GetControlId("fundList"), LB_GETTEXT, dwSel, (LPARAM)(LPSTR)chBuffer);
	}


	FundsStatsDlg dlg(hWnd, fundId, std::wstring(chBuffer));
	dlg.ShowDlg();
}

LRESULT CALLBACK Funds::FundsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	Funds* lpData = (Funds*)dwRefData;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		if (uIdSubclass == lpData->GetControlId("fundList"))
		{
			if (wParam == VK_DELETE)
			{
				lpData->DeleteFund();
				return true;
			}
		}
		else if (uIdSubclass == lpData->GetControlId("fundInput"))
		{
			if (wParam == VK_RETURN)
			{
				lpData->SaveFund();
				return true;
			}
		}

		break;
	}
	case WM_LBUTTONDBLCLK:
	{
		int index = (int)SendMessageW(hWnd, LB_GETCURSEL, 0, 0);
		if (index != LB_ERR)
		{
			lpData->ShowDonationsByFundId(lpData->fundsNum[index]);
		}
		break;
	}
	}
	
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}