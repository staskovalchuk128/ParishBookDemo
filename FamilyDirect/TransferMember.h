/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Common/DlgCommon.h"
#include "FamilyInfoStruct.h"
#include "../Components/UserSearcher.h"

#define TMM_SWITCHFAMILY 128

class TransferMember : public DlgCommon 
{
public:


	TransferMember(HWND hWnd, FamilyInfo* fi, int memId);
	~TransferMember();

	bool SearchFamily();

	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);

	int ShowDlg();


	void OnTransferMember();
	void OnTransferMember(int familyId, bool newFamily, std::function<void(int)> callback);
private:
	std::map<int, int> searchUserIndexes;
	UserSearcher* familySearcher;
	int currentMemberId;
	FamilyInfo* familyInfo;

	static LRESULT CALLBACK ecProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};