/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Common/DlgCommon.h"
#include "FamilyInfoStruct.h"
#include "../Components/UserSearcher.h"


class FamilyDonationsList : public DlgCommon
{
public:
	UserSearcher* familySearcher;
	int currentMemberId;
	FamilyInfo* familyInfo;

	FamilyDonationsList(HWND hWnd, FamilyInfo* fi, int memId);
	~FamilyDonationsList();

	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);

	int ShowDlg();

	void UpdateTable(std::function<void(void)> onUpdated = nullptr);
	void OnDlgResized();


	void UpdateYearField(std::function<void(void)> callback = nullptr);

	void ShowBatchDesc(int batchId);
	
	void AddControls();
	void CreateTable();
private:
	TableCreater* table;

	std::map<int, std::wstring> fundsList;
	std::map<int, int> searchUserIndexes;
	std::map<std::wstring, int> fundsIds;
	std::map<std::wstring, int> memberIds;
	int currentYear;
	int currentFundId;

	static LRESULT CALLBACK MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

