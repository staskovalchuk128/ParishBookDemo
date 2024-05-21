#include "EndYearReport.h"

EndYearReport::EndYearReport(HWND hWnd) : DlgCommon(hWnd), Reports()
{
	currentPageNum = currentPrintingItemId = printersPageHeight = -1;
	this->hWnd = hWnd;
	firstPageFits = 18;
	secondPageFits = 49;

	nameFormats[NameFormat::FORMAL] = L"Formal name (Mr. & Mrs.)";
	nameFormats[NameFormat::INFORMAL] = L"Informal name";

	currentNameFormat = NameFormat::FORMAL;
}

EndYearReport::~EndYearReport() {}

void EndYearReport::ShowDlg()
{
	OpenDlg(hWnd, "End Of Year Report");
}

void EndYearReport::OnDlgInit()
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);


	CreateStatic("df", 10, 10, 200, 20, WS_VISIBLE | WS_CHILD, L"Date from:");
	CreateEdit("dateFrom", 10, 40, 200, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, std::wstring(L"01/01/" + std::to_wstring(GetInt(GetCurrentYear()) - 1)).c_str(), 0);

	CreateStatic("dt", 230, 10, 200, 20, WS_VISIBLE | WS_CHILD, L"Date to:");
	CreateEdit("dateTo", 230, 40, 200, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, std::wstring(L"12/31/" + GetCurrentYear()).c_str(), 0);

	SetEditMask("dateFrom", ES_DATE_MASK);
	SetEditMask("dateTo", ES_DATE_MASK);

	CreateStatic("spn", 10, 90, 300, 20, WS_VISIBLE | WS_CHILD, L"Priest Name: (Required)");
	CreateEdit("priestName", 10, 120, screenWidth / 2, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"test", 0);

	CreateStatic("spn", 10, 160, screenWidth / 2, 20, WS_VISIBLE | WS_CHILD, L"Thank you message: (Optional, Recomended ~100 symbols)");
	CreateEdit("thanksMsg", 10, 190, screenWidth / 2, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP);

	CreateStatic("sm", 10, 230, screenWidth / 2, 20, WS_VISIBLE | WS_CHILD, L"Name format:");
	HWND monthH = CreateCombobox("nameFormat", 10, 260, screenWidth / 2, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL);

	for (auto it : nameFormats)
	{
		SendMessage(monthH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)it.second.c_str());
	}
	SendMessage(monthH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);


	CreateBtn("print", 10, 40, 140, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Print Report", WS_STICK_RIGHT);
	SetButtonIcon("print", IDB_PRINT);

	CreateBtn("btnClose", 10, 340, 120, 30, WS_CHILD | WS_VISIBLE, L"Close", WS_STICK_RIGHT);
	SetColor("btnClose", APP_COLORS::GRAY);

	SetFocus(GetControlHWND("dateFrom"));

	DoneLoadingDlg();
	//DELETE THIS
	//OnPrintReport();
}

void EndYearReport::OnDlgResized() {}

bool EndYearReport::OnPrintReport()
{
	periodFrom = GetWindowTextStr(GetControlHWND("dateFrom"));
	periodTo = GetWindowTextStr(GetControlHWND("dateTo"));
	priestName = GetWindowTextStr(GetControlHWND("priestName"));
	thanksMsg = GetWindowTextStr(GetControlHWND("thanksMsg"));

	int index = (int)SendMessage(GetControlHWND("nameFormat"), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	currentNameFormat = (NameFormat)index;

	if (periodFrom.length() != 10 || periodTo.length() != 10)
	{
		MessageDlg(hWnd, L"Error",
			L"Enter valid from/to date.",
			MD_OK, MD_ERR).OpenDlg();
		return false;
	}

	if (priestName.length() == 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Enter priest name.",
			MD_OK, MD_ERR).OpenDlg();
		return false;
	}

	Print();
	return true;
}

bool EndYearReport::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("print"))
		{
			OnPrintReport();
			return true;
		}
		else if (wId == GetControlId("btnClose"))
		{
			SendMessage(hWnd, WM_COMMAND, IDCANCEL, NULL);
			return true;
		}

		break;
	}
	}

	return false;
}

