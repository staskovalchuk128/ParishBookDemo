#include "MarriageCertificate.h"
#include "../../Components/MonthsStruct.h"

#include "..//ContReports.h"

MarriageCertificate::~MarriageCertificate() {}

MarriageCertificate::MarriageCertificate(HWND hw, HINSTANCE hI) : CertCommon(hw, hI, L"marriage")
{
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Husband Name", "mainName" }));
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Wife Name", "mainName2" }));
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Witness 1", "witness1" }));
}

bool MarriageCertificate::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("btnPrintCert"))
		{
			try
			{
				ValidateFields();
			}
			catch (std::wstring& str)
			{
				MessageDlg(NULL, L"Error",
					str.c_str(),
					MD_OK, MD_ERR).OpenDlg();
				return true;
			}
			catch (const std::exception& e)
			{
				MessageDlg(NULL, L"Error",
					StrToWStr(e.what()).c_str(),
					MD_OK, MD_ERR).OpenDlg();
				return true;
			}

			Printer printer(hWnd);
			printer.SetShowContentOffset(true, L"MarCertPrint");
			printer.SetDrawPageNum(false);
			printer.SetDrawPrintDate(false);
			printer.SetUserBorder(false);
			printer.SetMinusPrinterBorder(true);
			printer.SetPaperSize(DMPAPER_A5);
			printer.SetSavedPrintingConfig(selectionConfigName);
			printer.SetPrintedCallback(std::bind(&MarriageCertificate::OnPrinted, this, std::placeholders::_1));
			if (printer.InitPriner(std::bind(&MarriageCertificate::DrawCert, this, std::placeholders::_1), false))
			{
				printer.Print();
			}
			SaveToHistory();

			return true;
		}
		else if (wId == GetControlId("btnPrintFancyCert"))
		{
			try
			{
				ValidateFields();
			}
			catch (std::wstring& str)
			{
				MessageDlg(NULL, L"Error",
					str.c_str(),
					MD_OK, MD_ERR).OpenDlg();
				return true;
			}
			catch (const std::exception& e)
			{
				MessageDlg(NULL, L"Error",
					StrToWStr(e.what()).c_str(),
					MD_OK, MD_ERR).OpenDlg();
				return true;
			}

			Printer printer(hWnd);
			printer.SetShowContentOffset(true, L"MarFancyCertPrint");
			printer.SetDrawPageNum(false);
			printer.SetDrawPrintDate(false);
			printer.SetUserBorder(false);
			printer.SetMinusPrinterBorder(true);
			printer.SetPaperSize(DMPAPER_A4);
			printer.SetOrientation(DMORIENT_LANDSCAPE);
			printer.SetSavedPrintingConfig(selectionConfigName);
			printer.SetPrintedCallback(std::bind(&MarriageCertificate::OnPrinted, this, std::placeholders::_1));
			if (printer.InitPriner(std::bind(&MarriageCertificate::DrawFancyCert, this, std::placeholders::_1), false))
			{
				printer.Print();
			}
			SaveToHistory();

			return true;
		}

		break;
	}
	}

	return false;
}

