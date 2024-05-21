/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "ReportsRequests.h"
#include "../Controls/TableCtrl.h"
#include "../FamilyDirect/EditFamily.h"
#include "../Components/Printer/Printer.h"
#include "../Common/Config.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

class Reports : public DrawCommon
{
public:
	Reports();
	~Reports();

	//returns the position of the last string in millimeters
	float DrawPageTitle(PrinterDrawer* printer);

	void GetParishStatistic(std::function<void(std::map<std::string, std::wstring>)> onFinished);
protected:
	Config config;
	std::shared_ptr<Printer> printer;
	std::vector<std::map<std::string, std::wstring>> reportData;
	std::wstring orgName, orgAddress, orgCity, orgState, orgZip;
};