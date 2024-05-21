#include "ConfirmationCertificate.h"
#include "../../Components/MonthsStruct.h"

#include "..//ContReports.h"


ConfirmationCertificate::ConfirmationCertificate(HWND hw, HINSTANCE hI) : CertCommon(hw, hI, L"Conf")
{
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Main Name", "mainName" }));
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Fathers Name", "fathersName" }));
	fillTableFields.push_back(std::pair<std::wstring, std::string>({ L"Mothers Name", "mothersName" }));
}

ConfirmationCertificate::~ConfirmationCertificate() {}

bool ConfirmationCertificate::OnCommand(WPARAM wParam)
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
			printer.SetShowContentOffset(true, L"ConfCertPrint");
			printer.SetDrawPageNum(false);
			printer.SetDrawPrintDate(false);
			printer.SetUserBorder(false);
			printer.SetMinusPrinterBorder(true);
			printer.SetPaperSize(DMPAPER_A5);
			printer.SetSavedPrintingConfig(selectionConfigName);
			printer.SetPrintedCallback(std::bind(&ConfirmationCertificate::OnPrinted, this, std::placeholders::_1));
			if (printer.InitPriner(std::bind(&ConfirmationCertificate::DrawCert, this, std::placeholders::_1), false))
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
			printer.SetShowContentOffset(true, L"ConfFancyCertPrint");
			printer.SetDrawPageNum(false);
			printer.SetDrawPrintDate(false);
			printer.SetUserBorder(false);
			printer.SetMinusPrinterBorder(true);
			printer.SetPaperSize(DMPAPER_A4);
			printer.SetOrientation(DMORIENT_LANDSCAPE);
			printer.SetSavedPrintingConfig(selectionConfigName);
			printer.SetPrintedCallback(std::bind(&ConfirmationCertificate::OnPrinted, this, std::placeholders::_1));
			if (printer.InitPriner(std::bind(&ConfirmationCertificate::DrawFancyCert, this, std::placeholders::_1), false))
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

