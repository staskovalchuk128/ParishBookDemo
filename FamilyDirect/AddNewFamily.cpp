#include "AddNewFamily.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

AddNewFamily::~AddNewFamily()
{
	if (membersTable) delete membersTable;
	delete localCon;
}

AddNewFamily::AddNewFamily(HWND hw, HINSTANCE hI) : DlgCommon(hw)
{
	hWnd = hw;
	controlsMaxY = 0;
	currentFamilyId = 0;
	actionAfter = "";
	scrollBar = NULL;
	localCon = NULL;
	membersTable = NULL;
}

void AddNewFamily::AutoFillNames()
{
	try
	{
		UpdateFamilyStruct();
	}

	catch (std::wstring str) {}

	catch (const std::exception&) {}

	FamilyStruct::FSitem headMember, wifeMember;
	std::wstring email;

	for (auto it : family.members)
	{
		if (it.second.type == L"Head" || it.second.type == L"Husband") headMember = it.second;
		else if (it.second.type == L"Wife") wifeMember = it.second;

		if (email.length() == 0 && it.second.email.length() > 0)
		{
			email = it.second.email;
		}
	}

	bool isFullFamily = wifeMember.firstName.length() > 0;
	bool isSameLastName = isFullFamily && headMember.lastName == wifeMember.lastName;

	std::wstring headFullName = headMember.firstName + L" " + headMember.lastName;

	std::wstring fFistName;
	std::wstring fLastName;
	std::wstring fMailingName;
	std::wstring fInformalName;
	std::wstring fFormalSalutation;
	std::wstring fInFormalSalutation;

	fFistName = isFullFamily ? headMember.firstName + L" and " + wifeMember.firstName : headMember.firstName;
	fLastName = isFullFamily ? (isSameLastName ? headMember.lastName : headMember.lastName + L" and " + wifeMember.lastName) : headMember.lastName;
	fMailingName = isFullFamily ? headMember.prefix + L" and " + wifeMember.prefix + L" " + headFullName : headMember.prefix + L" " + headFullName;
	fInformalName = isFullFamily ? headMember.firstName + L" and " + wifeMember.firstName + L" " + headMember.lastName : headMember.firstName + L" " + headMember.lastName;
	fFormalSalutation = isFullFamily ? headMember.prefix + L" and " + wifeMember.prefix + L" " + headMember.lastName : headMember.prefix + L" " + headMember.lastName;
	fInFormalSalutation = fFistName;

	if (isFullFamily && !isSameLastName)
	{
		fMailingName = headMember.prefix + L" " + headMember.firstName + L" " + headMember.lastName + L" and " + wifeMember.prefix + L" " + wifeMember.firstName + L" " + wifeMember.lastName;
		fInformalName = headMember.firstName + L" " + headMember.lastName + L" and " + wifeMember.firstName + L" " + wifeMember.lastName;
		fFormalSalutation = headMember.prefix + L" " + headMember.lastName + L" and " + wifeMember.prefix + L" " + wifeMember.lastName;
	}

	SetWindowText(localCon->GetControlHWND("fLastName"), fLastName.c_str());
	SetWindowText(localCon->GetControlHWND("fFirstName"), fFistName.c_str());
	SetWindowText(localCon->GetControlHWND("fEmail"), email.c_str());

	SetWindowText(localCon->GetControlHWND("fMailingName"), fMailingName.c_str());
	SetWindowText(localCon->GetControlHWND("fInformalName"), fInformalName.c_str());
	SetWindowText(localCon->GetControlHWND("fFormalSalutation"), fFormalSalutation.c_str());
	SetWindowText(localCon->GetControlHWND("fInformalSalutation"), fInFormalSalutation.c_str());
}

void AddNewFamily::ClearForms()
{
	for (auto it : localCon->GetCurrentControls())
	{
		if (it->GetName() == "autoFillFamilyNames") continue;
		SetWindowText(it->GetHWND(), L"");
	}
}

