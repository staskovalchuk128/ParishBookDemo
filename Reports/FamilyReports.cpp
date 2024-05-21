#include "FamilyReports.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"




FamilyReports::FamilyReports(HWND hw, HINSTANCE hI) : PageCommon(hw, hI), Reports() 
{
}

FamilyReports::~FamilyReports() {}

bool FamilyReports::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);
	if (HIWORD(wParam) == BN_CLICKED)
	{
		if (wId == GetControlId("bithdayRep"))
		{
			BirthdayReport(hWnd).ShowDlg();
		}
		else if (wId == GetControlId("EndYearRep"))
		{
			EndYearReport(hWnd).ShowDlg();
		}
		else if (wId == GetControlId("WeddRep"))
		{
			WeddingReport(hWnd).ShowDlg();
		}
		else if (wId == GetControlId("hwcSingle") || wId == GetControlId("hwwcSingle") ||
			wId == GetControlId("hmsBoys") || wId == GetControlId("hmsGirls") ||
			wId == GetControlId("hmsTotalChildren") || wId == GetControlId("hmstsMale") ||
			wId == GetControlId("hmstsFemale") || wId == GetControlId("hmstsTotalSouls") || wId == GetControlId("hmstsTotalFamilies")
			|| wId == GetControlId("shActiveFamilies") || wId == GetControlId("shInactiveFamilies") || wId == GetControlId("shDeceasedFamilies") 
			|| wId == GetControlId("shContributorOnlyFamilies") || wId == GetControlId("shMovedFamilies")
			|| wId == GetControlId("shActiveMembers") || wId == GetControlId("shInactiveMembers")
			|| wId == GetControlId("shDeceasedMembers") || wId == GetControlId("shUnknownMembers"))
		{
			int reportIndex = (int)(GetControlPtr(GetControlName((int)wId))->GetLParam());

			FamReport fr(hWnd);

			if (reportIndex >= 0)
			{
				fr.currentReportType = fr.reportTypes[reportIndex];
				fr.ShowDlg();
			}

			//onShowReportView();
			return true;
		}
	}

	return false;
}

void FamilyReports::AddControls()
{
	CreateBtn("bithdayRep", 30, 70, 130, 30, WS_CHILD | WS_VISIBLE, L"Birthdays", 0);
	CreateBtn("WeddRep", 180, 70, 210, 30, WS_CHILD | WS_VISIBLE, L"Wedding Anniversaries", 0);
	CreateBtn("EndYearRep", 410, 70, 130, 30, WS_CHILD | WS_VISIBLE, L"End Of Year", 0);
	CreateGroupBox("sPs", 10, 30, -10, 100, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, L"Select type of report: ");

	ShowTotalFamReport();
}