void ConfirmationCertificate::AddControls()
{
	SPData winSize = GetWinSize();

	int itemWidth = winSize.width / 4;
	int itemHeight = 30;


	CreateStatic("sTitle", 10, 20, -10, 40, WS_VISIBLE | WS_CHILD, L"Confirmation Cerificate");
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

	CreateStatic("smainName", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Main name");
	CreateEdit("mainName", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sFathersName", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Fathers name");
	CreateEdit("fathersName", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("smothersName", itemWidth * 2 + 30, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Mothers name");
	CreateEdit("mothersName", itemWidth * 2 + 30, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sBaptizeDate", itemWidth * 3 + 40, rowTitleY, itemWidth - 50, itemHeight, WS_VISIBLE | WS_CHILD, L"Baptize date");
	CreateEdit("baptizeDate", itemWidth * 3 + 40, rowValY, itemWidth - 50, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");
	SetEditMask("baptizeDate", ES_DATE_MASK);

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	CreateStatic("sBaptizedChurch", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Baptized church");
	CreateEdit("baptizedChurch", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sRecevedDate", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Receved Date");
	CreateEdit("recevedDate", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");
	SetEditMask("recevedDate", ES_DATE_MASK);

	CreateStatic("sRecevedChurchName", itemWidth * 2 + 30, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"In the Church of");
	CreateEdit("recevedChurchName", itemWidth * 2 + 30, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	CreateStatic("sRecevedChurchAddress", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Church address");
	CreateEdit("recevedChurchAddress", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sDeoceseOf", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Deocese of");
	CreateEdit("deoceseOf", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sMostRev", itemWidth * 2 + 30, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Most Rev");
	CreateEdit("mostRev", itemWidth * 2 + 30, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	CreateStatic("sSponsor", itemWidth * 3 + 40, rowTitleY, itemWidth - 50, itemHeight, WS_VISIBLE | WS_CHILD, L"Sponsor");
	CreateEdit("sponsor", itemWidth * 3 + 40, rowValY, itemWidth - 50, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

	//New row
	rowTitleY = rowValY + itemHeight + rowOffsetY;
	rowValY = rowTitleY + rowTitleValOffsetY;

	std::wstring currentDate = GetCurrentDate(L"/");

	CreateStatic("sDated", 10, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Dated");
	CreateEdit("dated", 10, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, currentDate.c_str());
	SetEditMask("dated", ES_DATE_MASK);

	CreateStatic("sPastor", itemWidth + 20, rowTitleY, itemWidth, itemHeight, WS_VISIBLE | WS_CHILD, L"Issued by");
	CreateEdit("pastor", itemWidth + 20, rowValY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"");

}


void ConfirmationCertificate::ValidateFields()
{
	std::wstring recevedDate = GetWindowTextStr(GetControlHWND("recevedDate")), baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		dated = GetWindowTextStr(GetControlHWND("dated"));

	if (baptizeDate.length() != 10) throw std::wstring(L"Enter valid 'baptize date'");
	if (recevedDate.length() != 10) throw std::wstring(L"Enter valid 'Receved Date'");
	if (dated.length() != 10) throw std::wstring(L"Enter valid birth 'dated'");
}


void ConfirmationCertificate::DrawCert(PrinterDrawer* printer)
{
	printer->SetNewFont(4.0f, CFONT_LUCIDA_CALLIGRAPHY, FontStyleRegular);
	Months months;

	float adjustY = -24.5f;
	float adjustX = -3.0f;

	std::wstring xOffsetVal, yOffsetVal;

	config.GetValue(L"ConfCertPrint", L"xOffset", xOffsetVal);
	config.GetValue(L"ConfCertPrint", L"yOffset", yOffsetVal);

	adjustX += GetFloat(xOffsetVal);
	adjustY += GetFloat(yOffsetVal);

	float churchNameY = 67.0f + adjustY;
	float churchNameX = 44.0f + adjustX;

	float churchAdressY = 76.0f + adjustY;
	float churchAdressX = 27.0f + adjustX;

	std::wstring churchAddressStr = orgAddress + L", " + orgCity + L", " + orgState + L", " + orgZip;


	printer->DrawStr(orgName, churchNameX, churchNameY);
	printer->DrawStr(churchAddressStr, churchAdressX, churchAdressY);

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")), fathersName = GetWindowTextStr(GetControlHWND("fathersName")),
		mothersName = GetWindowTextStr(GetControlHWND("mothersName")), baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		baptizedChurch = GetWindowTextStr(GetControlHWND("baptizedChurch")), recevedDate = GetWindowTextStr(GetControlHWND("recevedDate")),
		recevedChurchName = GetWindowTextStr(GetControlHWND("recevedChurchName")), recevedChurchAddress = GetWindowTextStr(GetControlHWND("recevedChurchAddress")),
		deoceseOf = GetWindowTextStr(GetControlHWND("deoceseOf")), mostRev = GetWindowTextStr(GetControlHWND("mostRev")),
		sponsor = GetWindowTextStr(GetControlHWND("sponsor")), dated = GetWindowTextStr(GetControlHWND("dated")),
		pastor = GetWindowTextStr(GetControlHWND("pastor"));


	float mainNameY = 98.5f + adjustY;
	float mainNameX = 32.0f + adjustX;

	float fathersNameY = 107.5f + adjustY;
	float fathersNameX = 45.0f + adjustX;

	float mothersNameY = 118.0f + adjustY;
	float mothersNameX = 29.0f + adjustX;

	float baptizeDateY = 126 + adjustY;
	float baptizeDateDayX = 55.0f + adjustX;
	float baptizeDateYearX = 115.0f + adjustX;


	float baptizedChurchY = 133.5f + adjustY;
	float baptizedChurchX = 25.0f + adjustX;


	float recevedDateY = 152.7f + adjustY;
	float recevedDateDayX = 75.0f + adjustX;
	float recevedDateMonthX = 30.0f + adjustX;
	float recevedDateYearX = 110.0f + adjustX;

	float recevedChurchNameY = 159.5f + adjustY;
	float recevedChurchNameX = 50.0f + adjustX;

	float recevedChurchAddressY = 167.5f + adjustY;
	float recevedChurchAddressX = 30.0f + adjustX;

	float deoceseOfY = 175.5f + adjustY;
	float deoceseOfX = 50.0f + adjustX;

	float mostRevY = 183.5f + adjustY;
	float mostRevX = 55.0f + adjustX;

	float sponsorY = 191.5f + adjustY;
	float sponsorX = 55.0f + adjustX;

	float datedY = 199.0f + adjustY;
	float datedX = 45.0f + adjustX;

	float pastorY = 208.0f + adjustY;
	float pastorX = 65.0f + adjustX;

	int bapMonth = GetInt(baptizeDate.substr(0, 2));
	int bapDay = GetInt(baptizeDate.substr(3, 2));
	int bapYear = GetInt(baptizeDate.substr(6));

	int recMonth = GetInt(recevedDate.substr(0, 2));
	int recDay = GetInt(recevedDate.substr(3, 2));
	int recYear = GetInt(recevedDate.substr(6));

	std::wstring bapMonthStr = months.GetMonthName(bapMonth);
	std::wstring recMonthStr = months.GetMonthName(recMonth);

	std::wstring bapNumEnd = GetDayEnd(bapDay);
	std::wstring recNumEnd = GetDayEnd(recDay);

	printer->DrawStr(mainName, mainNameX, mainNameY);
	printer->DrawStr(fathersName, fathersNameX, fathersNameY);
	printer->DrawStr(mothersName, mothersNameX, mothersNameY);
	printer->DrawStr(bapMonthStr + L" " + std::to_wstring(bapDay) + bapNumEnd, baptizeDateDayX, baptizeDateY);
	printer->DrawStr(std::to_wstring(bapYear), baptizeDateYearX, baptizeDateY);

	printer->DrawStr(baptizedChurch, baptizedChurchX, baptizedChurchY);

	printer->DrawStr(std::to_wstring(recDay) + recNumEnd, recevedDateDayX, recevedDateY);
	printer->DrawStr(recMonthStr, recevedDateMonthX, recevedDateY);
	printer->DrawStr(std::to_wstring(recYear), recevedDateYearX, recevedDateY);

	printer->DrawStr(recevedChurchName, recevedChurchNameX, recevedChurchNameY);
	printer->DrawStr(recevedChurchAddress, recevedChurchAddressX, recevedChurchAddressY);
	printer->DrawStr(deoceseOf, deoceseOfX, deoceseOfY);
	printer->DrawStr(mostRev, mostRevX, mostRevY);
	printer->DrawStr(sponsor, sponsorX, sponsorY);
	printer->DrawStr(dated, datedX, datedY);
	printer->DrawStr(pastor, pastorX, pastorY);

}

void ConfirmationCertificate::DrawFancyCert(PrinterDrawer* printer)
{
	printer->SetNewFont(4.0f, CFONT_LUCIDA_CALLIGRAPHY, FontStyleRegular);

	Months months;
	float yAdjust = 1.0f;
	float xAdjust = 44.0f;

	std::wstring xOffsetVal, yOffsetVal;

	config.GetValue(L"ConfFancyCertPrint", L"xOffset", xOffsetVal);
	config.GetValue(L"ConfFancyCertPrint", L"yOffset", yOffsetVal);

	xAdjust += GetFloat(xOffsetVal);
	yAdjust += GetFloat(yOffsetVal);

	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")), fathersName = GetWindowTextStr(GetControlHWND("fathersName")),
		mothersName = GetWindowTextStr(GetControlHWND("mothersName")), baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		baptizedChurch = GetWindowTextStr(GetControlHWND("baptizedChurch")), recevedDate = GetWindowTextStr(GetControlHWND("recevedDate")),
		recevedChurchName = GetWindowTextStr(GetControlHWND("recevedChurchName")), recevedChurchAddress = GetWindowTextStr(GetControlHWND("recevedChurchAddress")),
		deoceseOf = GetWindowTextStr(GetControlHWND("deoceseOf")), mostRev = GetWindowTextStr(GetControlHWND("mostRev")),
		sponsor = GetWindowTextStr(GetControlHWND("sponsor")), dated = GetWindowTextStr(GetControlHWND("dated")),
		pastor = GetWindowTextStr(GetControlHWND("pastor"));


	float mainNameY = 70.0f + yAdjust;
	float mainNameX = 93.0f + xAdjust;

	float recevedDateY = 95.5f + yAdjust;
	float recevedDateDayX = 112.0f + xAdjust;
	float recevedDateMonthX = 165.0f + xAdjust;
	float recevedDateYearX = 212.0f + xAdjust;

	float recevedChurchNameY = 115.0f + yAdjust;
	float recevedChurchNameX = 98.0f + xAdjust;

	float recevedChurchAddressY = 115.0f + yAdjust;
	float recevedChurchAddressX = 165.0f + xAdjust;

	float mostRevY = 137.5f + yAdjust;
	float mostRevX = 98.0f + xAdjust;

	int recMonth = GetInt(recevedDate.substr(0, 2));
	int recDay = GetInt(recevedDate.substr(3, 2));
	int recYear = GetInt(recevedDate.substr(6));

	std::wstring recMonthStr = months.GetMonthName(recMonth);

	std::wstring recNumEnd = GetDayEnd(recDay);

	float name1LineX = 89.0f + xAdjust;
	float name1LineWidth = 134.0f;

	RectF name1Size = printer->MeasureText(mainName);
	mainNameX = name1LineX + (name1LineWidth - name1Size.Width) / 2.0f;

	printer->DrawStr(mainName, mainNameX, mainNameY);

	printer->DrawStr(std::to_wstring(recDay) + recNumEnd, recevedDateDayX, recevedDateY);
	printer->DrawStr(recMonthStr, recevedDateMonthX, recevedDateY);
	printer->DrawStr(std::to_wstring(recYear), recevedDateYearX, recevedDateY);

	printer->DrawStr(recevedChurchName, recevedChurchNameX, recevedChurchNameY);
	printer->DrawStr(recevedChurchAddress, recevedChurchAddressX, recevedChurchAddressY);
	printer->DrawStr(mostRev, mostRevX, mostRevY);


}



void ConfirmationCertificate::ClearAllFields()
{

	SetWindowText(GetControlHWND("mainName"), L"");
	SetWindowText(GetControlHWND("fathersName"), L"");
	SetWindowText(GetControlHWND("mothersName"), L"");
	SetWindowText(GetControlHWND("baptizeDate"), L"");
	SetWindowText(GetControlHWND("baptizedChurch"), L"");
	SetWindowText(GetControlHWND("recevedDate"), L"");
	SetWindowText(GetControlHWND("recevedChurchName"), L"");
	SetWindowText(GetControlHWND("recevedChurchAddress"), L"");
	SetWindowText(GetControlHWND("deoceseOf"), L"");
	SetWindowText(GetControlHWND("mostRev"), L"");
	SetWindowText(GetControlHWND("sponsor"), L"");
	SetWindowText(GetControlHWND("dated"), L"");
	SetWindowText(GetControlHWND("pastor"), L"");
}

void ConfirmationCertificate::SaveToHistory(std::function<void(void)> callback)
{
	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")),
		fathersName = GetWindowTextStr(GetControlHWND("fathersName")),
		mothersName = GetWindowTextStr(GetControlHWND("mothersName")),
		baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		baptizedChurch = GetWindowTextStr(GetControlHWND("baptizedChurch")),
		recevedDate = GetWindowTextStr(GetControlHWND("recevedDate")),
		recevedChurchName = GetWindowTextStr(GetControlHWND("recevedChurchName")),
		recevedChurchAddress = GetWindowTextStr(GetControlHWND("recevedChurchAddress")),
		deoceseOf = GetWindowTextStr(GetControlHWND("deoceseOf")),
		mostRev = GetWindowTextStr(GetControlHWND("mostRev")),
		sponsor = GetWindowTextStr(GetControlHWND("sponsor")),
		dated = GetWindowTextStr(GetControlHWND("dated")),
		pastor = GetWindowTextStr(GetControlHWND("pastor"));

	PBConnection::Request("saveConfirmationCertToHistory",
		{
			{"type", certificateTypeStr},
			{"mainName", mainName},
			{"fathersName", fathersName},
			{"mothersName", mothersName},
			{"baptizeDate", baptizeDate},
			{"baptizedChurch", baptizedChurch},
			{"recevedDate", recevedDate},
			{"recevedChurchName", recevedChurchName},
			{"recevedChurchAddress", recevedChurchAddress},
			{"deoceseOf", deoceseOf},
			{"mostRev", mostRev},
			{"sponsor", sponsor},
			{"dated", dated},
			{"pastor", pastor}
		},
		[&, callback](PBResponse res)
		{
			if (callback != nullptr) callback();
		}
	);

}

std::vector<std::pair<std::wstring, std::string>> ConfirmationCertificate::GetFillTableFields()
{
	return fillTableFields;
}

void ConfirmationCertificate::FillFromHistory(int id)
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
					fathersName = result["fathersName"],
					mothersName = result["mothersName"],
					baptizeDate = result["baptizeDate"],
					baptizedChurch = result["baptizedChurch"],
					recivedDate = result["recivedDate"],
					recevedChurchName = result["recevedChurchName"],
					recevedChurchAddress = result["recevedChurchAddress"],
					deocesOf = result["deocesOf"],
					mostRev = result["mostRev"],
					sponsor = result["sponsor"],
					dated = result["dated"],
					rev = result["rev"];

				SetWindowText(GetControlHWND("mainName"), mainName.c_str());
				SetWindowText(GetControlHWND("fathersName"), fathersName.c_str());
				SetWindowText(GetControlHWND("mothersName"), mothersName.c_str());
				SetWindowText(GetControlHWND("baptizeDate"), baptizeDate.c_str());
				SetWindowText(GetControlHWND("baptizedChurch"), baptizedChurch.c_str());
				SetWindowText(GetControlHWND("recevedDate"), recivedDate.c_str());
				SetWindowText(GetControlHWND("recevedChurchName"), recevedChurchName.c_str());
				SetWindowText(GetControlHWND("recevedChurchAddress"), recevedChurchAddress.c_str());
				SetWindowText(GetControlHWND("deoceseOf"), deocesOf.c_str());
				SetWindowText(GetControlHWND("mostRev"), mostRev.c_str());
				SetWindowText(GetControlHWND("sponsor"), sponsor.c_str());
				SetWindowText(GetControlHWND("dated"), dated.c_str());
				SetWindowText(GetControlHWND("pastor"), rev.c_str());

			}
		}
	);
}



void ConfirmationCertificate::SaveToTempData()
{
	std::wstring mainName = GetWindowTextStr(GetControlHWND("mainName")),
		fathersName = GetWindowTextStr(GetControlHWND("fathersName")),
		mothersName = GetWindowTextStr(GetControlHWND("mothersName")),
		baptizeDate = GetWindowTextStr(GetControlHWND("baptizeDate")),
		dated = GetWindowTextStr(GetControlHWND("dated")),
		pastor = GetWindowTextStr(GetControlHWND("pastor"));

	tempDataPtr->mainName = mainName;
	tempDataPtr->fathersName = fathersName;
	tempDataPtr->mothersName = mothersName;
	tempDataPtr->baptizeDate = baptizeDate;
	tempDataPtr->dated = dated;
	tempDataPtr->pastor = pastor;

}
void ConfirmationCertificate::FillFromTempData()
{
	SetWindowText(GetControlHWND("mainName"), tempDataPtr->mainName.c_str());
	SetWindowText(GetControlHWND("fathersName"), tempDataPtr->fathersName.c_str());
	SetWindowText(GetControlHWND("mothersName"), tempDataPtr->mothersName.c_str());
	SetWindowText(GetControlHWND("baptizeDate"), tempDataPtr->baptizeDate.c_str());
	SetWindowText(GetControlHWND("dated"), tempDataPtr->dated.c_str());
	SetWindowText(GetControlHWND("pastor"), tempDataPtr->pastor.c_str());
}