int AddNewFamily::GetCurrentFamilyId()
{
	return currentFamilyId;
}

std::string AddNewFamily::GetActionAfter()
{
	return actionAfter;
}

void AddNewFamily::SaveForm()
{
	std::map<std::string, std::wstring > form;

	for (auto it : localCon->GetCurrentControls())
	{
		form[it->GetName()] = GetWindowTextStr(it->GetHWND());
	}

	UpdateFamilyStruct(false);

	family.familyLastName = form["fLastName"];

	if (family.members.size() == 0)
	{
		if (form["firstName"].length() == 0)
		{
			throw std::wstring(L"Enter first name");
		}

		if (form["lastName"].length() == 0)
		{
			throw std::wstring(L"Enter last name");
		}
	}

	if (form["addressLine1"].length() == 0)
	{
		throw std::wstring(L"Enter address");
	}

	if (form["zip"].length() == 0)
	{
		throw std::wstring(L"Enter Postal Code");
	}

	if (form["city"].length() == 0)
	{
		throw std::wstring(L"Enter city");
	}

	if (family.familyLastName.length() == 0)
	{
		throw std::wstring(L"Enter Family Last Name, you can also click 'Auto Fill Family Names' button to fill that automatically");
	}

	std::map<std::string, std::wstring> requestData = 
	{
		{ "fLastName", form["fLastName"] },
		{ "fFirstName", form["fFirstName"] },
		{ "fInformalSalutation", form["fInformalSalutation"] },
		{ "fFormalSalutation", form["fFormalSalutation"] },
		{ "fInformalName", form["fInformalName"]},
		{ "fZip", family.zip },
		{ "fState", family.state },
		{ "fCity", family.city },
		{ "fAddressType", family.addressType },
		{ "fAddressLine1", family.addressLine1 },
		{ "fAddressLine2", family.addressLine2 },
		{ "fGroup", family.group },
		{ "fEmail",  form["fEmail"] },
		{ "fMailingName", form["fMailingName"] }
	};

	std::wstring fullFamilyName = L"";

	bool isThereHead = false;

	for (auto it : family.members)
	{
		if (it.second.type == L"Head" || it.second.type == L"Husband")
		{
			isThereHead = true;
			break;
		}
	}

	std::vector<std::map<std::string, std::wstring>> membersData = {};

	for (int i = 0; i < family.members.size(); i++)
	{
		FamilyStruct::FSitem member = family.members[i];
		member.birthDate = ChangeDateFormat(member.birthDate);

		membersData.push_back(
			{
				{"lastName", member.lastName},
				{"firstName", member.firstName},
				{"familyLastName", family.familyLastName },
				{"email", member.email },
				{"type", member.type },
				{"gender", member.gender },
				{"prefix", member.prefix },
				{"birthDate", member.birthDate },
				{"suffix", member.suffix }
			}
		);


		if (member.type == L"Head" || member.type == L"Husband")
		{
			fullFamilyName = fullFamilyName.length() > 0 ?
				member.lastName + L", " + member.firstName + L" and " + fullFamilyName :
				member.lastName + L", " + member.firstName;
		}
		else if (member.type == L"Wife")
		{
			fullFamilyName = fullFamilyName.length() > 0 ?
				fullFamilyName + L" and " + member.firstName :
				member.firstName;

			if (isThereHead == false)
			{
				fullFamilyName = member.lastName + L", " + member.firstName;
			}
		}
	}

	fullFamilyName = fullFamilyName.length() == 0 ? family.members[0].lastName + L", " + family.members[0].firstName : fullFamilyName;
	
	requestData["fullFamilyName"] = fullFamilyName;
	requestData["membersData"] = CreateJsonFormVec(membersData);

	PreloaderComponent::Start(hWnd);

	PBConnection::Request("addNewFamily", requestData, [&](PBResponse res)
		{
			PreloaderComponent::Stop();

			currentFamilyId = GetInt(res.GetResultMap("main")["id"]);

			int msgSwitchBox = MessageDlg(hWnd, L"Success",
				L"Family created successfuly, switch dialog to edit the family?",
				MD_YESNO, MD_SUCCESS).OpenDlg();

			if (msgSwitchBox == IDYES)
			{
				actionAfter = "edit";
			}

			EndDialog(hWnd, IDCANCEL);
		}
	);
}

