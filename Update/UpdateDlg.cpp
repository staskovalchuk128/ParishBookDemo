#include "UpdateDlg.h"

UpdateDlg::UpdateDlg(HWND hWnd) : DlgCommon(hWnd)
{
	this->hWnd = hWnd;
}

UpdateDlg::~UpdateDlg() {}

void UpdateDlg::ShowDlg()
{
	OpenDlg(hWnd, "Updating Application");
}

void UpdateDlg::OnDlgInit()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	CreateStatic("ssf", 10, 20, screenWidth / 3, 20, WS_VISIBLE | WS_CHILD, L"New vesion of ParishBook is avalable.", 0);

	CreateStatic("warnMsg", 10, 60, -10, 50, WS_VISIBLE | WS_CHILD, L"IMPORTANT: Save all your work before updating or you will loose your usaved data.", 0);
	hErr = CreateStatic("errMsg", 10, 60, -10, 50, WS_CHILD, L"");
	SetColor("errMsg", APP_COLORS::RED);

	CreateBtn("btnCancel", 10, 150, 120, 30, WS_CHILD | WS_VISIBLE, L"Cancel", WS_STICK_RIGHT);
	SetColor("btnCancel", APP_COLORS::GRAY);

	CreateBtn("update", 150, 150, 120, 30, WS_CHILD | WS_VISIBLE, L"UPDATE NOW", WS_STICK_RIGHT);

	DoneLoadingDlg();
}

void UpdateDlg::OnDlgClose()
{
	Updater::abortDownloding = true;
	WaitForSingleObject(updatingThread, (DWORD)INFINITY);
	CloseHandle(updatingThread);
}

void UpdateDlg::StartUpdating()
{
	RemoveControl(GetControlHWND("update"));

	SetControls(this);

	RemoveControl(GetControlHWND("warnMsg"));
	progressBar = CreateCustom("progressBar", PROGRESS_CLASS, 10, 60, -30, 30, WS_CHILD, L"", 0);
	progressBarDesc = CreateStatic("sProgressDesc", 10, 110, -10, 20, WS_CHILD, L"Downloading...", 0);

	StartUpdate();
}

bool UpdateDlg::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("update"))
		{
			StartUpdating();
			return true;
		}
		else if (wId == GetControlId("btnCancel"))
		{
			EndDialog(hWnd, IDCANCEL);
			return true;
		}
	}

	return false;
}