int EndYearReport::GetTotalPages(std::vector<std::map<std::string, std::wstring>>& data)
{
	int totalPages = 1;
	int totalItems = (int)data.size();

	totalItems -= firstPageFits;

	if (totalItems <= 0) return totalPages;

	int diff = (int)ceil((float)totalItems / (float)secondPageFits);

	return totalPages + diff;
	return 0;
}

void EndYearReport::DrawPageNum(std::vector<std::map<std::string, std::wstring>>& data, PrinterDrawer* printer)
{
	printer->SetNewFont(2.0f, CFONT_ARIAL, FontStyleRegular);
	int totalPages = GetTotalPages(data);

	std::wstring pageStr = L"Page " + std::to_wstring(currentPageNum) + L" of " + std::to_wstring(totalPages);
	RectF strSize = printer->MeasureText(pageStr);
	PointF pos((float)printer->GetPageWidth() - strSize.X, (float)printer->GetPrinterBorderOffsetY());

	printer->DrawStr(pageStr, pos.X, pos.Y);

	printer->SetFontToDefault();
	currentPageNum++;
}
void EndYearReport::DrawBottomPageInfo(PrinterDrawer* printer)
{
	printer->SetNewFont(2.0f, CFONT_ARIAL, FontStyleRegular);
	std::wstring str = L"No goods or services were received in exchange for these contributions except for intangible religious benefits.";
	RectF strSize = printer->MeasureText(str);

	PointF pos(0, (float)printersPageHeight - printer->GetPrinterBorderOffsetY() - strSize.Y);
	printer->DrawStr(str, pos.X, pos.Y, TEXT_ALIGN_CENTER);
	printer->SetFontToDefault();


}
void EndYearReport::DrawReportItem(std::map<std::string, std::wstring>& reportItem, PrinterDrawer* printer)
{

	currentPageNum = 1;
	//currentPrintingItemId = itemId;

	std::map<std::string, std::wstring> familyInfo = CreateMapFromJson(reportItem["familyInfo"]);
	std::vector<std::map<std::string, std::wstring>> reportItems = CreateVecFromJson(reportItem["donations"]);

	std::wstring familyName = familyInfo["mailingName"];
	if (currentNameFormat == NameFormat::INFORMAL)
	{
		familyName = familyInfo["informalName"];
	}
	familyName = Trim(familyName);
	std::wstring address = familyInfo["address"];
	std::wstring address2 = familyInfo["address2"];
	std::wstring city = familyInfo["city"];
	std::wstring state = familyInfo["state"];
	std::wstring zip = familyInfo["zip"];
	std::wstring plus4 = familyInfo["plus4"];
	zip = plus4.length() > 0 ? zip + L"-" + plus4 : zip;
	address = address2.length() > 0 ? address + address2 : address;

	std::wstring fullCityAddress = city + L", " + state + L" " + zip;

	std::wstring printedDate = GetCurrentDate(L"/");

	DrawPageNum(reportItems, printer);
	DrawBottomPageInfo(printer);

	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleRegular);

	printer->DrawStr(orgName, 16.0f, 19.0f);
	printer->DrawStr(orgAddress, 16.0f, 24.0f);

	std::wstring cityStr = orgCity + L", " + orgState + L" " + orgZip;

	printer->DrawStr(cityStr, 16.0f, 29.0f);

	std::wstring year(periodFrom.rbegin(), periodFrom.rbegin() + 4);
	reverse(year.begin(), year.end());

	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleBold);
	printer->DrawStr(L"Contributions Detail Statement", 129.0f, 49.0f);
	printer->SetNewFont(3.0f, CFONT_ARIAL, FontStyleRegular);
	printer->DrawStr(L"Reporting Period: " + periodFrom + L" to " + periodTo, 129.0f, 55.0f);
	printer->SetNewFont(2.0f, CFONT_ARIAL, FontStyleRegular);
	printer->DrawStr(L"Date Printed: " + printedDate, 148.0f, 62.0f);
	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleRegular);

	printer->DrawStr(familyName.c_str(), 36.0f, 65.0f);
	printer->DrawStr(address.c_str(), 36.0f, 70.0f);
	printer->DrawStr(fullCityAddress.c_str(), 36.0f, 76.0f);

	printer->DrawStr(L"Dear " + familyName, 14.0f, 120.0f);
	printer->DrawStr(L"Here is your " + year + L" Contribution Statement.", 14.0f, 125.0f);

	float nextOffset = 125.0f;

	if (thanksMsg.length() > 0) {
		printer->DrawStr(thanksMsg.c_str(), 14.0f, nextOffset += 5.0f);
	}

	printer->DrawStr(L"May God Bless you,", 14.0f, nextOffset += 5.0f);
	printer->DrawStr(priestName.c_str(), 14.0f, nextOffset += 5.0f);

	PrinterTableDrawer* table = printer->GetTable();

	table->SetStartNewPageCallback([&,printer]() {
		DrawPageNum(reportItems, printer);
		DrawBottomPageInfo(printer);
	});

	PDTHeader tabHeader;
	tabHeader.Add(70, L"Fund Name");
	tabHeader.Add(30, L"Amount");
	table->SetHeaderData(tabHeader);


	float totalGiven = 0;
	
	for (std::vector<std::map<std::string, std::wstring>>::reverse_iterator it = reportItems.rbegin(); it != reportItems.rend(); ++it)
	{
		totalGiven += GetFloat((*it)["totalAmount"]);
	}

	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleBold);
	printer->DrawStr(L"Total Contributions: $" + GetFloatFormatStr(std::to_wstring(totalGiven)), 0.0f, 149.0f, TEXT_ALIGN_CENTER);
	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleRegular);

	table->SetTableOffsets((float)printer->GetBorderX(), 154.0f, -1.0f, -1.0f);
	table->DrawHeader();

	for (std::vector<std::map<std::string, std::wstring>>::reverse_iterator it = reportItems.rbegin(); it != reportItems.rend(); ++it)
	{
		std::vector<std::wstring> rowData;
		rowData.push_back((*it)["fundName"]);
		rowData.push_back(GetFloatFormatStr(std::to_wstring(GetFloat((*it)["totalAmount"]))));
		table->DrawRow(rowData);
	}
}

