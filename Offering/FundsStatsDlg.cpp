#include "FundsStatsDlg.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

FundsStatsDlg::FundsStatsDlg(HWND hWnd, int fundId, std::wstring fundName) : DlgCommon(hWnd), fundId(fundId), fundName(fundName)
{
	totalDonated = 0.0f;
	table = NULL;
	this->hWnd = hWnd;
	minDlgWidth = 600;
}

FundsStatsDlg::~FundsStatsDlg()
{
	delete table;
}

int FundsStatsDlg::ShowDlg()
{
	return OpenDlg(hWnd, "Fund report");
}

void FundsStatsDlg::OnDlgInit()
{
	int cY = 20;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int dlgWidth = screenWidth / 2;

	int itemsOffsetX = dlgWidth / 100 * 5;
	int selectMembersWidth = dlgWidth / 100 * 30;
	int selectYearWidth = dlgWidth / 100 * 20;
	int selectBatchWidth = dlgWidth / 100 * 45;
	int itemX = 10;

	CreateStatic("fundName", 10, 10, -1, 20, WS_VISIBLE | WS_CHILD, fundName.c_str());
	CreateLine(10, 35, -10, 1, "ln", APP_COLORS::GRAY_BORDER);

	CreateStatic("sdf", 10, 45, 100, 20, WS_VISIBLE | WS_CHILD, L"Date From:");
	CreateEdit("dateFrom", 10, 75, 200, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");
	SetEditMask("dateFrom", ES_DATE_MASK);

	CreateStatic("sdt", 240, 45, 100, 20, WS_VISIBLE | WS_CHILD, L"Date To:");
	CreateEdit("dateTo", 240, 75, 200, 30);
	SetEditMask("dateTo", ES_DATE_MASK);


	CreateBtn("print", 10, 75, 140, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Print Report", WS_STICK_RIGHT);
	SetButtonIcon("print", IDB_PRINT);

	table = new TableCreater(hWnd, GetWindowInstance(hWnd), this);
	table->Create("contTable", 10, 135, screenWidth / 2, screenHeight / 2 - 50);
	table->CreateColumns({
		{ L"Family", 50, "family" },
		{ L"Amount", 50, "amount" },
		});
	table->InsertColumns();

	int yTotalD = screenHeight / 2;
	CreateStatic("totalDonated", 10, 90 + yTotalD + 10, 300, 20, WS_VISIBLE | WS_CHILD);
	DoneLoadingDlg();
	table->ClearTable();

	UpdateTable([&]()
		{
		}
	);


}


void FundsStatsDlg::OnDlgResized()
{
	table->ResetScrollSize();
}


void FundsStatsDlg::UpdateTable(std::function<void(void)> onUpdatedCallback)
{
	PreloaderComponent::Start(hWnd);

	std::wstring dateFrom = GetWindowTextStr(GetControlHWND("dateFrom"));
	std::wstring dateTo = GetWindowTextStr(GetControlHWND("dateTo"));

	PBConnection::Request("getFundsStats", {
			{ "fundId", std::to_wstring(fundId) },
			{ "dateFrom", dateFrom.length() > 0 ? ChangeDateFormat(dateFrom) : L""},
			{ "dateTo", dateTo.length() > 0 ? ChangeDateFormat(dateTo) : L""}
		},
		[&, onUpdatedCallback](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			std::vector<TableColumns> columns = table->GetColums();
			table->ClearTable();
			
			totalDonated = 0.0f;
			for (size_t i = 0; i < result.size(); i++)
			{
				table->InsertRow(GetInt(result[i]["id"]));

				for (size_t j = 0; j < columns.size(); j++)
				{
					table->InsertItem(result[i][columns[j].dbName], (int)j);
					if (columns[j].dbName == "amount")
					{
						totalDonated += GetFloat(result[i][columns[j].dbName]);
					}
				}
			}

			std::wstring str = L"Total donated: $" + GetFloatFormatStr(std::to_wstring(totalDonated));
			SetWindowText(GetControlHWND("totalDonated"), str.c_str());

			PreloaderComponent::Stop();

			if (onUpdatedCallback != nullptr) onUpdatedCallback();
		}
	);

}

void FundsStatsDlg::DrawReport(PrinterDrawer* printer)
{
	float lastY = DrawPageTitle(printer);
	printer->DrawStr(L"Fund report for: " + fundName, 0, lastY += 15);

	PrinterTableDrawer* table = printer->GetTable();


	PDTHeader tabHeader;
	tabHeader.Add(10, L"#");
	tabHeader.Add(50, L"Family");
	tabHeader.Add(40, L"Amount");
	table->SetHeaderData(tabHeader);



	table->SetTableOffsets(0.0f, lastY += 10.0f, -1.0f, -1.0f);
	table->DrawHeader();



	float prevTableY = 0.0f;
	int i = 1;
	for (auto &it: printingData)
	{
		std::vector<std::wstring> rowData;
		rowData.push_back(std::to_wstring(i));
		rowData.push_back(it["family"]);
		rowData.push_back(it["amount"]);
		prevTableY = table->DrawRow(rowData);
		i++;
	}

	printer->DrawLine(0.0f, prevTableY + 1.0f, -1.0f, 0.1f);

	std::wstring donatedStr = L"Total donated: $" + GetFloatFormatStr(std::to_wstring(totalDonated));
	printer->DrawStr(donatedStr, 0.0f, prevTableY + 1.1f);
}

void FundsStatsDlg::Print()
{
	printer = std::make_shared<Printer>(hWnd);
	std::wstring docName = L"Fund Report - " + GetCurrentDate(L"-");
	printer->SetDocName(docName.c_str());


	std::wstring dateFrom = GetWindowTextStr(GetControlHWND("dateFrom"));
	std::wstring dateTo = GetWindowTextStr(GetControlHWND("dateTo"));

	PBConnection::Request("getFundsStats", {
		{ "fundId", std::to_wstring(fundId) },
		{ "dateFrom", dateFrom.length() > 0 ? ChangeDateFormat(dateFrom) : L""},
		{ "dateTo", dateTo.length() > 0 ? ChangeDateFormat(dateTo) : L""},
		}, std::bind(&FundsStatsDlg::OnReadyToPrint, this, std::placeholders::_1)
	);

	
	if (printer->InitPriner(std::bind(&FundsStatsDlg::DrawReport, this, std::placeholders::_1)))
	{
		printer->Print();
	}
}

void FundsStatsDlg::OnReadyToPrint(PBResponse res)
{
	printingData = res.GetResult();
	std::reverse(printingData.begin(), printingData.end());
	printer->OnFinishedLoadingReport();
}

bool FundsStatsDlg::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("btnCancel"))
		{
			EndDialog(hWnd, IDCANCEL);
			return true;
		}
		else if (wId == GetControlId("print"))
		{
			Print();
			return true;
		}
	}
	else if (HIWORD(wParam) == EN_CHANGE)
	{
		if (wId == GetControlId("dateFrom"))
		{
			std::wstring currentDateFrom = GetWindowTextStr(GetControlHWND("dateFrom"));
			if (currentDateFrom.length() == 0 || currentDateFrom.length() == 10)
			{
				UpdateTable();
			}
			return true;
		}
		else if (wId == GetControlId("dateTo"))
		{
			std::wstring currentDateTo = GetWindowTextStr(GetControlHWND("dateTo"));
			if (currentDateTo.length() == 0 || currentDateTo.length() == 10)
			{
				UpdateTable();
			}
			return true;
		}
	}
	

	return false;
}

LRESULT CALLBACK FundsStatsDlg::ecProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	FundsStatsDlg* lpData = (FundsStatsDlg*)dwRefData;


	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}