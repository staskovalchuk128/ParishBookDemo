#include "CitySearcher.h"
#include "..//PBConnection.h"

void CitySearcher::GetInfoByZip(std::wstring zip, std::function<void(PBResponse res)> callback)
{
	PBConnection::Request("getInfoByZip", { {"zip",zip } }, callback);
}