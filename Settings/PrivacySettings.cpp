#include "PrivacySettings.h"
#include "..//Components/PreloaderComponent.h"
#include "..//PBConnection.h"

PrivacySettings::PrivacySettings(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
}

PrivacySettings::~PrivacySettings() {}

void PrivacySettings::AddControls()
{
	CreateStatic("sTitle", 10, 30, -10, 40, WS_VISIBLE | WS_CHILD, L"Privacy Settings");
	SetFontSize("sTitle", 28);

	int itemY = 130;

	CreateStatic("sCurrentPassword", 20, itemY, -20, 20, WS_VISIBLE | WS_CHILD, L"Current password:");
	CreateEdit("currentPassword", 20, itemY += 25, -20, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_PASSWORD, L"");

	CreateStatic("sCurrentPassword", 20, itemY += 45, -20, 20, WS_VISIBLE | WS_CHILD, L"New password:");
	CreateEdit("newPassword", 20, itemY += 25, -20, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_PASSWORD, L"");

	CreateStatic("sRepeatNewPassword", 20, itemY += 45, -20, 20, WS_VISIBLE | WS_CHILD, L"Repeat new password:");
	CreateEdit("repeatNewPassword", 20, itemY += 25, -20, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_PASSWORD, L"");

	CreateBtn("save", 20, itemY += 45, 150, 30, WS_CHILD | WS_VISIBLE, L"Change password", 0);

	CreateGroupBox("sPasswordChange", 10, 90, -10, itemY - 15, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, L"Changing password", 0);

}

bool PrivacySettings::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("save"))
		{
			SaveSettings();
		}
	}

	return false;
}

void PrivacySettings::SaveSettings()
{
	std::wstring currentPassword = GetWindowTextStr(GetControlHWND("currentPassword"));
	std::wstring newPassword = GetWindowTextStr(GetControlHWND("newPassword"));
	std::wstring repeatNewPassword = GetWindowTextStr(GetControlHWND("repeatNewPassword"));

	if (newPassword != repeatNewPassword)
	{
		MessageDlg(hWnd, L"Error",
			L"The entered passwords do not match",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}
	if (newPassword.length() < 6)
	{
		MessageDlg(hWnd, L"Error",
			L"The password must be at least 6 characters long",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}

	PreloaderComponent::Start(hWnd);

	PBConnection::Request("saveNewPassword",
		{
			{"currentPassword", currentPassword},
			{"newPassword", newPassword},
			{"repeatNewPassword", repeatNewPassword}

		},
		[&](PBResponse res)
		{
			PreloaderComponent::Stop();

			MessageDlg(hWnd, L"Success",
				L"Password changed successfuly",
				MD_OK, MD_SUCCESS).OpenDlg();
		}
	);



}