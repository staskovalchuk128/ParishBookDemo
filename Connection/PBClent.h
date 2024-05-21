/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <iostream>
#include <memory>
#include <map>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#define SSL_R_SHORT_READ 219

#pragma warning(push)
#pragma warning(disable : 4996)
#include <boost/asio/ssl.hpp>
#pragma warning(pop)


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio/impl/read.hpp>

using boost::asio::ip::tcp;
class PBResponse;
class PBRequest;

class PBClent : public std::enable_shared_from_this<PBClent>
{
public:
	typedef boost::system::error_code error_code;

	PBClent(boost::asio::io_service& ios, boost::asio::ssl::context& context);

	void Start(boost::asio::ip::tcp::resolver::iterator ep, std::function<void(bool, std::wstring)> callback);

	void Stop(error_code err = {}, bool showErrorMsg = true);
	bool IsStarted();
	void OnConnected(const error_code& err);
	void HandleHandshake(const boost::system::error_code& error);
	void OnReadHeader(std::shared_ptr<PBClent>& s, const error_code& err, size_t bytes);
	void OnReadBody(std::shared_ptr<PBClent>& s, const error_code& err, size_t bytes);
	void HandleWrite(std::shared_ptr<PBClent>& s, const error_code& err, size_t bytes);
	void ReadHeader();
	void ReadBody();
	void SendRequest(std::string msg);

	void WriteImp(std::string msg, std::shared_ptr<PBRequest> request, std::function<void(std::shared_ptr<PBRequest>)> callback = nullptr);
	void ReadImp();

private:
	bool VerifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx);

	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket;
	boost::asio::io_service& ios;
	boost::asio::streambuf readBuffer;
	boost::asio::streambuf headerBuffer;
	std::string readBody;
	int responseSize;
	bool started;
	std::shared_ptr<PBRequest> currentRequest;
	std::function<void(bool, std::wstring)> connectionCallback;
	std::function<void(std::shared_ptr<PBRequest>)> requestCallback;

	std::string sendingMessageStr;
};