void AddNewFamily::UpdateFamilyStruct(bool strictError)
{
	std::map<std::string, std::wstring > form;

	for (auto it : localCon->GetCurrentControls())
	{
		form[it->GetName()] = GetWindowTextStr(it->GetHWND());
	}

	family.addressLine1 = form["addressLine1"];
	family.addressLine2 = form["addressLine2"];
	family.group = form["familyGroup"];
	family.city = form["city"];
	family.state = form["state"];
	family.country = form["country"];
	family.zip = form["zip"];

	std::wstring firstName(form["firstName"]), lastName(form["lastName"]), type(form["type"]);

	if (strictError)
	{
		if (firstName.length() == 0)
		{
			throw std::wstring(L"Enter First Name");
		}

		if (lastName.length() == 0)
		{
			throw std::wstring(L"Enter Last Name");
		}
	}
	else
	{
		if (firstName.length() == 0 || lastName.length() == 0)
		{
			goto done;
		}
	}

	family.members[family.currentMemberId].birthDate = form["birthDate"];
	family.members[family.currentMemberId].email = form["email"];
	family.members[family.currentMemberId].firstName = form["firstName"];
	family.members[family.currentMemberId].gender = form["gender"];
	family.members[family.currentMemberId].lastName = form["lastName"];
	family.members[family.currentMemberId].nickName = form["nickName"];
	family.members[family.currentMemberId].prefix = form["prefix"];
	family.members[family.currentMemberId].suffix = form["suffix"];
	family.members[family.currentMemberId].type = form["type"];

done:
	bool done = true;
}