void FamilyReports::ShowTotalFamReport()
{

	PreloaderComponent::Start(hWnd);
	GetParishStatistic([&](std::map<std::string, std::wstring> parishStat)
		{
			FamReport fr(hWnd);

			std::wstring famWithChilden, famWithoutChilden, famSingleWithChildren, famSingleWithoutChildren,
				totalChildBoys, totalChildGirls, totalKids, totalMale, totalFemale, totalActiveFamilies, totalInactiveFamilies,
				totalDeceasedFamilies, totalContributorOnlyFamilies, totalMovedFamilies,
				totalActiveMembers, totalInactiveMembers, totalDeceasedMembers, totalUnknownMembers;

			if (parishStat.size() > 0)
			{
				famWithChilden = parishStat["famWithChilden"];
				famWithoutChilden = parishStat["famWithoutChilden"];
				famSingleWithChildren = parishStat["famSingleWithChildren"];
				famSingleWithoutChildren = parishStat["famSingleWithoutChildren"];
				totalKids = parishStat["allChildren"];
				totalChildBoys = parishStat["childBoys"];
				totalChildGirls = parishStat["childGirls"];
				totalMale = parishStat["allMale"];
				totalFemale = parishStat["allFemale"];
				totalActiveFamilies = parishStat["totalActiveFamilies"];
				totalInactiveFamilies = parishStat["totalInactiveFamilies"];
				totalDeceasedFamilies = parishStat["totalDeceasedFamilies"];
				totalContributorOnlyFamilies = parishStat["totalContributorOnlyFamilies"];
				totalMovedFamilies = parishStat["totalMovedFamilies"];
				totalActiveMembers = parishStat["totalActiveMembers"];
				totalInactiveMembers = parishStat["totalInactiveMembers"];
				totalDeceasedMembers = parishStat["totalDeceasedMembers"];
				totalUnknownMembers = parishStat["totalUnknownMembers"];
			}

			int leftX = 100, nLeftX = 20, centerX = 455, nCenterX = 340, rightX = 775, nRightX = 690;

			int leftY = 190;
			int lyMarging = 30;

			int valMarging = 150;


			CreateStatic("shms", nLeftX, leftY, 300, 20, WS_VISIBLE | WS_CHILD, L"Household Member Statistics");
			CreateStatic("shmsChildren", nLeftX, leftY += lyMarging, 150, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Children (0 - 17)");
			//nLeftX = 380;
			CreateStatic("shmsBoys", nLeftX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Boys:");
			CreateStatic("hmsBoys", nLeftX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalChildBoys.c_str());
			GetControlPtr("hmsBoys")->SetLParam(fr.GetRTInd(L"Boys"));
			SetUnderLineText(GetControlHWND("hmsBoys"));
			CreateStatic("shmsGirls", nLeftX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Girls:");
			CreateStatic("hmsGirls", nLeftX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalChildGirls.c_str());
			GetControlPtr("hmsGirls")->SetLParam(fr.GetRTInd(L"Girls"));
			SetUnderLineText(GetControlHWND("hmsGirls"));

			CreateStatic("shmsTotalChildren", nLeftX, leftY += lyMarging, 150, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Total Children:");
			CreateStatic("hmsTotalChildren", nLeftX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalKids.c_str());
			GetControlPtr("hmsTotalChildren")->SetLParam(fr.GetRTInd(L"Total Children"));
			SetUnderLineText(GetControlHWND("hmsTotalChildren"));

			//nLeftX = 350;
			CreateStatic("shmsTotalSouls", nLeftX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Total Souls");
			//nLeftX = 380;
			CreateStatic("shmstsMale", nLeftX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Male:");
			CreateStatic("hmstsMale", nLeftX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalMale.c_str());
			GetControlPtr("hmstsMale")->SetLParam(fr.GetRTInd(L"Males"));
			SetUnderLineText(GetControlHWND("hmstsMale"));
			CreateStatic("shmstsFemale", nLeftX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Female:");
			CreateStatic("hmstsFemale", nLeftX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalFemale.c_str());
			GetControlPtr("hmstsFemale")->SetLParam(fr.GetRTInd(L"Females"));
			SetUnderLineText(GetControlHWND("hmstsFemale"));

			/*
			CreateStatic("shmstsTotalSouls", nLeftX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Total Souls:");
			CreateStatic("hmstsTotalSouls", nLeftX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalSouls.c_str());
			GetControlPtr("hmstsTotalSouls")->SetLParam(fr.GetRTInd(L"Total Members"));
			SetUnderLineText(GetControlHWND("hmstsTotalSouls"));

			CreateStatic("shmstsTotalFamilies", nLeftX, leftY += lyMarging, 120, 20, WS_VISIBLE | WS_CHILD | SS_RIGHT, L"Total Families:");
			CreateStatic("hmstsTotalFamilies", nLeftX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalFamilies.c_str());
			GetControlPtr("hmstsTotalFamilies")->SetLParam(fr.GetRTInd(L"Total Families"));
			SetUnderLineText(GetControlHWND("hmstsTotalFamilies"));
			*/


			leftY = 190;
			valMarging = 150;

			CreateStatic("sfss", nCenterX, leftY, 300, 20, WS_VISIBLE | WS_CHILD, L"Families Statistic");

			CreateStatic("shActiveFamiliess", nCenterX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD, L"Active:");
			CreateStatic("shActiveFamilies", nCenterX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalActiveFamilies.c_str());
			GetControlPtr("shActiveFamilies")->SetLParam(fr.GetRTInd(L"Total Active Families"));
			SetUnderLineText(GetControlHWND("shActiveFamilies"));

			CreateStatic("shInactiveFamiliess", nCenterX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD, L"Inactive:");
			CreateStatic("shInactiveFamilies", nCenterX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalInactiveFamilies.c_str());
			GetControlPtr("shInactiveFamilies")->SetLParam(fr.GetRTInd(L"Total Inactive Families"));
			SetUnderLineText(GetControlHWND("shInactiveFamilies"));

			CreateStatic("shDeceasedFamiliess", nCenterX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD, L"Deceased:");
			CreateStatic("shDeceasedFamilies", nCenterX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalDeceasedFamilies.c_str());
			GetControlPtr("shDeceasedFamilies")->SetLParam(fr.GetRTInd(L"Total Deceased Families"));
			SetUnderLineText(GetControlHWND("shDeceasedFamilies"));

			CreateStatic("shContributorOnlyFamiliess", nCenterX, leftY += lyMarging, 140, 20, WS_VISIBLE | WS_CHILD, L"Contributor Only:");
			CreateStatic("shContributorOnlyFamilies", nCenterX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalContributorOnlyFamilies.c_str());
			GetControlPtr("shContributorOnlyFamilies")->SetLParam(fr.GetRTInd(L"Total Contributor Only Families"));
			SetUnderLineText(GetControlHWND("shContributorOnlyFamilies"));

			CreateStatic("shMovedFamiliess", nCenterX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD, L"Moved:");
			CreateStatic("shMovedFamilies", nCenterX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalMovedFamilies.c_str());
			GetControlPtr("shMovedFamilies")->SetLParam(fr.GetRTInd(L"Total Moved Families"));
			SetUnderLineText(GetControlHWND("shMovedFamilies"));

			leftY = 190;

			CreateStatic("sfms", nRightX, leftY, 300, 20, WS_VISIBLE | WS_CHILD, L"Members Statistic");

			CreateStatic("shActiveMemberss", nRightX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD, L"Active:");
			CreateStatic("shActiveMembers", nRightX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalActiveMembers.c_str());
			GetControlPtr("shActiveMembers")->SetLParam(fr.GetRTInd(L"Total Active Members"));
			SetUnderLineText(GetControlHWND("shActiveMembers"));

			CreateStatic("shInactiveMemberss", nRightX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD, L"Inactive:");
			CreateStatic("shInactiveMembers", nRightX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalInactiveMembers.c_str());
			GetControlPtr("shInactiveMembers")->SetLParam(fr.GetRTInd(L"Total Inactive Members"));
			SetUnderLineText(GetControlHWND("shInactiveMembers"));

			CreateStatic("shDeceasedMemberss", nRightX, leftY += lyMarging, 100, 20, WS_VISIBLE | WS_CHILD, L"Deceased:");
			CreateStatic("shDeceasedMembers", nRightX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalDeceasedMembers.c_str());
			GetControlPtr("shDeceasedMembers")->SetLParam(fr.GetRTInd(L"Total Deceased Members"));
			SetUnderLineText(GetControlHWND("shDeceasedMembers"));

			CreateStatic("shUnknownMemberss", nRightX, leftY += lyMarging, 140, 20, WS_VISIBLE | WS_CHILD, L"Unknown:");
			CreateStatic("shUnknownMembers", nRightX + valMarging + 10, leftY, 50, 20, WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY, totalUnknownMembers.c_str());
			GetControlPtr("shUnknownMembers")->SetLParam(fr.GetRTInd(L"Total Unknown Members"));
			SetUnderLineText(GetControlHWND("shUnknownMembers"));


			CreateGroupBox("sPs", 10, 150, -10, 290, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, L"Parish Statistics");


			//DELETE THIS
			//EndYearReport(hWnd).ShowDlg();

			PreloaderComponent::Stop();
		}
	);


}

void FamilyReports::DrawReport(PrinterDrawer* printer)
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

bool FamilyReports::OnPrint()
{
	
	Printer printer(hWnd);
	if (printer.InitPriner(std::bind(&FamilyReports::DrawReport, this, std::placeholders::_1)))
	{
		printer.Print();
	}
	
	return true;

}