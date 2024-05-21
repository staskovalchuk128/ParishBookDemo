#include "FamReport.h"
#include "..//..//PBConnection.h"

FamReport::FamReport(HWND hWnd) : DlgCommon(hWnd), Reports()
{
	this->hWnd = hWnd;
	table = NULL;
	reportTypes = { L"Boys", L"Girls", L"Total Children", L"Males", L"Females", L"Single Households With Children", L"Single Households Without Children", 
		L"Total Active Members", L"Total Inactive Members", L"Total Deceased Members", L"Total Unknown Members",
		L"Total Active Families", L"Total Inactive Families", L"Total Deceased Families", L"Total Contributor Only Families", L"Total Moved Families" };

	currentOpenedItemId = 0;
	currentLookupItemType = "family";
	currentReportType = L"";
}

FamReport::~FamReport() {}

void FamReport::OnOpenMember(std::string type)
{
	EditFamily* editFamily = new EditFamily(NULL, NULL);
	editFamily->Init(type, currentOpenedItemId);
	// Dialog....

	//When dialog closes
	delete editFamily;

	UpdateTable();
}

bool FamReport::HandleOpenMember()
{
	int itemId = table->GetSelectedRowId();

	if (itemId <= 0)
	{
		MessageBox(NULL, L"Select which Family or Member you want to edit", L"Error", MB_OK);
		return false;
	}

	currentOpenedItemId = itemId;

	OnOpenMember(currentLookupItemType);

	return true;
}

int FamReport::GetRTInd(std::wstring type)
{
	auto it = find(reportTypes.begin(), reportTypes.end(), type);
	if (it != reportTypes.end())
	{
		return static_cast<int> (it - reportTypes.begin());
	}

	return -1;
}

void FamReport::ShowDlg()
{
	OpenDlg(hWnd, "Parish Report");
}

void FamReport::OnDlgInit()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	table = new TableCreater(hWnd, NULL, this);
	table->Create("reportsTable", 10, 60, screenWidth / 2, screenHeight / 2);
	//	table->scClass = reinterpret_cast<PagesCommon*>(this);

	HWND rs = CreateCombobox("reportTypeSelect", 10, 20, 300, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP | CBS_OWNERDRAWFIXED);

	int selectedType = 0;

	for (int j = 0; j < reportTypes.size(); j++)
	{
		int index = static_cast<int> (SendMessage(rs, CB_ADDSTRING, (WPARAM)0, (LPARAM)reportTypes[j].c_str()));
		if (currentReportType != L"" && currentReportType == reportTypes[j]) selectedType = index;
	}

	SendMessage(rs, CB_SETCURSEL, (WPARAM)selectedType, (LPARAM)0);

	SetWindowSubclass(GetControlHWND("reportsTable"), ReportsProc, GetControlId("reportsTable"), (DWORD_PTR)this);

	DoneLoadingDlg();
	UpdateTable();
}

void FamReport::OnDlgResized()
{
	table->ResetScrollSize();
}

std::vector<TableColumns> FamReport::GetDialogTableColumns(std::wstring selectedReport)
{
	std::vector<std::wstring > mcNames = { L"Boys", L"Girls", L"Total Children", L"Males", L"Females",
		L"Total Active Members", L"Total Inactive Members", L"Total Deceased Members", L"Total Unknown Members"};

	std::vector<std::wstring > fcNames = { L"Single Households With Children", L"Single Households Without Children",
	L"Total Active Families", L"Total Inactive Families" , L"Total Deceased Families" , L"Total Contributor Only Families" , L"Total Moved Families" };

	std::vector<TableColumns> MemberColumns = {
		{ L"ID", 10, "id" },
		{ L"First Name", 25, "firstName" },
		{ L"Last Name", 25, "lastName" },
		{ L"Birth Date", 20, "birthDate" },
		{ L"Member Type", 20, "type" }
	};

	std::vector<TableColumns> FamilyColumns = {
		{ L"ID", 10, "id" },
		{ L"Family", 25, "familyName" },
		{ L"Mailing Name", 25, "mailingName" },
		{ L"Address", 20, "address" },
		{ L"Envelope #", 20, "envelope" }
	};

	if (find(mcNames.begin(), mcNames.end(), selectedReport) != mcNames.end())
	{
		currentLookupItemType = "member";
		return MemberColumns;
	}

	if (find(fcNames.begin(), fcNames.end(), selectedReport) != fcNames.end())
	{
		currentLookupItemType = "family";
		return FamilyColumns;
	}

	return FamilyColumns;
}

