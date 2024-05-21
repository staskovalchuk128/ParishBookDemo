#include "SacramentsReports.h"

SacramentsReports::~SacramentsReports() {}

SacramentsReports::SacramentsReports(HWND hw, HINSTANCE hI) : PageCommon(hw, hI), Reports() {}

void SacramentsReports::AddControls()
{
	CreateGroupBox("sPs", 10, 30, -1, 410, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, L"Sacrament Reports");

	CreateStatic("ssd", 20, 60, 200, 20, WS_VISIBLE | WS_CHILD, L"Start Date:");
	CreateEdit("startDate", 20, 80, 80, 20, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL);

	CreateStatic("ssd", 120, 60, 200, 20, WS_VISIBLE | WS_CHILD, L"End Date:");
	CreateEdit("endDate", 120, 80, 80, 20, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL);

	HWND fg = CreateListBox("familyGroup", 20, 120, -20, 200, WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY);
	std::vector<std::string> sacTypes = {"Baptism", "Confirmation", "Funeral", "First Eucharist", "Marriage"};

	for (int i = 0; i < sacTypes.size(); i++)
	{
		SendMessage(fg, LB_ADDSTRING, 0, (LPARAM)(sacTypes[i].c_str()));
	}

	SendMessage(fg, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	CreateBtn("print", 20, 60, 100, 20, WS_CHILD | WS_BORDER | WS_VISIBLE, L"Print", WS_STICK_RIGHT);

	SetWindowSubclass(hWnd, SacramentsReportsProc, IDC_STATIC, (DWORD_PTR)this);
	SetWindowSubclass(GetControlHWND("print"), SacramentsReportsProc, GetControlId("print"), (DWORD_PTR)this);
}

void SacramentsReports::OnShowReportView()
{
	//showReportDlg();
}

void SacramentsReports::DrawReport()
{
	/*
	std::map<std::string, std::wstring > parishStat = GetParishStatistic();

	std::wstring famTotal, famWithChilden, famWithoutChilden, famSingleWithChildren, famSingleWithoutChildren,
		totalChildBoys, totalChildGirls, totalKids, totalMale, totalFemale, totalSouls;

	if (parishStat.size() > 0)
	{
		famTotal = parishStat["famTotal"];
		famWithChilden = parishStat["famWithChilden"];
		famWithoutChilden = parishStat["famWithoutChilden"];
		famSingleWithChildren = parishStat["famSingleWithChildren"];
		famSingleWithoutChildren = parishStat["famSingleWithoutChildren"];
		totalKids = parishStat["allChildren"];
		totalChildBoys = parishStat["childBoys"];
		totalChildGirls = parishStat["childGirls"];
		totalMale = parishStat["allMale"];
		totalFemale = parishStat["allFemale"];
		totalSouls = parishStat["allSouls"];
	}

	DrawPageTitle(printer);

	printer->DrawTextPx("Parish Statistical Report", 30, 0, "Arial Rounded MT Bold", 4, "left");
	printer->DrawLine(0, 40, -1, 40.3, BLACK_BRUSH);

	double offsetY = 8, offsetXVal = 40;
	double colTop = 50;
	int leftCt = 20, leftCi = 30;

	double maxOffset = 0;

	printer->DrawTextPx("Households: ", colTop, leftCt, "Arial Rounded MT Bold", 3, "left");

	printer->DrawTextPx("Registered: ", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(famTotal.c_str(), colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);
	printer->DrawTextPx("Un Registered:", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx("0", colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);
	printer->DrawTextPx("Total Households: ", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(famTotal.c_str(), colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);

	printer->DrawTextPx("Members: ", colTop += offsetY, leftCt, "Arial Rounded MT Bold", 3, "left");
	printer->DrawTextPx("Children (0-17): ", colTop += offsetY, leftCt + 5.0, "Arial Rounded MT Bold", 3, "left");
	printer->DrawTextPx("Boys:", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(totalChildBoys.c_str(), colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);
	printer->DrawTextPx("Girls:", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(totalChildGirls.c_str(), colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);
	printer->DrawTextPx("Total Children:", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(totalKids.c_str(), colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);

	printer->DrawTextPx("Total Souls: ", colTop += offsetY, leftCt + 5.0, "Arial Rounded MT Bold", 3, "left");
	printer->DrawTextPx("Male:", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(totalMale.c_str(), colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);
	printer->DrawTextPx("Female:", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(totalFemale.c_str(), colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);
	printer->DrawTextPx("Total Souls:", colTop += offsetY, leftCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(totalSouls.c_str(), colTop, leftCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);

	maxOffset = colTop;

	colTop = 50;
	int rightCt = 110, rightCi = 120;

	printer->DrawTextPx("Households without children: ", colTop, rightCt, "Arial Rounded MT Bold", 3, "left");

	printer->DrawTextPx("Single:", colTop += offsetY, rightCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(famSingleWithoutChildren.c_str(), colTop, rightCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);
	printer->DrawTextPx("Total without children:", colTop += offsetY, rightCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(famWithoutChilden.c_str(), colTop, rightCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);

	printer->DrawTextPx("Households with children: ", colTop += offsetY, rightCt, "Arial Rounded MT Bold", 3, "left");

	printer->DrawTextPx("Single: ", colTop += offsetY, rightCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(famSingleWithChildren.c_str(), colTop, rightCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);
	printer->DrawTextPx("Total with children:", colTop += offsetY, rightCi, "Arial Rounded MT", 3, "left");
	printer->DrawTextPx(famWithChilden.c_str(), colTop, rightCi + offsetXVal, "Arial Rounded MT", 3, "left", DT_RIGHT);

	printer->DrawLine(0, maxOffset + (offsetY *3), -1, maxOffset + (offsetY *3) + 0.2, BLACK_BRUSH);
	*/
}

bool SacramentsReports::OnPrint()
{
	/*
	printer = new Printer(hWnd);

	if (printer->InitPriner() == true)
	{
		DrawReport();
		printer->DonePreDraw();
		DrawReport();
		printer->Print();
	}

	delete printer;
	*/
	return true;

}

INT_PTR CALLBACK SacramentsReports::TestDraw(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg)
	{
	case WM_CTLCOLORDLG:
		return (INT_PTR)GetStockObject(WHITE_BRUSH);
	case WM_INITDIALOG:
	{
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	}

	return (INT_PTR)FALSE;
}

LRESULT CALLBACK SacramentsReports::SacramentsReportsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	SacramentsReports* lpData = (SacramentsReports*)dwRefData;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
		if (uIdSubclass == lpData->GetControlId("hwcSingle") || uIdSubclass == lpData->GetControlId("hwwcSingle") ||
			uIdSubclass == lpData->GetControlId("hmsBoys") || uIdSubclass == lpData->GetControlId("hmsGirls") ||
			uIdSubclass == lpData->GetControlId("hmsTotalChildren") || uIdSubclass == lpData->GetControlId("hmstsMale") ||
			uIdSubclass == lpData->GetControlId("hmstsFemale") || uIdSubclass == lpData->GetControlId("hmstsTotalSouls"))
		{ 		/*
						int reportIndex = static_cast<int>(lpData->con->getControlItem(lpData->con->getControlName(static_cast< int>(uIdSubclass)))->lParam);
						if (reportIndex >= 0) lpData->currentReportType = lpData->reportTypes[reportIndex];
						lpData->onShowReportView();
						return true;
						*/
		}
		else if (uIdSubclass == lpData->GetControlId("print"))
		{
			lpData->OnPrint();

			return true;
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}