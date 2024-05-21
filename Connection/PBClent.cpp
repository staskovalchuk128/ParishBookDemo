#include "PBClent.h"
#include "../Common/CFunctions.hpp"
#include "..//PBConnection.h"


PBClent::PBClent(boost::asio::io_service& ios, boost::asio::ssl::context& context) : ios(ios), socket(ios, context), started(true), headerBuffer(MAX_HEADER_SIZE)
{
	responseSize = 0;
}

bool PBClent::VerifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	return preverified;
}

void PBClent::Start(boost::asio::ip::tcp::resolver::iterator endpoint_iterator, std::function<void(bool, std::wstring)> callback)
{
}

void PBClent::Stop(error_code err, bool showErrMsg)
{
}
bool PBClent::IsStarted()
{
	return started;
}

void PBClent::OnConnected(const error_code& err)
{
}


void PBClent::HandleHandshake(const boost::system::error_code& err)
{
}

void PBClent::OnReadHeader(std::shared_ptr<PBClent>& s, const error_code& err, size_t bytes)
{
}


void PBClent::OnReadBody(std::shared_ptr<PBClent>& s, const error_code& err, size_t bytes)
{
}

void PBClent::HandleWrite(std::shared_ptr<PBClent>& s, const error_code& err, size_t bytes)
{
}

void PBClent::ReadHeader()
{
	boost::asio::async_read_until(socket, headerBuffer, HEADER_SEP_KEY,
		boost::bind(&PBClent::OnReadHeader, this,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

}

void PBClent::ReadBody()
{
	boost::asio::async_read(socket,
		readBuffer.prepare(responseSize - readBody.size()),
		boost::asio::transfer_all(),
		std::bind(
			&PBClent::OnReadBody, this,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}


void PBClent::SendRequest(std::string msg)
{
}

void PBClent::WriteImp(std::string msg, std::shared_ptr<PBRequest> request, std::function<void(std::shared_ptr<PBRequest>)> callback)
{
}
void PBClent::ReadImp()
{
	ios.post(boost::bind(&PBClent::ReadHeader, this));
}