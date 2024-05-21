#include "CertificatesHistoryDlg.h"
#include "CertCommon.h"
#include "..//..//PBConnection.h"
#include "..//..//Components/PreloaderComponent.h"

CertificatesHistoryDlg::CertificatesHistoryDlg(HWND hWnd, CertCommon* certPagePtr)
	: DlgCommon(hWnd), Reports(), certPagePtr(certPagePtr)
{
	certTypeStr = certPagePtr->GetCertificateTypeStr();
	tableFields = certPagePtr->GetFillTableFields();
	this->hWnd = hWnd;
	table = NULL;
}

CertificatesHistoryDlg::~CertificatesHistoryDlg() 
{
	delete table;
}

void CertificatesHistoryDlg::ShowDlg()
{
	OpenDlg(hWnd, "History");
}

void CertificatesHistoryDlg::OnDlgInit()
{
	CreateBtn("select", 160, 40, 160, 30, WS_CHILD | WS_VISIBLE, L"Fill from selected", WS_STICK_RIGHT);
	CreateBtn("delete", 10, 40, 140, 30, WS_CHILD | WS_VISIBLE, L"Delete selected", WS_STICK_RIGHT);
	SetColor("delete", APP_COLORS::RED);

	this->table = new TableCreater(hWnd, GetWindowInstance(hWnd), this);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	table->Create("contTable", 10, 90, screenWidth / 2, screenHeight / 2);

	std::vector<TableColumns> tColumns =
	{
		{ L"ID", 10, "id" }
	};
	for (auto &it: tableFields)
	{
		tColumns.push_back(
			{
				it.first, 20, it.second
			}
		);
	}

	table->CreateColumns(tColumns);

	table->InsertColumns();

	SetWindowSubclass(hWnd, Proc, IDC_STATIC, (DWORD_PTR)this);
	DoneLoadingDlg();
	table->ClearTable();

	UpdateTable([&]()
		{
		}
	);

	

}

void CertificatesHistoryDlg::OnDlgResized()
{
	table->ResetScrollSize();
}

void CertificatesHistoryDlg::UpdateTable(std::function<void(void)> onUpdatedCallback)
{

	PreloaderComponent::Start(hWnd);
	PBConnection::Request("getCertificatesPrintHistory",
		{
			{"type", certTypeStr}
		},
		[&, onUpdatedCallback](PBResponse res)
		{
			table->ClearTable();
			
			std::vector<TableColumns> columns = table->GetColums();
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();
			for (size_t i = 0; i < result.size(); i++)
			{
				table->InsertRow(GetInt(result[i]["id"]));

				for (size_t j = 0; j < columns.size(); j++)
				{
					table->InsertItem(result[i][columns[j].dbName], static_cast<int> (j));
				}
			}

			if (onUpdatedCallback != nullptr) onUpdatedCallback();
			PreloaderComponent::Stop();
		}
	);

}

bool CertificatesHistoryDlg::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (wId == ID_DELETE_TI)
	{
		int sId = table->GetSelectedRowId();
		DeleteSelected(sId);
		return true;
	}

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("select"))
		{
			int sId = table->GetSelectedRowId();
			if (sId <= 0)
			{
				MessageDlg(NULL, L"Error",
					L"Select table row",
					MD_OK, MD_ERR).OpenDlg();
				return true;
			}
			FillSelected(sId);
			return true;
		}
		else if (wId == GetControlId("delete"))
		{
			int sId = table->GetSelectedRowId();
			if (sId <= 0)
			{
				MessageDlg(NULL, L"Error",
					L"Select table row",
					MD_OK, MD_ERR).OpenDlg();
				return true;
			}
			DeleteSelected(sId);
			return true;
		}

		break;
	}

	case CBN_SELCHANGE:
	{
		if (wId == GetControlId("month"))
		{
			UpdateTable();
			return true;
		}

		break;
	}
	}


	return false;
}

void CertificatesHistoryDlg::FillSelected(int id)
{
	if (id <= 0) return;
	certPagePtr->FillFromHistory(id);
	SendMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
}

void CertificatesHistoryDlg::DeleteSelected(int id)
{
	int res = MessageDlg(NULL, L"Confirmation",
		L"Are you sure you want to delete this record?",
		MD_YESNO, MD_QUESTION).OpenDlg();
	if (res != IDYES) return;

	PreloaderComponent::Start(hWnd);
	PBConnection::Request("deleteCertificateFromHistory",
		{
			{"certId", std::to_wstring(id) }
		},
		[&](PBResponse res)
		{
			UpdateTable();
		}
	);
}

LRESULT CALLBACK CertificatesHistoryDlg::Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CertificatesHistoryDlg* lpData = (CertificatesHistoryDlg*)dwRefData;

	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_DBLCLK:
		{
			int sId = lpData->table->GetSelectedRowId();
			lpData->FillSelected(sId);
			return true;
		}
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