void FamReport::UpdateTable(std::function<void(void)> callback)
{
	std::wstring selectedReport = currentReportType != L"" ? currentReportType : GetComboBoxValue("reportTypeSelect");

	PBConnection::Request("getParishStatistic", { { "selectedReport", selectedReport} },
		[&, callback](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			table->ClearTable();

			std::wstring selectedReport = currentReportType != L"" ? currentReportType : GetComboBoxValue("reportTypeSelect");
			std::vector<TableColumns> columns = GetDialogTableColumns(selectedReport);

			table->CreateColumns(columns);
			table->InsertColumns();



			if (result.size() > 200)
			{
				table->SetScrollWheelOffert(10);
			}

			for (std::vector<std::map<std::string, std::wstring>>::reverse_iterator it = result.rbegin(); it != result.rend(); ++it)
			{
				table->InsertRow(std::stoi((*it)["id"].c_str()));
				for (int i = 0; i < columns.size(); i++)
				{
					table->InsertItem((*it)[columns[i].dbName], i);
				}
			}

			currentReportType = L"";

			if (callback != nullptr) callback();
		}
	);

}

bool FamReport::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("print"))
		{
			Print();
			return true;
		}

		break;
	}

	case CBN_SELCHANGE:
	{
		if (wId == GetControlId("reportTypeSelect"))
		{
			UpdateTable();
			return true;
		}

		break;
	}
	}

	return false;
}

bool FamReport::DrawReport()
{
	/*
	DrawPageTitle(printer);
	printer->DrawTextPx(std::string("Wedding Anniversaries Report: ").c_str(), 0, 8, "Arial Rounded MT Bold", 3, "left");

	RECT tableOffset = { 0, 11, 0, 0 };

	std::vector<std::vector<std::wstring>> tabBody;

	PDTHeader tabHeader;
	tabHeader(10, "#");
	tabHeader(30, "Family Name");
	tabHeader(30, "Wedding Date");
	tabHeader(30, "Years");

	std::vector<TableColumns> columns = table->GetColums();

	DB db;

	std::string query = "";

	if (query.length() == 0) return false;

	DBResults reportData = db.GetData(query);
	if (reportData.result.size() == 0)
	{
		MessageBox(NULL, L"Request returned zero", L"Error", MB_OK);
	}

	int i = 0;
	for (std::vector<std::map<std::string, std::wstring>>::reverse_iterator it = reportData.result.rbegin(); it != reportData.result.rend(); ++it)
	{
		tabBody.push_back(std::vector<std::wstring >());
		tabBody[i].push_back(to_wstring(i + 1));
		tabBody[i].push_back((*it)["lastName"] + L", " + (*it)["firstNames"]);
		tabBody[i].push_back((*it)["date"]);
		tabBody[i].push_back((*it)["years"]);
		i++;
	}

	//printer->DrawTable(tableOffset, tabHeader, tabBody, false);
	*/
	return true;
}

bool FamReport::Print()
{
	/*
	printer = new Printer(hWnd);
	string docName = "Wedding Anniversaries Report - " + GetCurrentDate("-");
	printer->SetDocName(docName.c_str());

	if (printer->InitPriner() == true) {
		DrawReport();
		printer->DonePreDraw();
		DrawReport();
		printer->Print();
	}

	*/
	return true;
}

LRESULT CALLBACK FamReport::ReportsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	FamReport* lpData = (FamReport*)dwRefData;

	switch (uMsg)
	{
	case WM_LBUTTONDBLCLK:
	{
		if (uIdSubclass == lpData->GetControlId("reportsTable"))
		{
			lpData->HandleOpenMember();
			return true;
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}