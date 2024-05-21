/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "Connection/PBResponse.h"

#define PB_HOST "xxx.xxx.xxx.xxx"
#define PB_PORT 0000

#define MM_ON_SERVER_CONNECTED	 12281
#define MM_REQUEST_RESPONSE		 12282

class PBClent;
class PBRequest
{
public:
	PBRequest(std::string action, std::map<std::string, std::wstring> params, std::function<void(PBResponse)> callback, bool aborted)
		: action(action), params(params), callback(callback), aborted(aborted) {}
	std::string action;
	std::map<std::string, std::wstring> params;
	std::function<void(PBResponse)> callback;
	bool aborted;
	PBResponse response;
	
	void NotifyRequestFailed()
	{
		if(failedCallback != nullptr) failedCallback();
	}
	void OnFailed(std::function<void(void)> failedCallback)
	{
		this->failedCallback = failedCallback;
	}
private:
	std::function<void(void)> failedCallback = nullptr;
};

class PBConnection
{
public:
	PBConnection();
	void ConnectToServer(std::function<void(bool, std::wstring)> callback);
	static void DisconnectFromServer();
	static bool IsConnected();

	static std::shared_ptr<PBRequest> Request(std::string action, std::map<std::string, std::wstring> params, std::function<void(PBResponse)> callback);
	static void OnResponse(std::shared_ptr<PBRequest> r);

	static void SetMainHWND(void* hWnd);
	static void SetMainThreadId(int id);
	static void AbortAllRequests();
private:
	static std::shared_ptr<PBClent> bpClient;
	static PBConnection* _self;

	static void OnConnected(bool res, std::wstring err);

	static std::function<void(bool, std::wstring)> connectedCallback;
	static std::function<void(PBResponse)> prevCallback;
	static std::vector<std::shared_ptr<PBRequest>> currentRequests;
	static int mainThreadId;
	static bool currentlyBusy;


	static void CheckQueue();
};

