#include "FullCommunionCertificate.h"
#include "../../Components/MonthsStruct.h"

#include "..//ContReports.h"


FullCommunionCertificate::FullCommunionCertificate(HWND hw, HINSTANCE hI)
	: CertCommon(hw, hI, L"FullComm")
{

	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Main Name", "mainName" }));
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Recived Date", "recivedDate" }));
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Birth Date", "birthDate" }));
}

FullCommunionCertificate::~FullCommunionCertificate() {}

bool FullCommunionCertificate::OnCommand(WPARAM wParam)
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
			printer.SetShowContentOffset(true, L"FULLCCertPrint");
			printer.SetDrawPageNum(false);
			printer.SetDrawPrintDate(false);
			printer.SetUserBorder(false);
			printer.SetMinusPrinterBorder(true);
			printer.SetPaperSize(DMPAPER_A5);
			printer.SetSavedPrintingConfig(selectionConfigName);
			printer.SetPrintedCallback(std::bind(&FullCommunionCertificate::OnPrinted, this, std::placeholders::_1));
			if (printer.InitPriner(std::bind(&FullCommunionCertificate::DrawCert, this, std::placeholders::_1), false))
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
			printer.SetShowContentOffset(true, L"FULLCFancyCertPrint");
			printer.SetDrawPageNum(false);
			printer.SetDrawPrintDate(false);
			printer.SetUserBorder(false);
			printer.SetMinusPrinterBorder(true);
			printer.SetPaperSize(DMPAPER_A4);
			printer.SetOrientation(DMORIENT_LANDSCAPE);
			printer.SetSavedPrintingConfig(selectionConfigName);
			printer.SetPrintedCallback(std::bind(&FullCommunionCertificate::OnPrinted, this, std::placeholders::_1));
			if (printer.InitPriner(std::bind(&FullCommunionCertificate::DrawFancyCert, this, std::placeholders::_1), false))
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