void AddNewFamily::ClearMemberFields()
{
	LockWindowUpdate(hWnd);

	std::vector<std::string> fieldToClear = { "birthDate", "email", "firstName", "lastName", "nickName", "suffix" };

	for (int i = 0; i < fieldToClear.size(); i++)
	{
		SetWindowText(localCon->GetControlHWND(fieldToClear[i]), L"");
	}

	SendMessage(localCon->GetControlHWND("type"), CB_SELECTSTRING, 0, (LPARAM)L"Head");
	SendMessage(localCon->GetControlHWND("gender"), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	SendMessage(localCon->GetControlHWND("prefix"), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	LockWindowUpdate(NULL);

}

void AddNewFamily::OnAddAnotherMember()
{
	LockWindowUpdate(hWnd);

	try
	{
		UpdateFamilyStruct();

		family.currentMemberId = static_cast<int> (family.members.size());

		ClearMemberFields();

		SetWindowText(localCon->GetControlHWND("addAnotherMember"), L"Add another member");

		if (membersTableOpened == true) UpdateMembersTable();

		SetFocus(localCon->GetControlHWND("firstName"));
	}

	catch (std::wstring str)
	{
		MessageDlg(hWnd, L"Error",
			str.c_str(),
			MD_OK, MD_ERR).OpenDlg();
	}

	catch (std::exception& e)
	{
		MessageDlg(hWnd, L"Error",
			StrToWStr(e.what()).c_str(),
			MD_OK, MD_ERR).OpenDlg();
	}

	LockWindowUpdate(NULL);

	UpdateShowMembersBtnText();
}

void AddNewFamily::UpdateMembersTable()
{
	membersTable->ClearTable();

	for (auto it : family.members)
	{
		membersTable->InsertRow(it.first);
		membersTable->InsertItem(std::to_wstring(it.first + 1).c_str(), 0);
		membersTable->InsertItem((it.second.firstName + L" " + it.second.lastName).c_str(), 1);
		membersTable->InsertItem(it.second.type.c_str(), 2);
		membersTable->InsertItem(it.second.gender.c_str(), 3);
	}
}

void AddNewFamily::UpdateShowMembersBtnText()
{
	std::wstring btnText = L"Show Members";
	if (membersTableOpened)
	{
		btnText = L"Hide Members";
	}

	int membersCount = (int)family.members.size();
	btnText += L" (" + std::to_wstring(membersCount) + L")";

	SetWindowText(localCon->GetControlHWND("showMembers"), btnText.c_str());
}

void AddNewFamily::SwitchMembersTable(bool show)
{
	if (show == true)
	{
		membersTable = new TableCreater(localCon->GetHWND(), hInst, localCon);

		membersTable->Create("membersTable", 10, controlsMaxY + 20, -10, 300);

		SetWindowSubclass(localCon->GetControlHWND("membersTable"), addFamilyProc, localCon->GetControlId("membersTable"), (DWORD_PTR)this);

		membersTable->InsertColumn(0, L"#", 10);
		membersTable->InsertColumn(1, L"Member name", 30);
		membersTable->InsertColumn(2, L"Type", 20);
		membersTable->InsertColumn(3, L"Gender", 20);

		UpdateMembersTable();

		membersTableOpened = true;
	}
	else
	{
		RemoveWindowSubclass(localCon->GetControlHWND("membersTable"), addFamilyProc, localCon->GetControlId("membersTable"));

		delete membersTable;
		membersTable = NULL;
		membersTableOpened = false;
	}

	UpdateShowMembersBtnText();

	scrollBar->SetVScrollRange(localCon->GetTotalParentHeight());

}

void AddNewFamily::FillCityInfo()
{
	std::wstring zip = GetWindowTextStr(localCon->GetControlHWND("zip"));

	if (zip.length() > 0)
	{
		CitySearcher::GetInfoByZip(zip, [&](PBResponse res)
			{
				std::map<std::string, std::wstring> cityData = res.GetResultMap("main");
				if (cityData.size() > 0)
				{
					SetWindowText(localCon->GetControlHWND("city"), (cityData["city"]).c_str());
					SetWindowText(localCon->GetControlHWND("state"), (cityData["state"]).c_str());
					SetWindowText(localCon->GetControlHWND("country"), (cityData["country"]).c_str());
				}
				else
				{
					MessageDlg(hWnd, L"Error",
						L"Information of city not found, check if Postal Code is correct",
						MD_OK, MD_ERR).OpenDlg();
				}

			}
		);
	}
	else
	{
		MessageDlg(hWnd, L"Error",
			L"Enter Postal Code",
			MD_OK, MD_ERR).OpenDlg();
	}
}

void AddNewFamily::AddControls()
{
	int editHeight = 30;
	int fStaticWIdth = 120, sStaticWidth = 160, tStaticWidth = 120;
	int editsWIdth = 200;
	int xOffsetBetweenES = 10;
	int xOffsetBetweenCols = 50;

	int leftX = fStaticWIdth + xOffsetBetweenES, nLeftX = 0,
		nCenterX = nLeftX + leftX + xOffsetBetweenCols + editsWIdth, centerX = nCenterX + sStaticWidth + xOffsetBetweenES,
		nRightX = centerX + xOffsetBetweenCols + editsWIdth, rightX = nRightX + tStaticWidth + xOffsetBetweenES;

	/*LEFT */

	int leftY = 10;
	int lyMarging = 40;

	HWND prefix = localCon->CreateCombobox("prefix", leftX, leftY, editsWIdth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP | CBS_OWNERDRAWFIXED);
	HWND firstName = localCon->CreateEdit("firstName", leftX, leftY += lyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("nickName", leftX, leftY += lyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("lastName", leftX, leftY += lyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("suffix", leftX, leftY += lyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	HWND type = localCon->CreateCombobox("type", leftX, leftY += lyMarging, editsWIdth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP | CBS_OWNERDRAWFIXED);
	HWND gender = localCon->CreateCombobox("gender", leftX, leftY += lyMarging, editsWIdth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP | CBS_OWNERDRAWFIXED);
	localCon->CreateEdit("birthDate", leftX, leftY += lyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->SetEditMask("birthDate", ES_DATE_MASK);
	localCon->CreateEdit("email", leftX, leftY += lyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);

	leftY = 15;
	localCon->CreateStatic("sprefix", nLeftX, leftY, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Prefix:");
	localCon->CreateStatic("sfirstName", nLeftX, leftY += lyMarging, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"First Name:");
	localCon->CreateStatic("snickName", nLeftX, leftY += lyMarging, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Nick Name:");
	localCon->CreateStatic("slastName", nLeftX, leftY += lyMarging, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Last Name:");
	localCon->CreateStatic("ssuffix", nLeftX, leftY += lyMarging, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Suffix:");
	localCon->CreateStatic("stype", nLeftX, leftY += lyMarging, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Type:");
	localCon->CreateStatic("sgender", nLeftX, leftY += lyMarging, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Gender:");
	localCon->CreateStatic("sbirthDate", nLeftX, leftY += lyMarging, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Birth date:");
	localCon->CreateStatic("semail", nLeftX, leftY += lyMarging, fStaticWIdth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"E-Mail Address:");

	std::vector<std::wstring > prefixListItems = { L"Mr.", L"Mrs.", L"Ms", L"", L"Br.", L"Brother", L"Capt.", L"CDR", L"Col.", L"DealocalCon", L"Dr.", L"Fr.", L"Gen.", L"Judge", L"LTC", L"Master", L"Miss", L"Most Rev", L"Most Rev.", L"Ms.", L"Rev.", L"Rev. Mr.", L"Rev. Msgr.", L"Sr.", L"Sra.", L"Unknown", L"Very Rev." };

	std::vector<std::wstring > TypeListItems = { L"Adult", L"Daughter", L"Father", L"Foster Daughter", L"Foster Son", L"Granddaughter", L"Grandfather", L"Grandmother", L"Grandson", L"Head", L"Husband", L"Mother", L"Son", L"Unknown", L"Wife" };

	std::vector<std::wstring > GenderListItems = { L"Male", L"Female" };

	for (int j = 0; j < prefixListItems.size(); j++)
	{
		SendMessage(prefix, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)prefixListItems[j].c_str());
	}

	SendMessage(prefix, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	for (int j = 0; j < TypeListItems.size(); j++)
	{
		SendMessage(type, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TypeListItems[j].c_str());
	}

	SendMessage(type, CB_SELECTSTRING, 0, (LPARAM)L"Head");

	for (int j = 0; j < GenderListItems.size(); j++)
	{
		SendMessage(gender, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)GenderListItems[j].c_str());
	}

	SendMessage(gender, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	/*CENTER*/

	int centerY = 10;
	int cyMarging = 40;

	HWND fg = localCon->CreateCombobox("familyGroup", centerX, centerY, editsWIdth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP | CBS_OWNERDRAWFIXED);
	localCon->CreateBtn("autoFillFamilyNames", centerX, centerY += cyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | ES_CENTER, L"Auto Fill Names");
	localCon->CreateEdit("fLastName", centerX, centerY += cyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("fFirstName", centerX, centerY += cyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("fMailingName", centerX, centerY += cyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("fInformalName", centerX, centerY += cyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("fFormalSalutation", centerX, centerY += cyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("fInformalSalutation", centerX, centerY += cyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("fEmail", centerX, centerY += cyMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);

	std::vector<std::wstring> fgListItems = { L"Active", L"Contributor Only", L"Deceased", L"Inactive", L"Moved" };

	for (int j = 0; j < fgListItems.size(); j++)
	{
		SendMessage(fg, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)fgListItems[j].c_str());
	}

	SendMessage(fg, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	centerY = 15;
	localCon->CreateStatic("sfamilyGroup", nCenterX, centerY, sStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Family Group:");
	localCon->CreateStatic("sfLastName", nCenterX, centerY += (cyMarging * 2), sStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Last Name:");
	localCon->CreateStatic("sfFirstName", nCenterX, centerY += cyMarging, sStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"First Name(s):");
	localCon->CreateStatic("sfMailingName", nCenterX, centerY += cyMarging, sStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Formal Mailing Name:");
	localCon->CreateStatic("sfInformalName", nCenterX, centerY += cyMarging, sStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Informal Name:");
	localCon->CreateStatic("sfFormalSalutation", nCenterX, centerY += cyMarging, sStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Formal Salutation:");
	localCon->CreateStatic("sfInformalSalutation", nCenterX, centerY += cyMarging, sStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Informal Salutation:");
	localCon->CreateStatic("sfEmail", nCenterX, centerY += cyMarging, sStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"E-Mail Address:");

	/*RIGHT*/

	int rightY = 10;
	int ryMarging = 40;


	HWND at = localCon->CreateCombobox("addressType", rightX, rightY, editsWIdth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP | CBS_OWNERDRAWFIXED);
	localCon->CreateEdit("addressLine1", rightX, rightY += ryMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("addressLine2", rightX, rightY += ryMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("aFirstName", rightX, rightY += ryMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("zip", rightX, rightY += ryMarging, 80, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_NUMBER);
	localCon->CreateBtn("btnFindCity", rightX + 90, rightY, 110, editHeight, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Find City");
	localCon->SetButtonIcon("btnFindCity", IDB_SEARCH);

	localCon->CreateEdit("city", rightX, rightY += ryMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("state", rightX, rightY += ryMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);
	localCon->CreateEdit("country", rightX, rightY += ryMarging, editsWIdth, editHeight, WS_VISIBLE | WS_CHILD | WS_TABSTOP);

	std::vector<std::wstring> faListItems = { L"Home", L"Mailing", L"Other" };

	for (int j = 0; j < faListItems.size(); j++)
	{
		SendMessage(at, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)faListItems[j].c_str());
	}

	SendMessage(at, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	rightY = 15;

	localCon->CreateStatic("saddressType", nRightX, rightY, tStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Address Type:");
	localCon->CreateStatic("saddressLine1", nRightX, rightY += ryMarging, tStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Address Line 1:");
	localCon->CreateStatic("saddressLine2", nRightX, rightY += ryMarging, tStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Address Line 2:");
	localCon->CreateStatic("saFirstName", nRightX, rightY += ryMarging, tStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"First Name(s):");
	localCon->CreateStatic("szip", nRightX, rightY += ryMarging, tStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Postal Code:");
	localCon->CreateStatic("scity", nRightX, rightY += ryMarging, tStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"City:");
	localCon->CreateStatic("sstate", nRightX, rightY += ryMarging, tStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"State/Region:");
	localCon->CreateStatic("scountry", nRightX, rightY += ryMarging, tStaticWidth, editHeight, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Country:");

	/*END*/

	localCon->CreateBtn("addAnotherMember", 10, leftY + 50, 210, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Add another member");
	localCon->SetButtonIcon("addAnotherMember", IDB_ADD);

	localCon->CreateBtn("showMembers", 250, leftY + 50, 230, 30, WS_CHILD | WS_VISIBLE, L"Show members");

	controlsMaxY = leftY + 80;

	SetFocus(localCon->GetControlHWND("firstName"));

}

bool AddNewFamily::HandleEditMember()
{
	LockWindowUpdate(hWnd);

	int memberLocalId = membersTable->GetSelectedRowId();

	std::wstring lnBuff = GetWindowTextStr(localCon->GetControlHWND("firstName"));
	std::wstring fnBuff = GetWindowTextStr(localCon->GetControlHWND("lastName"));

	if (lnBuff.length() > 0 || fnBuff.length() > 0)
	{
		int msgboxID = MessageDlg(hWnd, L"Confirmation",
			L"You are currently working with another member, the information you entered will be lost, are you sure you want to continue?",
			MD_YESNO, MD_QUESTION).OpenDlg();
		if (msgboxID == IDNO) return false;
	}

	family.currentMemberId = memberLocalId;

	FamilyStruct::FSitem memberItem = family.GetCurrentMember();

	SetWindowText(localCon->GetControlHWND("birthDate"), memberItem.birthDate.c_str());
	SetWindowText(localCon->GetControlHWND("email"), memberItem.email.c_str());
	SetWindowText(localCon->GetControlHWND("firstName"), memberItem.firstName.c_str());
	SetWindowText(localCon->GetControlHWND("gender"), memberItem.gender.c_str());
	SetWindowText(localCon->GetControlHWND("lastName"), memberItem.lastName.c_str());
	SetWindowText(localCon->GetControlHWND("nickName"), memberItem.nickName.c_str());
	SetWindowText(localCon->GetControlHWND("prefix"), memberItem.prefix.c_str());
	SetWindowText(localCon->GetControlHWND("suffix"), memberItem.suffix.c_str());
	SetWindowText(localCon->GetControlHWND("type"), memberItem.type.c_str());
	SetWindowText(localCon->GetControlHWND("addAnotherMember"), L"Save Member");

	SendMessage(localCon->GetControlHWND("type"), CB_SELECTSTRING, 0, (LPARAM)memberItem.type.c_str());
	SendMessage(localCon->GetControlHWND("gender"), CB_SELECTSTRING, 0, (LPARAM)memberItem.gender.c_str());
	SendMessage(localCon->GetControlHWND("prefix"), CB_SELECTSTRING, 0, (LPARAM)memberItem.prefix.c_str());

	LockWindowUpdate(NULL);

	return true;
}

void AddNewFamily::DeleteMember(int localMemberId)
{
	family.RemoveMember(localMemberId);
	UpdateMembersTable();
	ClearMemberFields();
}

void AddNewFamily::ShowDeleteMemberDialog()
{
	int index = -1;
	LV_ITEM lvi = { 0 };

	index = ListView_GetNextItem(membersTable->GetHWND(),
		-1, LVNI_SELECTED);
	if (index < 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Select which member you want to delete",
			MD_OK, MD_ERR).OpenDlg();
	}
	else
	{
		int msgboxID = MessageDlg(hWnd, L"Confirmation",
			L"Are you sure you want to delete this Member?",
			MD_YESNO, MD_QUESTION).OpenDlg();

		if (msgboxID == IDYES)
		{
			wchar_t idBuf[10] = { 0 };

			ListView_GetItemText(membersTable->GetHWND(), index, 0, idBuf, 10);

			int localMemberId = *idBuf != NULL ? GetInt(idBuf) : 0;
			if (localMemberId >= 0) localMemberId--;

			DeleteMember(localMemberId);
			UpdateShowMembersBtnText();
		}
	}
}

void AddNewFamily::ShowDialog()
{
	OpenDlg(hWnd, "Adding New Family");
}

void AddNewFamily::OnDlgInit()
{
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int bodyOffsetTop = 10;
	int bodyWidth = 900;
	int bodyHeight = screenHeight / 2;

	HWND body = CreateStatic("efBody", 10, bodyOffsetTop, bodyWidth, bodyHeight, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	SetWindowSubclass(body, addFamilyProc, GetControlId("efBody"), (DWORD_PTR)this);

	scrollBar = new ScrollBar(body, NULL);
	scrollBar->CreateScroll(21, -1);

	CreateBtn("btnClose", 10, bodyHeight + bodyOffsetTop + 30, 120, 30, WS_CHILD | WS_VISIBLE, L"Close", WS_STICK_RIGHT);
	SetColor("btnClose", APP_COLORS::GRAY);

	CreateBtn("btnSave", 150, bodyHeight + bodyOffsetTop + 30, 120, 30, WS_CHILD | WS_VISIBLE, L"Add Family", WS_STICK_RIGHT);

	localCon = new Controls(body);

	AddControls();
	UpdateShowMembersBtnText();

	GetControlPtr("efBody")->SetWidth(localCon->GetTotalParentWidth() + 30);	// + extra room for the scroll
	
	DoneLoadingDlg();
}

bool AddNewFamily::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (wId == ID_EDIT_TI)
	{
		HandleEditMember();
		return true;
	}
	else if (wId == ID_DELETE_TI)
	{
		ShowDeleteMemberDialog();
		return true;
	}

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("btnSave"))
		{
			try
			{
				SaveForm();
			}

			catch (std::wstring str)
			{
				MessageDlg(hWnd, L"Error",
					str.c_str(),
					MD_OK, MD_ERR).OpenDlg();
			}

			catch (std::exception& e)
			{
				MessageDlg(hWnd, L"Error",
					StrToWStr(e.what()).c_str(),
					MD_OK, MD_ERR).OpenDlg();
			}

			return true;
		}
		else if (wId == GetControlId("btnClose"))
		{
			SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), NULL);
			return true;
		}
		else if (wId == localCon->GetControlId("btnFindCity"))
		{
			FillCityInfo();
			return true;
		}
		else if (wId == localCon->GetControlId("addAnotherMember"))
		{
			OnAddAnotherMember();
			return true;
		}
		else if (wId == localCon->GetControlId("showMembers"))
		{
			SwitchMembersTable(membersTableOpened == false ? true : false);
			return true;
		}
		else if (wId == localCon->GetControlId("autoFillFamilyNames"))
		{
			AutoFillNames();
			return true;
		}
	}

	return false;
}

bool AddNewFamily::OnBeforeDlgClose()
{
	std::wstring firstName = GetWindowTextStr(localCon->GetControlHWND("firstName"));
	std::wstring lastName = GetWindowTextStr(localCon->GetControlHWND("lastName"));
	if (family.members.size() > 0 || firstName.length() > 0 || lastName.length() > 0)
	{
		int msgboxID = MessageDlg(hWnd, L"Confirmation",
			L"You have some family information entered, if you close the window the information will NOT be saved, you want to continue?",
			MD_YESNO, MD_QUESTION).OpenDlg();
		if (msgboxID == IDNO) return true;
	}

	return false;
}

LRESULT CALLBACK AddNewFamily::addFamilyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	AddNewFamily* lpData = (AddNewFamily*)dwRefData;

	switch (uMsg)
	{
	case WM_SETFOCUS:
	{
		SetFocus(GetNextDlgTabItem(GetParent((HWND)wParam), (HWND)wParam, FALSE));
		return true;
	}

	case WM_COMMAND:
	{
		lpData->OnDlgCommand(wParam);
		break;
	}

	case WM_KEYDOWN:
	{
		if (uIdSubclass == lpData->localCon->GetControlId("membersTable"))
		{
			if (wParam == VK_DELETE)
			{
				lpData->ShowDeleteMemberDialog();
				return true;
			}
		}

		break;
	}

	case WM_LBUTTONDBLCLK:
	{
		if (lpData->membersTableOpened == true && uIdSubclass == lpData->localCon->GetControlId("membersTable"))
		{
			lpData->HandleEditMember();
			return true;
		}

		break;
	}

	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_RCLICK:
		{
			int index = 0;
			LV_ITEM lvi = { 0 };

			index = ListView_GetNextItem(lpData->membersTable->GetHWND(),
				-1, LVNI_SELECTED);
			if (index < 0) return true;

			POINT cursor;
			GetCursorPos(&cursor);
			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_TABLEMENU));
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