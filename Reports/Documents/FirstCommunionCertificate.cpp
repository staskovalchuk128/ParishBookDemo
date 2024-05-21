#include "FirstCommunionCertificate.h"
#include "../../Components/MonthsStruct.h"

#include "..//ContReports.h"


FirstCommunionCertificate::FirstCommunionCertificate(HWND hw, HINSTANCE hI) : CertCommon(hw, hI, L"firstCom")
{
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Main Name", "mainName" }));
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Fathers Name", "fathersName" }));
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Mothers Name", "mothersName" }));
}

FirstCommunionCertificate::~FirstCommunionCertificate() {}

bool FirstCommunionCertificate::OnCommand(WPARAM wParam)
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
			printer.SetShowContentOffset(true, L"FCCertPrint");
			printer.SetDrawPageNum(false);
			printer.SetDrawPrintDate(false);
			printer.SetUserBorder(false);
			printer.SetMinusPrinterBorder(true);
			printer.SetPaperSize(DMPAPER_A5);
			printer.SetSavedPrintingConfig(selectionConfigName);
			printer.SetPrintedCallback(std::bind(&FirstCommunionCertificate::OnPrinted, this, std::placeholders::_1));
			if (printer.InitPriner(std::bind(&FirstCommunionCertificate::DrawCert, this, std::placeholders::_1), false))
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
			printer.SetShowContentOffset(true, L"FCFancyCertPrint");
			printer.SetDrawPageNum(false);
			printer.SetDrawPrintDate(false);
			printer.SetUserBorder(false);
			printer.SetMinusPrinterBorder(true);
			printer.SetPaperSize(DMPAPER_A4);
			printer.SetOrientation(DMORIENT_LANDSCAPE);
			printer.SetSavedPrintingConfig(selectionConfigName);
			printer.SetPrintedCallback(std::bind(&FirstCommunionCertificate::OnPrinted, this, std::placeholders::_1));
			if (printer.InitPriner(std::bind(&FirstCommunionCertificate::DrawFancyCert, this, std::placeholders::_1), false))
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

