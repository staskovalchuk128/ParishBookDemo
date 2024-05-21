#include "FamilyEnvAssigner.h"
#include <regex>
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

FamilyEnvAssigner::FamilyEnvAssigner(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	childCon = NULL;
	childScroll = NULL;

}

FamilyEnvAssigner::~FamilyEnvAssigner()
{
	delete childCon;
	delete childScroll;
	RemoveWindowSubclass(hWnd, FamilyEnvAssignerProc, IDC_STATIC);

}

void FamilyEnvAssigner::AddControls()
{
	SPData winSize = GetWinSize(hWnd);

	CreateBtn("btnAutoAssign", 10, 45, 200, 30, WS_CHILD | WS_VISIBLE, L"Auto assign");
	CreateBtn("btnClearAll", 10, 45, 200, 30, WS_CHILD | WS_VISIBLE, L"Clear all", WS_STICK_RIGHT);
	SetColor("btnClearAll", APP_COLORS::RED);

	CreateLine(10, 100, -10, 1, "line", APP_COLORS::GRAY_BORDER);

	HWND contentHwnd = CreateStatic("contentHwnd", 10, 110, -10, -80, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);

	childCon = new Controls(contentHwnd);
	childScroll = new ScrollBar(contentHwnd, hInst);
	childScroll->CreateScroll(21, -1);

	CreateLine(10, 70, -10, 1, "line", APP_COLORS::GRAY_BORDER, WS_STICK_BOTTOM);

	CreateBtn("btnSave", 10, 10, 200, 30, WS_CHILD | WS_VISIBLE, L"Save envelopes", WS_STICK_BOTTOM | WS_STICK_RIGHT);


	SetWindowSubclass(hWnd, FamilyEnvAssignerProc, IDC_STATIC, (DWORD_PTR)this);

	UpdateFamiliesTable();

}

void FamilyEnvAssigner::OnAutoAssign()
{
	int res = MessageDlg(hWnd, L"Confirmation",
		L"Are you sure you want to auto assign all the envelope numbers?",
		MD_YESNO, MD_SUCCESS).OpenDlg();
	if (res == IDNO) return;

	int index = 1;
	for (auto it : childCon->GetCurrentControls())
	{
		if (it->GetTypeId() == CONTROL_TYPES::EDIT)
		{
			SetWindowText(it->GetHWND(), (LPWSTR)std::to_wstring(index).c_str());
			index++;
		}
	}
}

void FamilyEnvAssigner::OnClearAll()
{
	int res = MessageDlg(hWnd, L"Confirmation",
		L"Are you sure you want to clear all the envelope numbers?",
		MD_YESNO, MD_QUESTION).OpenDlg();
	if (res == IDNO) return;

	for (auto it : childCon->GetCurrentControls())
	{
		if (it->GetTypeId() == CONTROL_TYPES::EDIT)
		{
			SetWindowText(it->GetHWND(), L"");
		}
	}
}

void FamilyEnvAssigner::SaveAll()
{
	int res = MessageDlg(hWnd, L"Confirmation",
		L"Are you sure you want to save this?",
		MD_YESNO, MD_QUESTION).OpenDlg();
	if (res == IDNO) return;

	std::vector<std::map<std::string, std::wstring>> requestParams = {};

	for (auto& it : childCon->GetCurrentControls())
	{
		if (it->GetTypeId() == CONTROL_TYPES::EDIT)
		{
			int familyId = it->GetLParam();
			std::wstring envId = GetWindowTextStr(it->GetHWND());

			requestParams.push_back({
				{ "familyId", std::to_wstring(familyId) },
				{ "envelope", envId },
			});
		}
	}

	PreloaderComponent::Start(hWnd);

	PBConnection::Request("saveEnvelopesForEnvAssigner", { { "data", CreateJsonFormVec(requestParams) } },
		[&](PBResponse res)
		{
			PreloaderComponent::Stop();
			MessageDlg(hWnd, L"Success",
				L"New envelope numbers are saved",
				MD_OK, MD_SUCCESS).OpenDlg();
		}
	);

	

}

void FamilyEnvAssigner::UpdateFamiliesTable()
{
	PreloaderComponent::Start(hWnd);

	PBConnection::Request("getFamiliesForEnvAssigner", {},
		[&](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResultVec("main");

			SPData winSize = GetWinSize(childCon->GetHWND());

			int columnIndex = 1;
			int rowIndex = 0;
			int itemY = 0;
			int itemX = 10;
			int itemsOffsetX = 10;
			int itemHeight = 25;
			int famNameWidth = 200;
			int envWidth = 40;
			int itemsOffset = 5;

			int itemsPerCol = (int)floor((float)(winSize.height - 60) / (float)(itemHeight + itemsOffset));

			int totalItemWidth = famNameWidth + envWidth + itemX * 2;

			int totalWidth = 0, totalHeight = 0;

			for (auto &it : result)
			{
				int familyId = GetInt(it["id"]);
				std::wstring familyName = it["familyName"];
				int familyEnvNum = GetInt(it["envelope"]);
				std::string staticName = "famname_" + std::to_string(familyId);
				std::string envName = "famenv_" + std::to_string(familyId);

				itemY = (itemHeight + itemsOffset) * rowIndex;

				itemX = (totalItemWidth + itemsOffsetX * 2) * (columnIndex - 1);

				int envX = itemX + famNameWidth + itemsOffsetX;

				childCon->CreateStatic(staticName, itemX, itemY, famNameWidth, itemHeight, WS_CHILD | WS_VISIBLE | SS_ENDELLIPSIS, familyName.c_str());
				childCon->CreateEdit(envName, envX, itemY, envWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_CENTER, std::to_wstring(familyEnvNum).c_str(), familyId);
				childCon->SetLParam(envName, familyId);
				static_cast<EditCtrl*> (childCon->GetControlPtr(envName))->SelectAllOnFocus(true);

				rowIndex++;

				if (rowIndex > itemsPerCol)
				{
					rowIndex = 0;
					columnIndex++;

					int lineX = (totalItemWidth + itemsOffsetX * 2) * (columnIndex - 1);
					lineX -= static_cast<int> (itemsOffsetX * 1.5f);
					std::string lineId = "line_" + std::to_string(columnIndex);

					childCon->CreateLine(lineX, 0, 1, -10, lineId.c_str(), APP_COLORS::GRAY_BORDER);
				}

				totalWidth = envX + envWidth;
				totalHeight = itemY + itemHeight > totalHeight ? itemY + itemHeight : totalHeight;
			}

			childScroll->SetVScrollRange(totalHeight + itemsOffsetX);
			childScroll->SetHScrollRange(totalWidth + itemsOffsetX);

			PreloaderComponent::Stop();

		}
	);

}

bool FamilyEnvAssigner::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("btnAutoAssign"))
		{
			OnAutoAssign();
			return true;
		}
		else if (wId == GetControlId("btnClearAll"))
		{
			OnClearAll();
			return true;
		}
		else if (wId == GetControlId("btnSave"))
		{
			SaveAll();
			return true;
		}

		break;
	}
	}

	return false;
}

LRESULT CALLBACK FamilyEnvAssigner::FamilyEnvAssignerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	FamilyEnvAssigner* lpData = (FamilyEnvAssigner*)dwRefData;

	switch (uMsg)
	{
	case WM_SIZE:
	{
		lpData->childScroll->ResetScroll(SB_HORZ);
		lpData->childScroll->ResetScroll(SB_VERT);
		return true;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}