/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Components/CitySearcher.h"
#include "../Common/DlgCommon.h"


struct FamilyStruct
{

	struct FSitem 
	{
		std::wstring prefix, firstName, nickName, lastName, suffix, type, gender, birthDate, email;
	};


	int currentMemberId = 0;
	std::map<int, FSitem> members;

	std::wstring familyName, familyLastName, group, addressType, addressLine1, addressLine2, zip, city, state, country;

	FSitem GetCurrentMember() 
	{
		if (members.find(currentMemberId) == members.end()) 
		{
			MessageBox(NULL, L"Member not found", L"Warning", MB_OK);
		}
		return members[currentMemberId];
	}

	void RemoveMember(int id) 
	{
		members.erase(id);
	}

};

class AddNewFamily : public DlgCommon 
{
public:
	AddNewFamily(HWND hWnd, HINSTANCE hInst);
	~AddNewFamily();

	void UpdateFamilyStruct(bool strictError = true);

	void SaveForm();
	void AutoFillNames();
	void ClearForms();
	void OnAddAnotherMember();

	void SwitchMembersTable(bool show = true);
	void UpdateMembersTable();
	void AddControls();
	void ClearMemberFields();

	bool HandleEditMember();
	void ShowDeleteMemberDialog();
	void DeleteMember(int localMemberId);

	void FillCityInfo();


	void ShowDialog();


	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);
	bool OnBeforeDlgClose();

	void UpdateShowMembersBtnText();
	int GetCurrentFamilyId();

	std::string GetActionAfter();
protected:
	bool membersTableOpened = false;
private:
	int currentFamilyId;
	std::string actionAfter;
	int controlsMaxY;
	ScrollBar* scrollBar;
	Controls* localCon;
	TableCreater* membersTable;
	FamilyStruct family;

	static LRESULT CALLBACK addFamilyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};