void FirstCommunionCertificate::AddControls()
{
	SPData winSize = GetWinSize();

	int itemWidth = winSize.width / 4;
	int itemHeight = 30;

	CreateStatic("sTitle", 10, 20, -10, 40, WS_VISIBLE | WS_CHILD, L"First Communion Cerificate");
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

	CreateStatic("smainName", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Name");
	CreateEdit("mainName", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sResidence", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Residence");
	CreateEdit("residence", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sBaptizedAt", itemWidth * 2 + 30, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Baptized at");
	CreateEdit("baptizedAt", itemWidth * 2 + 30, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sBaptizeDate", itemWidth * 3 + 40, rowTitleY, itemWidth - 50, itemHeight, WS_VISIBLE | WS_CHILD, L"Baptize date");
	CreateEdit("baptizeDate", itemWidth * 3 + 40, rowValY, itemWidth - 50, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");
	SetEditMask("baptizeDate", ES_DATE_MASK);

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	CreateStatic("sFathersName", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Fathers name");
	CreateEdit("fathersName", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sMothersName", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Mothers name");
	CreateEdit("mothersName", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sCommDate", itemWidth * 2 + 30, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Communion date");
	CreateEdit("commDate", itemWidth * 2 + 30, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");
	SetEditMask("commDate", ES_DATE_MASK);

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	std::wstring currentDate = GetCurrentDate(L"/");

	CreateStatic("sByRev", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"By Rev");
	CreateEdit("byRev", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sDated", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Dated");
	CreateEdit("dated", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, currentDate.c_str());
	SetEditMask("dated", ES_DATE_MASK);


}

void FirstCommunionCertificate::ValidateFields()
{
	std::wstring commDate = GetWindowTextStr(GetControlHWND("commDate")), baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		dated = GetWindowTextStr(GetControlHWND("dated"));

	if (baptizeDate.length() != 10) throw std::wstring(L"Enter valid 'baptize date'");
	if (commDate.length() != 10) throw std::wstring(L"Enter valid 'Communion date'");
	if (dated.length() != 10) throw std::wstring(L"Enter valid birth 'dated'");
}

void FirstCommunionCertificate::DrawCert(PrinterDrawer* printer)
{
	printer->SetNewFont(4.0f, CFONT_LUCIDA_CALLIGRAPHY, FontStyleRegular);

	Months months;

	float adjustY = -24.5f;
	float adjustX = -2.0f;

	std::wstring xOffsetVal, yOffsetVal;

	config.GetValue(L"FCCertPrint", L"xOffset", xOffsetVal);
	config.GetValue(L"FCCertPrint", L"yOffset", yOffsetVal);

	adjustX += GetFloat(xOffsetVal);
	adjustY += GetFloat(yOffsetVal);

	float churchNameY = 68.0f + adjustY;
	float churchNameX = 41.0f + adjustX;

	float churchAdressY = 77.0f + adjustY;
	float churchAdressX = 27.0f + adjustX;

	std::wstring churchAddressStr = orgAddress + L", " + orgCity + L", " + orgState + L", " + orgZip;


	printer->DrawStr(orgName, churchNameX, churchNameY);
	printer->DrawStr(churchAddressStr, churchAdressX, churchAdressY);

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")), residence = GetWindowTextStr(GetControlHWND("residence")),
		baptizedAt = GetWindowTextStr(GetControlHWND("baptizedAt")), baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		fathersName = GetWindowTextStr(GetControlHWND("fathersName")), mothersName = GetWindowTextStr(GetControlHWND("mothersName")),
		commDate = GetWindowTextStr(GetControlHWND("commDate")), byRev = GetWindowTextStr(GetControlHWND("byRev")),
		dated = GetWindowTextStr(GetControlHWND("dated"));


	float mainNameY = 94.5f + adjustY;
	float mainNameX = 30.0f + adjustX;

	float residenceY = 106.0f + adjustY;
	float residenceX = 38.0f + adjustX;

	float baptizedAtY = 118.0f + adjustY;
	float baptizedAtX = 40.0f + adjustX;

	float baptizeDateY = 130.0f + adjustY;
	float baptizeDateDayX = 34.0f + adjustX;
	float baptizeDateMonthX = 70.0f + adjustX;
	float baptizeDateYearX = 115.0f + adjustX;

	float fathersNameY = 142.0f + adjustY;
	float fathersNameX = 33.0f + adjustX;

	float mothersNameY = 154.0f + adjustY;
	float mothersNameX = 20.0f + adjustX;

	float commDateY = 178.0f + adjustY;
	float commDateDayX = 34.0f + adjustX;
	float commDateMonthX = 70.0f + adjustX;
	float commDateYearX = 115.0f + adjustX;

	float byRevY = 196.5f + adjustY;
	float byRevX = 63.0f + adjustX;

	float datedY = 202.0f + adjustY;
	float datedX = 37.0f + adjustX;

	int commMonth = GetInt(commDate.substr(0, 2));
	int commDay = GetInt(commDate.substr(3, 2));
	int commYear = GetInt(commDate.substr(6));

	int bapMonth = GetInt(baptizeDate.substr(0, 2));
	int bapDay = GetInt(baptizeDate.substr(3, 2));
	int bapYear = GetInt(baptizeDate.substr(6));

	std::wstring commMonthStr = months.GetMonthName(commMonth);
	std::wstring bapMonthStr = months.GetMonthName(bapMonth);

	std::wstring commNumEnd = GetDayEnd(commDay);
	std::wstring bapNumEnd = GetDayEnd(bapDay);

	printer->DrawStr(mainName, mainNameX, mainNameY);
	printer->DrawStr(residence, residenceX, residenceY);
	printer->DrawStr(baptizedAt, baptizedAtX, baptizedAtY);

	printer->DrawStr(std::to_wstring(bapDay) + bapNumEnd, baptizeDateDayX, baptizeDateY);
	printer->DrawStr(bapMonthStr, baptizeDateMonthX, baptizeDateY);
	printer->DrawStr(std::to_wstring(bapYear), baptizeDateYearX, baptizeDateY);

	printer->DrawStr(fathersName, fathersNameX, fathersNameY);
	printer->DrawStr(mothersName, mothersNameX, mothersNameY);

	printer->DrawStr(std::to_wstring(commDay) + commNumEnd, commDateDayX, commDateY);
	printer->DrawStr(commMonthStr, commDateMonthX, commDateY);
	printer->DrawStr(std::to_wstring(commYear), commDateYearX, commDateY);

	printer->DrawStr(byRev, byRevX, byRevY);
	printer->DrawStr(dated, datedX, datedY);

}

void FirstCommunionCertificate::DrawFancyCert(PrinterDrawer* printer)
{
	printer->SetNewFont(4.0f, CFONT_LUCIDA_CALLIGRAPHY, FontStyleRegular);

	Months months;
	float yAdjust = 2.0f;
	float xAdjust = 44.0f;

	std::wstring xOffsetVal, yOffsetVal;

	config.GetValue(L"FCFancyCertPrint", L"xOffset", xOffsetVal);
	config.GetValue(L"FCFancyCertPrint", L"yOffset", yOffsetVal);

	xAdjust += GetFloat(xOffsetVal);
	yAdjust += GetFloat(yOffsetVal);

	float churchNameY = 121.0f + yAdjust;
	float churchNameX = 103.0f + xAdjust;

	float churchAdressY = 121.0f + yAdjust;
	float churchAdressX = 175.0f + xAdjust;

	std::wstring churchAddressStr = orgCity + L", " + orgState;

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")), residence = GetWindowTextStr(GetControlHWND("residence")),
		baptizedAt = GetWindowTextStr(GetControlHWND("baptizedAt")), baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		fathersName = GetWindowTextStr(GetControlHWND("fathersName")), mothersName = GetWindowTextStr(GetControlHWND("mothersName")),
		commDate = GetWindowTextStr(GetControlHWND("commDate")), byRev = GetWindowTextStr(GetControlHWND("byRev")),
		dated = GetWindowTextStr(GetControlHWND("dated"));


	float mainNameY = 62.0f + yAdjust;
	float mainNameX = 94.0f + xAdjust;

	float commDateY = 104.0f + yAdjust;
	float commDateDayX = 112.0f + xAdjust;
	float commDateMonthX = 165.0f + xAdjust;
	float commDateYearX = 221.0f + xAdjust;

	float byRevY = 148.0f + yAdjust;
	float byRevX = 150.0f + xAdjust;


	int commMonth = GetInt(commDate.substr(0, 2));
	int commDay = GetInt(commDate.substr(3, 2));
	int commYear = GetInt(commDate.substr(6));

	std::wstring commMonthStr = months.GetMonthName(commMonth);

	std::wstring commNumEnd = GetDayEnd(commDay);


	float name1LineX = 88.0f + xAdjust;
	float name1LineWidth = 147.0f;

	RectF name1Size = printer->MeasureText(mainName);
	mainNameX = name1LineX + (name1LineWidth - name1Size.Width) / 2.0f;

	printer->DrawStr(mainName, mainNameX, mainNameY);

	printer->DrawStr(std::to_wstring(commDay) + commNumEnd, commDateDayX, commDateY);
	printer->DrawStr(commMonthStr, commDateMonthX, commDateY);
	printer->DrawStr(std::to_wstring(commYear), commDateYearX, commDateY);


	printer->DrawStr(L"St. Ann - Parish", churchNameX, churchNameY);
	printer->DrawStr(L"Kosciusko, TX", churchAdressX, churchAdressY);

	printer->DrawStr(byRev, byRevX, byRevY);

}



void FirstCommunionCertificate::ClearAllFields()
{

	SetWindowText(GetControlHWND("mainName"), L"");
	SetWindowText(GetControlHWND("residence"), L"");
	SetWindowText(GetControlHWND("baptizedAt"), L"");
	SetWindowText(GetControlHWND("baptizeDate"), L"");
	SetWindowText(GetControlHWND("fathersName"), L"");
	SetWindowText(GetControlHWND("mothersName"), L"");
	SetWindowText(GetControlHWND("commDate"), L"");
	SetWindowText(GetControlHWND("byRev"), L"");
	SetWindowText(GetControlHWND("dated"), L"");
}

void FirstCommunionCertificate::SaveToHistory(std::function<void(void)> callback)
{

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")),
		residence = GetWindowTextStr(GetControlHWND("residence")),
		baptizedAt = GetWindowTextStr(GetControlHWND("baptizedAt")),
		baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		fathersName = GetWindowTextStr(GetControlHWND("fathersName")),
		mothersName = GetWindowTextStr(GetControlHWND("mothersName")),
		commDate = GetWindowTextStr(GetControlHWND("commDate")),
		byRev = GetWindowTextStr(GetControlHWND("byRev")),
		dated = GetWindowTextStr(GetControlHWND("dated"));

	if (mainName.size() == 0) return;


	PBConnection::Request("saveFirstCommunionCertToHistory",
		{
			{"type", certificateTypeStr},
			{"mainName", mainName},
			{"residence", residence},
			{"baptizedAt", baptizedAt},
			{"baptizeDate", baptizeDate},
			{"fathersName", fathersName},
			{"mothersName", mothersName},
			{"commDate", commDate},
			{"byRev", byRev},
			{"dated", dated}
		},
		[&, callback](PBResponse res)
		{
			if (callback != nullptr) callback();
		}
	);
}

std::vector<std::pair<std::wstring, std::string>> FirstCommunionCertificate::GetFillTableFields()
{
	return fillTableFields;
}

void FirstCommunionCertificate::FillFromHistory(int id)
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
					residence = result["residence"],
					baptizedAt = result["baptizedAt"],
					baptizeDate = result["baptizeDate"],
					fathersName = result["fathersName"],
					mothersName = result["mothersName"],
					communionDate = result["communionDate"],
					rev = result["rev"],
					dated = result["dated"];

				SetWindowText(GetControlHWND("mainName"), mainName.c_str());
				SetWindowText(GetControlHWND("residence"), residence.c_str());
				SetWindowText(GetControlHWND("baptizedAt"), baptizedAt.c_str());
				SetWindowText(GetControlHWND("baptizeDate"), baptizeDate.c_str());
				SetWindowText(GetControlHWND("fathersName"), fathersName.c_str());
				SetWindowText(GetControlHWND("mothersName"), mothersName.c_str());
				SetWindowText(GetControlHWND("commDate"), communionDate.c_str());
				SetWindowText(GetControlHWND("byRev"), rev.c_str());
				SetWindowText(GetControlHWND("dated"), dated.c_str());

			}
		}
	);
}

void FirstCommunionCertificate::SaveToTempData()
{
	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")),
		baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		fathersName = GetWindowTextStr(GetControlHWND("fathersName")),
		mothersName = GetWindowTextStr(GetControlHWND("mothersName")),
		byRev = GetWindowTextStr(GetControlHWND("byRev")),
		dated = GetWindowTextStr(GetControlHWND("dated"));


	tempDataPtr->mainName = mainName;
	tempDataPtr->fathersName = fathersName;
	tempDataPtr->mothersName = mothersName;
	tempDataPtr->baptizeDate = baptizeDate;
	tempDataPtr->dated = dated;
	tempDataPtr->pastor = byRev;

}
void FirstCommunionCertificate::FillFromTempData()
{
	SetWindowText(GetControlHWND("mainName"), tempDataPtr->mainName.c_str());
	SetWindowText(GetControlHWND("baptizeDate"), tempDataPtr->baptizeDate.c_str());
	SetWindowText(GetControlHWND("fathersName"), tempDataPtr->fathersName.c_str());
	SetWindowText(GetControlHWND("mothersName"), tempDataPtr->mothersName.c_str());
	SetWindowText(GetControlHWND("byRev"), tempDataPtr->pastor.c_str());
	SetWindowText(GetControlHWND("dated"), tempDataPtr->dated.c_str());
}