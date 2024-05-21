#include "WeddingReport.h"

WeddingReport::WeddingReport(HWND hWnd) : DlgCommon(hWnd), Reports()
{
	this->hWnd = hWnd;
	currentMonth = L"Not Selected";
	table = NULL;
}

WeddingReport::~WeddingReport()
{
	delete table;
}

void WeddingReport::ShowDlg()
{
	OpenDlg(hWnd, "Wedding Anniversaries Report");
}

void WeddingReport::OnDlgInit()
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
	//table->scClass = reinterpret_cast<PagesCommon*>(this);

	table->CreateColumns({
		{ L"#", 10, "id" },
		{ L"Family Name", 30, "lastName" },
		{ L"Wedding Date", 30, "date" },
		{ L"Years", 30, "years" } 
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

void WeddingReport::OnDlgResized()
{
	table->ResetScrollSize();
}

void WeddingReport::UpdateTable(std::function<void(void)> callback)
{
	PreloaderComponent::Start(hWnd);

	Months months;
	int monthIndex = months[currentMonth];

	PBConnection::Request("getWeddingReport", { {"month", std::to_wstring(monthIndex) } },
		[&, callback](PBResponse res)
		{
			table->ClearTable();

			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			for (size_t i = 0, index = result.size(); i < result.size(); i++, index--)
			{
				table->InsertRow(GetInt(result[i]["id"]));
				table->InsertItem(std::to_wstring(index), 0);
				table->InsertItem(result[i]["lastName"] + L"," + result[i]["firstNames"], 1);
				table->InsertItem(result[i]["date"], 2);
				table->InsertItem(result[i]["years"], 3);
			}


			PreloaderComponent::Stop();
			if (callback != nullptr) callback();
		}
	);


}

bool WeddingReport::OnDlgCommand(WPARAM wParam)
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

void WeddingReport::DrawReport(PrinterDrawer* printer)
{

	float lastY = DrawPageTitle(printer);
	printer->DrawStr(L"Wedding Anniversaries Report: " + currentMonth, 0, lastY += 15);

	PrinterTableDrawer* table = printer->GetTable();


	PDTHeader tabHeader;
	tabHeader.Add(10, L"#");
	tabHeader.Add(30, L"Family Name");
	tabHeader.Add(30, L"Wedding Date");
	tabHeader.Add(30, L"Years");
	table->SetHeaderData(tabHeader);


	table->SetTableOffsets(0.0f, lastY += 10.0f, -1.0f, -1.0f);
	table->DrawHeader();

	int i = 1;

	for (std::vector<std::map<std::string, std::wstring>>::iterator it = reportData.begin(); it != reportData.end(); ++it)
	{
		std::vector<std::wstring> rowData;
		rowData.push_back(std::to_wstring(i));
		rowData.push_back((*it)["lastName"] + L", " + (*it)["firstNames"]);
		rowData.push_back((*it)["date"]);
		rowData.push_back((*it)["years"]);

		table->DrawRow(rowData);
		i++;
	}


}

void WeddingReport::Print()
{
	printer = std::make_shared<Printer>(hWnd);
	std::wstring docName = L"Wedding Anniversaries Report - " + GetCurrentDate(L"-");
	printer->SetDocName(docName.c_str());

	Months months;
	int monthIndex = months[currentMonth];

	PBConnection::Request("getWeddingReport",
		{ {"month", std::to_wstring(monthIndex) } }, std::bind(&WeddingReport::OnDataLoaded, this, std::placeholders::_1)
	);

	if (printer->InitPriner(std::bind(&WeddingReport::DrawReport, this, std::placeholders::_1)))
	{
		printer->Print();
	}
}

void WeddingReport::OnDataLoaded(PBResponse res)
{
	reportData = res.GetResult();
	std::reverse(reportData.begin(), reportData.end());
	printer->OnFinishedLoadingReport();
}