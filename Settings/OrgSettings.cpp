#include "OrgSettings.h"
#include "..//Components/PreloaderComponent.h"
#include "..//PBConnection.h"

OrgSettings::OrgSettings(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
}

OrgSettings::~OrgSettings() {}

void OrgSettings::AddControls()
{
	PreloaderComponent::Start(hWnd);

	CreateStatic("sTitle", 10, 30, -10, 40, WS_VISIBLE | WS_CHILD, L"Organization Settings");
	SetFontSize("sTitle", 28);

	CreateStatic("sbackupPath", 10, 90, -10, 20, WS_VISIBLE | WS_CHILD, L"Organization Name:");
	CreateEdit("orgName", 10, 115, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");

	RECT parentRc = GetWinRc(hWnd);
	int parentWidth = parentRc.right - parentRc.left;

	int itemsOffsetX = parentWidth / 100 * 2;
	int addressWidth = parentWidth / 100 * 48;
	int cityWidth = parentWidth / 100 * 20;
	int stateWidth = parentWidth / 100 * 15;
	int zipWidth = parentWidth / 100 * 15;

	int addressItemX = 10;

	CreateStatic("sorgAdress", addressItemX, 160, addressWidth, 20, WS_VISIBLE | WS_CHILD, L"Address:");
	CreateEdit("orgAddress", addressItemX, 185, addressWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");

	addressItemX += addressWidth + itemsOffsetX;

	CreateStatic("sorgCity", addressItemX, 160, cityWidth, 20, WS_VISIBLE | WS_CHILD, L"City:");
	CreateEdit("orgCity", addressItemX, 185, cityWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");

	addressItemX += cityWidth + itemsOffsetX;

	CreateStatic("sorgState", addressItemX, 160, stateWidth, 20, WS_VISIBLE | WS_CHILD, L"State:");
	CreateEdit("orgState", addressItemX, 185, stateWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");

	addressItemX += stateWidth + itemsOffsetX;

	CreateStatic("sorgZip", addressItemX, 160, zipWidth, 20, WS_VISIBLE | WS_CHILD, L"ZIP");
	CreateEdit("orgZip", addressItemX, 185, zipWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");

	CreateBtn("save", 10, 230, 150, 30, WS_CHILD | WS_VISIBLE, L"Save Settings", 0);


	PBConnection::Request("getOrgInfo", {}, std::bind(&OrgSettings::OnOrgDataLoaded, this, std::placeholders::_1));

}

void OrgSettings::OnOrgDataLoaded(PBResponse res)
{
	PreloaderComponent::Stop();
	std::map<std::string, std::wstring> orgData = res.GetResultMap("main");

	if (orgData.size() > 0)
	{
		std::wstring orgName = orgData["name"], orgAddress = orgData["address"], 
			orgCity = orgData["city"], orgState = orgData["state"], orgZip = orgData["zip"];


		SetWindowText(GetControlHWND("orgName"), orgName.c_str());
		SetWindowText(GetControlHWND("orgAddress"), orgAddress.c_str());
		SetWindowText(GetControlHWND("orgCity"), orgCity.c_str());
		SetWindowText(GetControlHWND("orgState"), orgState.c_str());
		SetWindowText(GetControlHWND("orgZip"), orgZip.c_str());

	}

}

bool OrgSettings::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("save"))
		{
			SaveOrgSettings();
		}
	}

	return false;
}

void OrgSettings::SaveOrgSettings()
{
	std::wstring orgName = GetWindowTextStr(GetControlHWND("orgName"));
	std::wstring orgAddress = GetWindowTextStr(GetControlHWND("orgAddress"));
	std::wstring orgCity = GetWindowTextStr(GetControlHWND("orgCity"));
	std::wstring orgState = GetWindowTextStr(GetControlHWND("orgState"));
	std::wstring orgZip = GetWindowTextStr(GetControlHWND("orgZip"));

	PreloaderComponent::Start(hWnd);
	
	PBConnection::Request("saveOrgInfo",
		{ 
			{"orgName", orgName},
			{"orgAddress", orgAddress},
			{"orgCity", orgCity},
			{"orgState", orgState},
			{"orgZip", orgZip}
		
		},
		[&](PBResponse res)
		{
			PreloaderComponent::Stop();

			MessageDlg(hWnd, L"Success",
				L"Settings saved successfuly",
				MD_OK, MD_SUCCESS).OpenDlg();
		}
	);



}