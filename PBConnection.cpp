#include "PBConnection.h"
#include "Connection/PBClent.h"
#include "Common\CFunctions.hpp"
#include <Windows.h>
#include <boost/thread/thread.hpp>


PBConnection* PBConnection::_self = NULL;
std::shared_ptr<PBClent> PBConnection::bpClient = NULL;
int PBConnection::mainThreadId = 0;
bool PBConnection::currentlyBusy = false;
std::vector<std::shared_ptr<PBRequest>> PBConnection::currentRequests = {};

std::function<void(PBResponse)> PBConnection::prevCallback = nullptr;
std::function<void(bool, std::wstring)> PBConnection::connectedCallback = nullptr;

static HWND mainWinHwnd = NULL;

PBConnection::PBConnection()
{
	_self = this;
}

bool PBConnection::IsConnected()
{
	return _self->bpClient->IsStarted();
}

void PBConnection::ConnectToServer(std::function<void(bool, std::wstring)> callback)
{
	connectedCallback = callback;
	boost::asio::io_service ios;
	boost::asio::io_service::work work(ios);

	boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
	ctx.load_verify_file("clientParishbook.crt");


	boost::asio::ip::tcp::resolver resolver(ios);

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(PB_HOST), PB_PORT);

	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(ep);

	bpClient = std::make_shared<PBClent>(ios, ctx);
	bpClient->Start(iterator, &PBConnection::OnConnected);
	ios.run();
}
void PBConnection::DisconnectFromServer()
{
	bpClient->Stop({}, false);
}

void PBConnection::SetMainThreadId(int id)
{
	mainThreadId = id;
}

void PBConnection::SetMainHWND(void* hWnd)
{
	mainWinHwnd = (HWND)(hWnd);
}

void PBConnection::OnConnected(bool res, std::wstring err)
{
	if (!res)
	{
		if (err == L"system:10061")
		{
			DisconnectFromServer();
			MessageBox(NULL, L"The server is not running", L"Connection error", MB_OK);
			exit(0);
		}
	}
	else
	{
		PostThreadMessage(mainThreadId, MM_ON_SERVER_CONNECTED, (WPARAM)&connectedCallback, (LPARAM)res);
	}
}

void PBConnection::OnResponse(std::shared_ptr<PBRequest> r)
{
	if (!r->aborted)
	{
		BOOL res = PostMessage(mainWinHwnd, MM_REQUEST_RESPONSE, (WPARAM)r.get(), 0);
		if (!res)
		{
			MessageBox(mainWinHwnd, GetLastErrorAsString().c_str(), L"Request error", MB_OK);
		}
	}
	std::vector<std::shared_ptr<PBRequest>>::iterator it = std::find_if(currentRequests.begin(), currentRequests.end(), [r](std::shared_ptr<PBRequest>& item)
		{
			return r.get() == item.get();
		}
	);
	if (it != currentRequests.end()) currentRequests.erase(it);
	currentlyBusy = false;
	CheckQueue();
}

void PBConnection::CheckQueue()
{
	if (currentlyBusy || currentRequests.size() == 0) return;

	if (currentRequests[0]->aborted)
	{
		currentRequests.erase(currentRequests.begin());
		CheckQueue();
	}
	else
	{
		Request(currentRequests[0]->action, currentRequests[0]->params, currentRequests[0]->callback);
		currentRequests.erase(currentRequests.begin());
	}

}

std::shared_ptr<PBRequest> PBConnection::Request(std::string action, std::map<std::string, std::wstring> params, std::function<void(PBResponse)> callback)
{
	AbortAllRequests();

	std::shared_ptr<PBRequest> request = std::make_shared<PBRequest>(action, params, callback, false);
	currentRequests.push_back(request);

	if (currentlyBusy) return request;

	params["timeZoneOffset"] = std::to_wstring(GetTimeZoneOffset());
	currentlyBusy = true;
	prevCallback = callback;
	std::string msg = "{\"action\":\"" + action + "\",\"params\":{";
	for (auto& it : params)
	{
		msg += "\"" + it.first + "\":\"" + WstringToUTF8(it.second) + "\",";
	}
	if(params.size() > 0) msg.pop_back(); // just remove the last comma
	msg += "}}";

	_self->bpClient->WriteImp(msg, request, &PBConnection::OnResponse);

	return request;
}

void PBConnection::AbortAllRequests()
{
	for (auto &it: currentRequests)
	{
		it->aborted = true;
	}
}