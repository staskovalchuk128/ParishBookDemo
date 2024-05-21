#include "Reports.h"

Reports::Reports()
{
	std::wstring configSection = L"Organization";

	config.GetValue(configSection, L"name", orgName);
	config.GetValue(configSection, L"address", orgAddress);
	config.GetValue(configSection, L"city", orgCity);
	config.GetValue(configSection, L"state", orgState);
	config.GetValue(configSection, L"zip", orgZip);
}

Reports::~Reports() {}

float Reports::DrawPageTitle(PrinterDrawer* printer)
{
	std::wstring cityStr = orgCity + L", " + orgState + L" " + orgZip;

	//make font bold and bigger
	printer->SetNewFont(4.0f, CFONT_ARIAL, FontStyleBold);
	printer->DrawStr(orgName.c_str(), 0, 7.0f, PD_TEXT_ALIGNS::TEXT_ALIGN_CENTER);
	
	//Set default regular font
	printer->SetFontToDefault();
	printer->DrawStr(orgAddress.c_str(), 0, 13.0f, PD_TEXT_ALIGNS::TEXT_ALIGN_CENTER);
	printer->DrawStr(cityStr.c_str(), 0, 18.0f, PD_TEXT_ALIGNS::TEXT_ALIGN_CENTER);

	return 18.0f;
}

void Reports::GetParishStatistic(std::function<void(std::map<std::string, std::wstring>)> onFinished)
{
	PBConnection::Request("getParishStatistic", {}
		, [&, onFinished](PBResponse res)
		{
			onFinished(res.GetResultMap("main"));
		}
	);

}