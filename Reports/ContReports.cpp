#include "ContReports.h"
#include "..//PBConnection.h"

ContReports::ContReports(HWND hWnd): Reports()
{
	batchInfo = {};
	batchItems = {};
	currentBatchId = 0;
	this->hWnd = hWnd;
}

ContReports::~ContReports() 
{
}

void ContReports::Print(int batchId)
{
	currentBatchId = batchId;
	printer = std::make_shared<Printer>(hWnd);
	std::wstring docName = L"Contributions - " + GetCurrentDate(L"-");
	printer->SetDocName(docName.c_str());

	PBConnection::Request("getContributionReport",
		{ {"batchId", std::to_wstring(currentBatchId)} }, std::bind(&ContReports::OnDataLoaded, this, std::placeholders::_1)
	);


	if (printer->InitPriner(std::bind(&ContReports::DrawReport, this, std::placeholders::_1)))
	{
		printer->Print();
	}

}

void ContReports::OnDataLoaded(PBResponse res)
{
	batchInfo = res.GetResultMap("batchInfo");
	batchItems = res.GetResultVec("items");

	printer->OnFinishedLoadingReport();
}

void ContReports::DrawReport(PrinterDrawer* printer)
{	

	std::wstring batchDate = batchInfo["batchDate"];
	std::wstring batchName = batchInfo["batchName"];
	std::wstring batchTotalAmount = batchInfo["amount"];

	float prevY = DrawPageTitle(printer);

	printer->DrawStr(L"Contribution Details Report - " + batchName, 0, prevY += 8.0f);
	printer->DrawStr(batchDate, 0, prevY += 4.0f);
	printer->DrawStr(L"Total: $" + GetFloatFormatStr(batchTotalAmount), 0, prevY += 4);


	PrinterTableDrawer* table = printer->GetTable();

	PDTHeader tabHeader;
	tabHeader.Add(20.0f, L"Donor");
	tabHeader.Add(10.0f, L"Env #");
	tabHeader.Add(30.0f, L"Fund");
	tabHeader.Add(20.0f, L"Amount");
	tabHeader.Add(20.0f, L"Date");
	table->SetHeaderData(tabHeader);


	std::map<std::wstring, std::vector<std::vector<std::wstring>>> fundsData;
	std::map<std::wstring, float> totalGivenData;

	for (std::vector<std::map<std::string, std::wstring>>::reverse_iterator it = batchItems.rbegin(); it != batchItems.rend(); ++it)
	{
		std::wstring firstName = (*it)["FirstName"], lastName = (*it)["LastName"], fundName = (*it)["fundName"], amount = (*it)["amount"];

		lastName = lastName.length() >= 4 ? lastName.substr(0, 3) + L"." : lastName;
		firstName = firstName.length() >= 4 ? firstName.substr(0, 3) + L"." : firstName;

		std::wstring contrName = lastName + L" " + firstName;

		if (Trim(contrName).length() == 0)
		{
			contrName = L"Cash";
		}


		std::vector<std::wstring> rowData;
		rowData.push_back(contrName);
		rowData.push_back((*it)["envelope"]);
		rowData.push_back(fundName);
		rowData.push_back(L"$ " + GetFloatFormatStr(amount));
		rowData.push_back((*it)["date"]);


		fundsData[fundName].push_back(rowData);
		if (totalGivenData.find(fundName) == totalGivenData.end()) totalGivenData[fundName] = 0.0f;
		totalGivenData[fundName] += GetFloat(amount);
	}


	int totalFamilies, totalCont = 0;
	int i = 0;
	float prevRowY = 0.0f;

	std::for_each(fundsData.rbegin(), fundsData.rend(), [&](std::pair<std::wstring, std::vector<std::vector<std::wstring>>> item)
		{
			totalFamilies = totalCont = 0;

			if (i == 0)
			{
				printer->DrawStr(L"Fund: " + item.first, 0, prevY += 10);
				table->SetTableOffsets(0.0f, prevY += 10.0f, -1.0f, -1.0f);
				table->DrawHeader();
			}
			else if (i > 0)
			{
				printer->StartNewPage();
				float prevY = DrawPageTitle(printer);

				printer->DrawStr(L"Contribution Details Report - " + batchName, 0, prevY += 8.0f);
				printer->DrawStr(batchDate, 0, prevY += 4.0f);
				printer->DrawStr(L"Total: $" + GetFloatFormatStr(batchTotalAmount), 0, prevY += 4);

				printer->DrawStr(L"Fund: " + item.first, 0, prevY += 10);

				table->SetTableOffsets(0.0f, prevY += 10.0f, -1.0f, -1.0f);
				table->DrawHeader();


			}


			std::for_each(item.second.rbegin(), item.second.rend(), [&](std::vector<std::wstring> rowData)
				{
					prevRowY = table->DrawRow(rowData);
					totalFamilies++;
					totalCont++;
				});


			float totalGiven = totalGivenData[item.first];

			printer->DrawLine(0, prevRowY, -1, 0.2f);

			printer->DrawStr(L"Summary: " + item.first + L", " + batchDate, 0, prevRowY += 4.0f);

			printer->DrawStr(std::to_wstring(totalCont) + L" Contributions", 0, prevRowY += 4.0f);
			printer->DrawStr(L"Total Given: $" + GetFloatFormatStr(std::to_wstring(totalGiven)), 75, prevRowY);

			printer->DrawStr(std::to_wstring(totalFamilies) + L" Families", 0, prevRowY += 4.0f);
			printer->DrawStr(L"Average Family Total: $" + GetFloatFormatStr(std::to_wstring(totalGiven / (float)totalFamilies)), 75, prevRowY);

			i++;
		});

}