void MarriageCertificate::AddControls()
{

	SPData winSize = GetWinSize();

	int itemWidth = 32;
	int itemsMarginX = 1;
	int itemHeight = 30;

	CreateStatic("sTitle", 10, 20, -10, 40, WS_VISIBLE | WS_CHILD, L"Marriage Cerificate");
	SetFontSize("sTitle", 28);

	CreateLine(10, 70, -10, 1, "sepLine", APP_COLORS::GRAY_BORDER);

	CreateBtn("btnPrintCert", 10, 90, 200, 30, WS_CHILD | WS_VISIBLE, L"Print certificate");
	CreateBtn("btnPrintFancyCert", 10, 90, 250, 30, WS_CHILD | WS_VISIBLE, L"Print fancy certificate", WS_STICK_RIGHT);

	CreateLine(10, 140, -10, 1, "sepLine", APP_COLORS::GRAY_BORDER);

	int lineY = 140;
	int rowOffsetY = 30;
	int rowTitleValOffsetY = 30;
	int rowTitleY = lineY + rowOffsetY;
	int rowValY = rowTitleY + rowTitleValOffsetY;

	CreateStatic("smainName1", itemsMarginX, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Main name 1", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("mainName", itemsMarginX, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"", WS_PRC_SIZE | WS_PRC_POS);

	CreateStatic("ssmainName2", itemWidth + itemsMarginX * 2, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Main name 2", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("mainName2", itemWidth + itemsMarginX * 2, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"", WS_PRC_SIZE | WS_PRC_POS);

	CreateStatic("sDateOfMar", itemWidth * 2 + itemsMarginX * 3, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Date of marriage", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("dateOfMar", itemWidth * 2 + itemsMarginX * 3, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"", WS_PRC_SIZE | WS_PRC_POS);
	SetEditMask("dateOfMar", ES_DATE_MASK);

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	CreateStatic("sStateOf", itemsMarginX, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"The state of", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("stateOf", itemsMarginX, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"", WS_PRC_SIZE | WS_PRC_POS);

	CreateStatic("sByRev", itemWidth + itemsMarginX * 2, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"By Rev.", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("byRev", itemWidth + itemsMarginX * 2, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"", WS_PRC_SIZE | WS_PRC_POS);

	CreateStatic("sWitness1", itemWidth * 2 + itemsMarginX * 3, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Witness 1", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("witness1", itemWidth * 2 + itemsMarginX * 3, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"", WS_PRC_SIZE | WS_PRC_POS);

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	std::wstring currentDate = GetCurrentDate(L"/");

	CreateStatic("sWitness2", itemsMarginX, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Witness 2", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("witness2", itemsMarginX, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"", WS_PRC_SIZE | WS_PRC_POS);

	CreateStatic("sDated", itemWidth + itemsMarginX * 2, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Dated", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("dated", itemWidth + itemsMarginX * 2, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, currentDate.c_str(), WS_PRC_SIZE | WS_PRC_POS);
	SetEditMask("dated", ES_DATE_MASK);

	CreateStatic("sPastor", itemWidth * 2 + itemsMarginX * 3, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Pastor", WS_PRC_SIZE | WS_PRC_POS);
	CreateEdit("pastor", itemWidth * 2 + itemsMarginX * 3, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"", WS_PRC_SIZE | WS_PRC_POS);

}



void MarriageCertificate::ValidateFields()
{
	std::wstring dateOfMar = GetWindowTextStr(GetControlHWND("dateOfMar")), dated = GetWindowTextStr(GetControlHWND("dated"));

	if (dateOfMar.length() != 10) throw std::wstring(L"Enter valid 'Date of marriage'");
	if (dated.length() != 10) throw std::wstring(L"Enter valid birth 'dated'");
}



void MarriageCertificate::DrawCert(PrinterDrawer* printer)
{
	Months months;

	float adjustY = -23.5f;
	float adjustX = -3.0f;

	std::wstring xOffsetVal, yOffsetVal;

	config.GetValue(L"MarCertPrint", L"xOffset", xOffsetVal);
	config.GetValue(L"MarCertPrint", L"yOffset", yOffsetVal);

	adjustX += GetFloat(xOffsetVal);
	adjustY += GetFloat(yOffsetVal);

	float churchNameY = 70.5f + adjustY;
	float churchNameX = 44.0f + adjustX;

	float churchAdressY = 79.0f + adjustY;
	float churchAdressX = 27.0f + adjustX;

	std::wstring churchAddressStr = orgAddress + L", " + orgCity + L", " + orgState + L", " + orgZip;

	printer->SetNewFont(4.0f, CFONT_LUCIDA_CALLIGRAPHY, FontStyleRegular);

	printer->DrawStr(orgName, churchNameX, churchNameY);
	printer->SetNewFont(4.0f, CFONT_LUCIDA_CALLIGRAPHY, FontStyleRegular);
	printer->DrawStr(churchAddressStr, churchAdressX, churchAdressY);
	printer->SetNewFont(4.0f, CFONT_LUCIDA_CALLIGRAPHY, FontStyleRegular);

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")), mainName2 = GetWindowTextStr(GetControlHWND("mainName2")),
		marDate = GetWindowTextStr(GetControlHWND("dateOfMar")), stateOf = GetWindowTextStr(GetControlHWND("stateOf")),
		rev = GetWindowTextStr(GetControlHWND("byRev")), witness = GetWindowTextStr(GetControlHWND("witness1")),
		witness2 = GetWindowTextStr(GetControlHWND("witness2")), dated = GetWindowTextStr(GetControlHWND("dated")),
		pastor = GetWindowTextStr(GetControlHWND("pastor"));

	float mainNameY = 102.0f + adjustY;
	float mainNameX = 30.0f + adjustX;

	float mainName2Y = 111.0f + adjustY;
	float mainName2X = 30.0f + adjustX;

	float marDateY = 128.5f + adjustY;
	float marDateDayX = 33.0f + adjustX;
	float marDateMonthX = 70.0f + adjustX;
	float marDateYearX = 114.0f + adjustX;

	float stateOfY = 155.0f + adjustY;
	float stateOfX = 40.0f + adjustX;

	float revY = 163.5f + adjustY;
	float revX = 30.0f + adjustX;

	float witnessY = 172.5f + adjustY;
	float witnessX = 49.0f + adjustX;

	float witness2Y = 181.5f + adjustY;
	float witness2X = 28.0f + adjustX;

	float datedY = 198.5f + adjustY;
	float datedX = 35.0f + adjustX;

	float pastorY = 214.0f + adjustY;
	float pastorX = 38.0f + adjustX;

	int marMonth = GetInt(marDate.substr(0, 2));
	int marDay = GetInt(marDate.substr(3, 2));
	int marYear = GetInt(marDate.substr(6));

	std::wstring marMonthStr = months.GetMonthName(marMonth);

	std::wstring marNumEnd = GetDayEnd(marDay);

	printer->DrawStr(mainName, mainNameX, mainNameY);
	printer->DrawStr(mainName2, mainName2X, mainName2Y);

	printer->DrawStr(std::to_wstring(marDay) + marNumEnd, marDateDayX, marDateY);
	printer->DrawStr(marMonthStr, marDateMonthX, marDateY);
	printer->DrawStr(std::to_wstring(marYear), marDateYearX, marDateY);

	printer->DrawStr(stateOf, stateOfX, stateOfY);
	printer->DrawStr(rev, revX, revY);
	printer->DrawStr(witness, witnessX, witnessY);
	printer->DrawStr(witness2, witness2X, witness2Y);
	printer->DrawStr(dated, datedX, datedY);
	printer->DrawStr(pastor, pastorX, pastorY);

}

void MarriageCertificate::DrawFancyCert(PrinterDrawer* printer)
{
	Months months;
	float yAdjust = 1.0f;
	float xAdjust = 44.0f;

	std::wstring xOffsetVal, yOffsetVal;

	config.GetValue(L"MarFancyCertPrint", L"xOffset", xOffsetVal);
	config.GetValue(L"MarFancyCertPrint", L"yOffset", yOffsetVal);

	xAdjust += GetFloat(xOffsetVal);
	yAdjust += GetFloat(yOffsetVal);


	float name1LineX = 96.0f + xAdjust;
	float name2LineX = 107.0f + xAdjust;
	float name1LineWidth = 137.0f;
	float name2LineWidth = 126.0f;


	float churchNameY = 136.0f + yAdjust;
	float churchNameX = 105.0f + xAdjust;

	float churchAdressY = 136.0f + yAdjust;
	float churchAdressX = 175.0f + xAdjust;

	std::wstring churchAddressStr = orgCity + L", " + orgState;

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")), mainName2 = GetWindowTextStr(GetControlHWND("mainName2")),
		marDate = GetWindowTextStr(GetControlHWND("dateOfMar")), stateOf = GetWindowTextStr(GetControlHWND("stateOf")),
		rev = GetWindowTextStr(GetControlHWND("byRev")), witness = GetWindowTextStr(GetControlHWND("witness1")),
		witness2 = GetWindowTextStr(GetControlHWND("witness2")), dated = GetWindowTextStr(GetControlHWND("dated")),
		pastor = GetWindowTextStr(GetControlHWND("pastor"));

	float mainNameY = 65.0f + yAdjust;
	float mainNameX = 100.0f + xAdjust;

	float mainName2Y = 78.0f + yAdjust;
	float mainName2X = 112.0f + xAdjust;

	float marDateY = 107.0f + yAdjust;
	float marDateDayX = 120.0f + xAdjust;
	float marDateMonthX = 180.0f + xAdjust;
	float marDateYearX = 219.0f + xAdjust;

	float stateOfY = 155.0f + yAdjust;
	float stateOfX = 40.0f + xAdjust;

	float revY = 123.0f + yAdjust;
	float revX = 109.0f + xAdjust;

	float witnessY = 157.0f + yAdjust;
	float witnessX = 150.0f + xAdjust;

	float witness2Y = 171.0f + yAdjust;
	float witness2X = 150.0f + xAdjust;

	int marMonth = GetInt(marDate.substr(0, 2));
	int marDay = GetInt(marDate.substr(3, 2));
	int marYear = GetInt(marDate.substr(6));

	std::wstring marMonthStr = months.GetMonthName(marMonth);

	std::wstring marNumEnd = GetDayEnd(marDay);

	printer->SetNewFont(5.0f, CFONT_LUCIDA_CALLIGRAPHY, FontStyleRegular);


	RectF name1Size = printer->MeasureText(mainName);
	mainNameX = name1LineX + (name1LineWidth - name1Size.Width) / 2.0f;

	RectF name2Size = printer->MeasureText(mainName2);
	mainName2X = name1LineX + (name1LineWidth - name2Size.Width) / 2.0f;


	printer->DrawStr(mainName, mainNameX, mainNameY);
	printer->DrawStr(mainName2, mainName2X, mainName2Y);

	printer->DrawStr(std::to_wstring(marDay) + marNumEnd, marDateDayX, marDateY);
	printer->DrawStr(marMonthStr, marDateMonthX, marDateY);
	printer->DrawStr(std::to_wstring(marYear), marDateYearX, marDateY);

	printer->DrawStr(rev, revX, revY);
	printer->DrawStr(L"St. Ann - Parish", churchNameX, churchNameY);
	printer->DrawStr(L"Kosciusko, TX", churchAdressX, churchAdressY);
	printer->DrawStr(witness, witnessX, witnessY);
	printer->DrawStr(witness2, witness2X, witness2Y);


}



void MarriageCertificate::ClearAllFields()
{

	SetWindowText(GetControlHWND("mainName"), L"");
	SetWindowText(GetControlHWND("mainName2"), L"");
	SetWindowText(GetControlHWND("dateOfMar"), L"");
	SetWindowText(GetControlHWND("stateOf"), L"");
	SetWindowText(GetControlHWND("byRev"), L"");
	SetWindowText(GetControlHWND("witness1"), L"");
	SetWindowText(GetControlHWND("witness2"), L"");
	SetWindowText(GetControlHWND("dated"), L"");
	SetWindowText(GetControlHWND("pastor"), L"");
}

void MarriageCertificate::SaveToHistory(std::function<void(void)> callback)
{

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")),
		mainName2 = GetWindowTextStr(GetControlHWND("mainName2")),
		dateOfMar = GetWindowTextStr(GetControlHWND("dateOfMar")),
		stateOf = GetWindowTextStr(GetControlHWND("stateOf")),
		byRev = GetWindowTextStr(GetControlHWND("byRev")),
		witness1 = GetWindowTextStr(GetControlHWND("witness1")),
		witness2 = GetWindowTextStr(GetControlHWND("witness2")),
		dated = GetWindowTextStr(GetControlHWND("dated")),
		pastor = GetWindowTextStr(GetControlHWND("pastor"));

	if (mainName.size() == 0) return;

	PBConnection::Request("saveMarriageCertToHistory",
		{
			{"type", certificateTypeStr},
			{"mainName", mainName},
			{"mainName2", mainName2},
			{"dateOfMar", dateOfMar},
			{"stateOf", stateOf},
			{"byRev", byRev},
			{"witness1", witness1},
			{"witness2", witness2},
			{"dated", dated},
			{"pastor", pastor}
		},
		[&, callback](PBResponse res)
		{
			if (callback != nullptr) callback();
		}
	);

}

std::vector<std::pair<std::wstring, std::string>> MarriageCertificate::GetFillTableFields()
{
	return fillTableFields;
}

void MarriageCertificate::FillFromHistory(int id)
{
	

	PBConnection::Request("fillCertificateFromHistory",
		{
			{"certId", std::to_wstring(id) }
		},
		[&](PBResponse res)
		{
			std::map<std::string, std::wstring> result = res.GetResultMap("main");

			if (result.size() > 0)
			{
				std::wstring mainName = result["mainName"],
					mainName2 = result["mainName2"],
					dateOfMar = result["dateOfMarriage"],
					stateOf = result["stateOf"],
					byRev = result["rev"],
					witness1 = result["witness1"],
					witness2 = result["witness2"],
					dated = result["dated"],
					pastor = result["issuedBy"];

				SetWindowText(GetControlHWND("mainName"), mainName.c_str());
				SetWindowText(GetControlHWND("mainName2"), mainName2.c_str());
				SetWindowText(GetControlHWND("dateOfMar"), dateOfMar.c_str());
				SetWindowText(GetControlHWND("stateOf"), stateOf.c_str());
				SetWindowText(GetControlHWND("byRev"), byRev.c_str());
				SetWindowText(GetControlHWND("witness1"), witness1.c_str());
				SetWindowText(GetControlHWND("witness2"), witness2.c_str());
				SetWindowText(GetControlHWND("dated"), dated.c_str());
				SetWindowText(GetControlHWND("pastor"), pastor.c_str());

			}
		}
	);

}



void MarriageCertificate::SaveToTempData()
{
	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")),
		dated = GetWindowTextStr(GetControlHWND("dated")),
		pastor = GetWindowTextStr(GetControlHWND("pastor"));

	tempDataPtr->mainName = mainName;
	tempDataPtr->dated = dated;
	tempDataPtr->pastor = pastor;

}
void MarriageCertificate::FillFromTempData()
{
	SetWindowText(GetControlHWND("mainName"), tempDataPtr->mainName.c_str());
	SetWindowText(GetControlHWND("dated"), tempDataPtr->dated.c_str());
	SetWindowText(GetControlHWND("pastor"), tempDataPtr->pastor.c_str());
}