void FullCommunionCertificate::AddControls()
{
	SPData winSize = GetWinSize();

	int itemWidth = winSize.width / 4;
	int itemHeight = 30;

	CreateStatic("sTitle", 10, 20, -10, 40, WS_VISIBLE | WS_CHILD, L"Full Communion Cerificate");
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

	CreateStatic("sBaptizedAt", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Baptized at");
	CreateEdit("baptizedAt", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sBaptizeDate", itemWidth * 2 + 30, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Baptize date");
	CreateEdit("baptizeDate", itemWidth * 2 + 30, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");
	SetEditMask("baptizeDate", ES_DATE_MASK);

	CreateStatic("sReceivedDate", itemWidth * 3 + 40, rowTitleY, itemWidth - 50, itemHeight, WS_VISIBLE | WS_CHILD, L"Received date");
	CreateEdit("receivedDate", itemWidth * 3 + 40, rowValY, itemWidth - 50, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");
	SetEditMask("receivedDate", ES_DATE_MASK);


	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	CreateStatic("sDiocese", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Diocese of");
	CreateEdit("diocese", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sBirthDate", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Birth date");
	CreateEdit("birthDate", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");
	SetEditMask("birthDate", ES_DATE_MASK);

	CreateStatic("sBirthPlace", itemWidth * 2 + 30, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Birth place");
	CreateEdit("birthPlace", itemWidth * 2 + 30, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sBaptizeChurch", itemWidth * 3 + 40, rowTitleY, itemWidth - 50, itemHeight, WS_VISIBLE | WS_CHILD, L"Baptize church");
	CreateEdit("baptizeChurch", itemWidth * 3 + 40, rowValY, itemWidth - 50, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;


	CreateStatic("sBaptizeChurchCity", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Baptize church city");
	CreateEdit("baptizeChurchCity", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sSponsor", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Sponsor");
	CreateEdit("sponsor", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sMinister", itemWidth * 2 + 30, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Minister");
	CreateEdit("minister", itemWidth * 2 + 30, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sPastor", itemWidth * 3 + 40, rowTitleY, itemWidth - 50, itemHeight, WS_VISIBLE | WS_CHILD, L"Pastor");
	CreateEdit("pastor", itemWidth * 3 + 40, rowValY, itemWidth - 50, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	std::wstring currentDate = GetCurrentDate(L"/");

	CreateStatic("sDated", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Dated");
	CreateEdit("dated", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, currentDate.c_str());
	SetEditMask("dated", ES_DATE_MASK);

}

void FullCommunionCertificate::ValidateFields()
{
	std::wstring receivedDate = GetWindowTextStr(GetControlHWND("receivedDate")), baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		dated = GetWindowTextStr(GetControlHWND("dated")), birthDate = GetWindowTextStr(GetControlHWND("birthDate"));

	if (baptizeDate.length() != 10) throw std::wstring(L"Enter valid 'baptize date'");
	if (receivedDate.length() != 10) throw std::wstring(L"Enter valid 'Received date'");
	if (birthDate.length() != 10) throw std::wstring(L"Enter valid 'Birth date'");
	if (dated.length() != 10) throw std::wstring(L"Enter valid 'dated'");
}

void FullCommunionCertificate::DrawCert(PrinterDrawer* printer)
{
	Months months;

	float adjustY = -21.0f;
	float adjustX = -2.0f;

	std::wstring xOffsetVal, yOffsetVal;

	config.GetValue(L"FULLCCertPrint", L"xOffset", xOffsetVal);
	config.GetValue(L"FULLCCertPrint", L"yOffset", yOffsetVal);

	adjustX += GetFloat(xOffsetVal);
	adjustY += GetFloat(yOffsetVal);

	float churchNameY = 67.5f + adjustY;
	float churchNameX = 42.0f + adjustX;

	float churchAdressY = 77.0f + adjustY;
	float churchAdressX = 42.0f + adjustX;

	std::wstring churchAddressStr = orgAddress + L", " + orgCity + L", " + orgState + L", " + orgZip;

	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleRegular);

	printer->DrawStr(orgName, churchNameX, churchNameY);
	printer->SetNewFont(3.0f, CFONT_ARIAL, FontStyleRegular);
	printer->DrawStr(churchAddressStr, churchAdressX, churchAdressY);
	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleRegular);


	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")), baptizedAt = GetWindowTextStr(GetControlHWND("baptizedAt")),
		baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")), receivedDate = GetWindowTextStr(GetControlHWND("receivedDate")),
		diocese = GetWindowTextStr(GetControlHWND("diocese")), birthDate = GetWindowTextStr(GetControlHWND("birthDate")),
		birthPlace = GetWindowTextStr(GetControlHWND("birthPlace")), baptizeChurch = GetWindowTextStr(GetControlHWND("baptizeChurch")),
		baptizeChurchCity = GetWindowTextStr(GetControlHWND("baptizeChurchCity")), sponsor = GetWindowTextStr(GetControlHWND("sponsor")),
		minister = GetWindowTextStr(GetControlHWND("minister")), pastor = GetWindowTextStr(GetControlHWND("pastor")), dated = GetWindowTextStr(GetControlHWND("dated"));



	float mainNameY = 106.0f + adjustY;
	float mainNameX = 30.0f + adjustX;

	float baptizedAtY = 118.0f + adjustY;
	float baptizedAtX = 40.0f + adjustX;

	float baptizeDateY = 130.5f + adjustY;
	float baptizeDateDayX = 34.0f + adjustX;
	float baptizeDateMonthX = 70.0f + adjustX;
	float baptizeDateYearX = 115.0f + adjustX;

	float receivedDateY = 164.0f + adjustY;
	float receivedDateDayX = 34.0f + adjustX;
	float receivedDateMonthX = 70.0f + adjustX;
	float receivedDateYearX = 115.0f + adjustX;

	float pastorY = 206.0f + adjustY;
	float pastorX = 33.0f + adjustX;

	float datedY = 176.5f + adjustY;
	float datedX = 37.0f + adjustX;

	int receivedMonth = GetInt(receivedDate.substr(0, 2));
	int receivedDay = GetInt(receivedDate.substr(3, 2));
	int receivedYear = GetInt(receivedDate.substr(6));

	int bapMonth = GetInt(baptizeDate.substr(0, 2));
	int bapDay = GetInt(baptizeDate.substr(3, 2));
	int bapYear = GetInt(baptizeDate.substr(6));

	std::wstring receivedMonthStr = months.GetMonthName(receivedMonth);
	std::wstring bapMonthStr = months.GetMonthName(bapMonth);

	std::wstring receivedNumEnd = GetDayEnd(receivedDay);
	std::wstring bapNumEnd = GetDayEnd(bapDay);

	printer->DrawStr(mainName, mainNameX, mainNameY);
	printer->DrawStr(baptizedAt, baptizedAtX, baptizedAtY);

	printer->DrawStr(std::to_wstring(bapDay) + bapNumEnd, baptizeDateDayX, baptizeDateY);
	printer->DrawStr(bapMonthStr, baptizeDateMonthX, baptizeDateY);
	printer->DrawStr(std::to_wstring(bapYear), baptizeDateYearX, baptizeDateY);

	printer->DrawStr(std::to_wstring(receivedDay) + receivedNumEnd, receivedDateDayX, receivedDateY);
	printer->DrawStr(receivedMonthStr, receivedDateMonthX, receivedDateY);
	printer->DrawStr(std::to_wstring(receivedYear), receivedDateYearX, receivedDateY);

	printer->DrawStr(pastor, pastorX, pastorY);
	printer->DrawStr(dated, datedX, datedY);

}

void FullCommunionCertificate::DrawFancyCert(PrinterDrawer* printer)
{
	Months months;
	float yAdjust = 2.0f;
	float xAdjust = 44.0f;

	std::wstring xOffsetVal, yOffsetVal;

	config.GetValue(L"FULLCFancyCertPrint", L"xOffset", xOffsetVal);
	config.GetValue(L"FULLCFancyCertPrint", L"yOffset", yOffsetVal);

	xAdjust += GetFloat(xOffsetVal);
	yAdjust += GetFloat(yOffsetVal);

	std::wstring churchAddressStr = orgCity + L", " + orgState;

	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleRegular);


	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")), baptizedAt = GetWindowTextStr(GetControlHWND("baptizedAt")),
		baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")), receivedDate = GetWindowTextStr(GetControlHWND("receivedDate")),
		diocese = GetWindowTextStr(GetControlHWND("diocese")), birthDate = GetWindowTextStr(GetControlHWND("birthDate")),
		birthPlace = GetWindowTextStr(GetControlHWND("birthPlace")), baptizeChurch = GetWindowTextStr(GetControlHWND("baptizeChurch")),
		baptizeChurchCity = GetWindowTextStr(GetControlHWND("baptizeChurchCity")), sponsor = GetWindowTextStr(GetControlHWND("sponsor")),
		minister = GetWindowTextStr(GetControlHWND("minister")), pastor = GetWindowTextStr(GetControlHWND("pastor")), dated = GetWindowTextStr(GetControlHWND("dated"));


	float churchNameY = 53.0f + yAdjust;
	float churchNameX = 120.0f + xAdjust;

	float dioceseY = 63.0f + yAdjust;
	float dioceseX = 159.0f + xAdjust;

	float mainNameY = 93.0f + yAdjust;
	float mainNameX = 134.0f + xAdjust;

	float birthDateY = 103.0f + yAdjust;
	float birthDateX = 130.0f + xAdjust;

	float birthPlaceY = 103.0f + yAdjust;
	float birthPlaceX = 188.0f + xAdjust;

	float bapYearY = 113.0f + yAdjust;
	float bapYearX = 153.0f + xAdjust;

	float bapChurchY = 113.0f + yAdjust;
	float bapChurchX = 175.0f + xAdjust;

	float bapChurchAddressY = 123.0f + yAdjust;
	float bapChurchAddressX = 114.0f + xAdjust;

	float datedY = 123.0f + yAdjust;
	float datedX = 190.0f + xAdjust;

	float sponsorY = 158.0f + yAdjust;
	float sponsorX = 165.0f + xAdjust;

	float ministerY = 173.5f + yAdjust;
	float ministerX = 165.0f + xAdjust;

	float pastorY = 189.5f + yAdjust;
	float pastorX = 165.0f + xAdjust;



	int birthMonth = GetInt(birthDate.substr(0, 2));
	int birthDay = GetInt(birthDate.substr(3, 2));
	int birthYear = GetInt(birthDate.substr(6));

	int datedMonth = GetInt(dated.substr(0, 2));
	int datedDay = GetInt(dated.substr(3, 2));
	int datedYear = GetInt(dated.substr(6));

	int receivedMonth = GetInt(receivedDate.substr(0, 2));
	int receivedDay = GetInt(receivedDate.substr(3, 2));
	int receivedYear = GetInt(receivedDate.substr(6));

	int bapMonth = GetInt(baptizeDate.substr(0, 2));
	int bapDay = GetInt(baptizeDate.substr(3, 2));
	int bapYear = GetInt(baptizeDate.substr(6));

	std::wstring receivedMonthStr = months.GetMonthName(receivedMonth);
	std::wstring birthMonthStr = months.GetMonthName(birthMonth);
	std::wstring datedMonthStr = months.GetMonthName(datedMonth);
	std::wstring bapMonthStr = months.GetMonthName(bapMonth);

	std::wstring receivedNumEnd = GetDayEnd(receivedDay);
	std::wstring bapNumEnd = GetDayEnd(bapDay);



	printer->DrawStr(orgName, churchNameX, churchNameY);

	printer->DrawStr(diocese, dioceseX, dioceseY);

	printer->DrawStr(mainName, mainNameX, mainNameY);

	std::wstring birthStr = birthMonthStr + L" " + std::to_wstring(birthDay) + L", " + std::to_wstring(birthYear);
	printer->DrawStr(birthStr, birthDateX, birthDateY);
	printer->DrawStr(birthPlace, birthPlaceX, birthPlaceY);

	printer->DrawStr(std::to_wstring(bapYear), bapYearX, bapYearY);
	printer->DrawStr(baptizeChurch, bapChurchX, bapChurchY);
	printer->DrawStr(baptizeChurchCity, bapChurchAddressX, bapChurchAddressY);

	std::wstring datedStr = datedMonthStr + L" " + std::to_wstring(datedDay) + L", " + std::to_wstring(datedYear);
	printer->DrawStr(datedStr, datedX, datedY);

	printer->DrawStr(sponsor, sponsorX, sponsorY);
	printer->DrawStr(minister, ministerX, ministerY);
	printer->DrawStr(pastor, pastorX, pastorY);
}



void FullCommunionCertificate::ClearAllFields()
{

	SetWindowText(GetControlHWND("mainName"), L"");
	SetWindowText(GetControlHWND("baptizedAt"), L"");
	SetWindowText(GetControlHWND("baptizeDate"), L"");
	SetWindowText(GetControlHWND("receivedDate"), L"");
	SetWindowText(GetControlHWND("diocese"), L"");
	SetWindowText(GetControlHWND("birthDate"), L"");
	SetWindowText(GetControlHWND("birthPlace"), L"");
	SetWindowText(GetControlHWND("baptizeChurch"), L"");
	SetWindowText(GetControlHWND("baptizeChurchCity"), L"");
	SetWindowText(GetControlHWND("sponsor"), L"");
	SetWindowText(GetControlHWND("minister"), L"");
	SetWindowText(GetControlHWND("pastor"), L"");
	SetWindowText(GetControlHWND("dated"), L"");
}

void FullCommunionCertificate::SaveToHistory(std::function<void(void)> callback)
{

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")),
		baptizedAt = GetWindowTextStr(GetControlHWND("baptizedAt")),
		baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		receivedDate = GetWindowTextStr(GetControlHWND("receivedDate")),
		diocese = GetWindowTextStr(GetControlHWND("diocese")),
		birthDate = GetWindowTextStr(GetControlHWND("birthDate")),
		birthPlace = GetWindowTextStr(GetControlHWND("birthPlace")),
		baptizeChurch = GetWindowTextStr(GetControlHWND("baptizeChurch")),
		baptizeChurchCity = GetWindowTextStr(GetControlHWND("baptizeChurchCity")),
		sponsor = GetWindowTextStr(GetControlHWND("sponsor")),
		minister = GetWindowTextStr(GetControlHWND("minister")),
		pastor = GetWindowTextStr(GetControlHWND("pastor")),
		dated = GetWindowTextStr(GetControlHWND("dated"));

	if (mainName.size() == 0) return;


	PBConnection::Request("saveFullCommunionCertToHistory",
		{
			{"type", certificateTypeStr},
			{"mainName", mainName},
			{"baptizedAt", baptizedAt},
			{"baptizeDate", baptizeDate},
			{"receivedDate", receivedDate},
			{"diocese", diocese},
			{"birthDate", birthDate},
			{"birthPlace", birthPlace},
			{"baptizeChurch", baptizeChurch},
			{"baptizeChurchCity", baptizeChurchCity},
			{"sponsor", sponsor},
			{"minister", minister},
			{"pastor", pastor},
			{"dated", dated}
		},
		[&, callback](PBResponse res)
		{
			if (callback != nullptr) callback();
		}
	);
}

std::vector<std::pair<std::wstring, std::string>> FullCommunionCertificate::GetFillTableFields()
{
	return fillTableFields;
}

void FullCommunionCertificate::FillFromHistory(int id)
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
					baptizedAt = result["baptizedAt"],
					baptizeDate = result["baptizeDate"],
					recivedDate = result["recivedDate"],
					deocesOf = result["deocesOf"],
					birthDate = result["birthDate"],
					birthPlace = result["birthPlace"],
					baptizedChurch = result["baptizedChurch"],
					baptizeChurchCity = result["baptizeChurchCity"],
					sponsor = result["sponsor"],
					minister = result["minister"],
					rev = result["rev"],
					dated = result["dated"];



				SetWindowText(GetControlHWND("mainName"), mainName.c_str());
				SetWindowText(GetControlHWND("baptizedAt"), baptizedAt.c_str());
				SetWindowText(GetControlHWND("baptizeDate"), baptizeDate.c_str());
				SetWindowText(GetControlHWND("receivedDate"), recivedDate.c_str());
				SetWindowText(GetControlHWND("diocese"), deocesOf.c_str());
				SetWindowText(GetControlHWND("birthDate"), birthDate.c_str());
				SetWindowText(GetControlHWND("birthPlace"), birthPlace.c_str());
				SetWindowText(GetControlHWND("baptizeChurch"), baptizedChurch.c_str());
				SetWindowText(GetControlHWND("baptizeChurchCity"), baptizeChurchCity.c_str());
				SetWindowText(GetControlHWND("sponsor"), sponsor.c_str());
				SetWindowText(GetControlHWND("minister"), minister.c_str());
				SetWindowText(GetControlHWND("pastor"), rev.c_str());
				SetWindowText(GetControlHWND("dated"), dated.c_str());

			}
		}
	);
}


void FullCommunionCertificate::SaveToTempData()
{
	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")),
		baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		birthDate = GetWindowTextStr(GetControlHWND("birthDate")),
		pastor = GetWindowTextStr(GetControlHWND("pastor")),
		dated = GetWindowTextStr(GetControlHWND("dated"));

	tempDataPtr->mainName = mainName;
	tempDataPtr->birthDate = birthDate;
	tempDataPtr->baptizeDate = baptizeDate;
	tempDataPtr->dated = dated;
	tempDataPtr->pastor = pastor;

}
void FullCommunionCertificate::FillFromTempData()
{
	SetWindowText(GetControlHWND("mainName"), tempDataPtr->mainName.c_str());
	SetWindowText(GetControlHWND("baptizeDate"), tempDataPtr->baptizeDate.c_str());
	SetWindowText(GetControlHWND("birthDate"), tempDataPtr->birthDate.c_str());
	SetWindowText(GetControlHWND("pastor"), tempDataPtr->pastor.c_str());
	SetWindowText(GetControlHWND("dated"), tempDataPtr->dated.c_str());
}