void EndYearReport::DrawReport(PrinterDrawer* printer)
{
	printersPageHeight = printer->GetPageHeight();


	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleRegular);

	printer->GetTable()->SetPageHeight(printersPageHeight - 10); // keep 1 cm for page bottom info


	for (std::vector<std::map<std::string, std::wstring>>::reverse_iterator it = reportData.rbegin(); it != reportData.rend(); ++it)
	{
		if (it != reportData.rbegin() && it != reportData.rend())
		{
			printer->StartNewPage();
		}

		DrawReportItem(*it, printer);
	}

}

void EndYearReport::Print()
{

	printer = std::make_shared<Printer>(hWnd);
	std::wstring docName = L"End Of Year Report - " + GetCurrentDate(L"-");
	printer->SetDocName(docName.c_str());
	printer->SetDrawPrintDate(false);
	printer->SetUserBorder(false);
	printer->SetMinusPrinterBorder(true);

	periodFrom = GetWindowTextStr(GetControlHWND("dateFrom"));
	periodTo = GetWindowTextStr(GetControlHWND("dateTo"));

	PBConnection::Request("getEndYearReport", {
		{"periodFrom", periodFrom},
		{"periodTo", periodTo}
		}, 
		std::bind(&EndYearReport::OnDataLoaded, this, std::placeholders::_1)
	);

	if (printer->InitPriner(std::bind(&EndYearReport::DrawReport, this, std::placeholders::_1)))
	{
		printer->Print();
	}

	
}

void EndYearReport::OnDataLoaded(PBResponse res)
{
	reportData = res.GetResultVec("main");
	printer->OnFinishedLoadingReport();
}