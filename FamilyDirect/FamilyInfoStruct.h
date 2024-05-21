/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "string"
#include "map"

struct FamilyInfo {

	int familyId = 0;
	int transferToFamId = 0;

	struct MemberSacramentItem 
	{
		int completed = 0;
		std::wstring date, prepYear, parish, celebrant;
	};

	struct MemberItem 
	{
		bool loaded = false;
		int id = 0;
		std::wstring prefix, firstName, nickName, middleName, maidenName, lastName, suffix, type, gender, birthDate, birthPlace, dateOfDeath, maritalStatus, pager, fax, email, homePhone, mobilePhone, workPhone, status, age;
		std::map<std::string, MemberSacramentItem> sacraments;
	};

	MemberItem* currentMember = NULL;

	std::map<int, MemberItem> members;

	bool loadedFamilyInfo = false;
	std::wstring firstNames, lastName, mailingName, informalName, formalS, informalS, group, email, primaryPhone, emergencyPhone, addressType, addressLine1, addressLine2, zip, city, state, country;
	int envelope = 0;

	MemberItem* GetMember(int memberId) 
	{
		return &members[memberId];
	}

	void SetCurrentMember(int memberId) 
	{
		currentMember = GetMember(memberId);
	}

	void RemoveCurrentMember() 
	{
		members[0] = MemberItem();
		currentMember = &members[0];
	}

	void RemoveMember(int id) 
	{
		members.erase(id);
	}

	void ClearAll() 
	{
		loadedFamilyInfo = false;
		familyId = 0;
		members.clear();
		delete currentMember;
	}
};