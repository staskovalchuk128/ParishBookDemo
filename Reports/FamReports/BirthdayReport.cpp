#include "BirthdayReport.h"

BirthdayReport::BirthdayReport(HWND hWnd) : DlgCommon(hWnd), Reports()
{
	this->hWnd = hWnd;
	currentMonth = L"Not Selected";
	table = NULL;
}

BirthdayReport::~BirthdayReport() 
{
	delete table;

}

void BirthdayReport::ShowDlg()
{
	OpenDlg(hWnd, "Birthday Report");
}

void BirthdayReport::OnDlgInit()
{
	Months months;

	CreateStatic("sm", 10, 10, 300, 20, WS_VISIBLE | WS_CHILD, L"Select Month:");
	HWND monthH = CreateCombobox("month", 10, 40, 300, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL);

	CreateBtn("print", 10, 40, 140, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Print Report", WS_STICK_RIGHT);

	SetButtonIcon("print", IDB_PRINT);

	this->table = new TableCreater(hWnd, GetWindowInstance(hWnd), this);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	table->Create("contTable", 10, 90, screenWidth / 2, screenHeight / 2);

	table->CreateColumns({
		{ L"ID", 10, "id" },
		{ L"First Name", 30, "firstName" },
		{ L"Last Name", 30, "lastName" },
		{ L"Birth date", 30, "birthDate" } 
	});

	table->InsertColumns();

	SendMessage(monthH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)currentMonth.c_str());
	for (int i = 0; i < months.items.size(); i++)
	{
		SendMessage(monthH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)months.items[i].c_str());
	}

	SendMessage(monthH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	DoneLoadingDlg();
	table->ClearTable();


	UpdateTable([&]()
		{
		}
	);

}

void BirthdayReport::OnDlgResized()
{
	table->ResetScrollSize();
}

void BirthdayReport::UpdateTable(std::function<void(void)> callback)
{
	PreloaderComponent::Start(hWnd);

	Months months;
	int monthIndex = months[currentMonth];

	PBConnection::Request("getBirthdayReport", { {"month", std::to_wstring(monthIndex) } },
		[&, callback](PBResponse res)
		{
			table->ClearTable();

			std::vector<TableColumns> columns = table->GetColums();

			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			std::reverse(result.begin(), result.end());

			for (size_t i = 0; i < result.size(); i++)
			{
				table->InsertRow(GetInt(result[i]["id"]));

				for (size_t j = 0; j < columns.size(); j++)
				{
					table->InsertItem(result[i][columns[j].dbName], (int)j);
				}
			}

			PreloaderComponent::Stop();
			if (callback != nullptr) callback();
		}
	);

}

bool BirthdayReport::OnDlgCommand(WPARAM wParam)
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
		if (wId == GetControlId("month"))
		{
			currentMonth = GetWindowTextStr(GetControlHWND("month"));
			UpdateTable();
			return true;
		}

		break;
	}
	}

	return false;
}

void BirthdayReport::DrawReport(PrinterDrawer* printer)
{
	float lastY = DrawPageTitle(printer);
	printer->DrawStr(L"Birthday Report: " + currentMonth, 0, lastY += 15);

	PrinterTableDrawer* table = printer->GetTable();


	PDTHeader tabHeader;
	tabHeader.Add(10, L"#");
	tabHeader.Add(30, L"Fist Name");
	tabHeader.Add(30, L"Last name");
	tabHeader.Add(30, L"Birth Date");
	table->SetHeaderData(tabHeader);


	table->SetTableOffsets(0.0f, lastY += 10.0f, -1.0f, -1.0f);
	table->DrawHeader();

	int i = 1;
	for (std::vector<std::map<std::string, std::wstring>>::iterator it = reportData.begin(); it != reportData.end(); ++it)
	{
		std::vector<std::wstring> rowData;
		rowData.push_back(std::to_wstring(i));
		rowData.push_back((*it)["firstName"]);
		rowData.push_back((*it)["lastName"]);
		rowData.push_back((*it)["birthDate"]);

		table->DrawRow(rowData);
		i++;
	}
}

void BirthdayReport::Print()
{
	printer = std::make_shared<Printer>(hWnd);
	std::wstring docName = L"Birthday Report - " + GetCurrentDate(L"-");
	printer->SetDocName(docName.c_str());

	Months months;
	int monthIndex = months[currentMonth];

	PBConnection::Request("getBirthdayReport",
		{ {"month", std::to_wstring(monthIndex) } }, std::bind(&BirthdayReport::OnDataLoaded, this, std::placeholders::_1)
	);

	if (printer->InitPriner(std::bind(&BirthdayReport::DrawReport, this, std::placeholders::_1)))
	{
		printer->Print();
	}
}

void BirthdayReport::OnDataLoaded(PBResponse res)
{
	reportData = res.GetResult();
	printer->OnFinishedLoadingReport();
}