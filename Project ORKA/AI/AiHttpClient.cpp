#include "AiHttpClient.hpp"

#include "../ASIONetworking.hpp"

#include <sstream>

AiHttpResponse aiHttpRequest(
	const String& host,
	const U16 port,
	const String& method,
	const String& path,
	const String& body,
	const Vector<std::pair<String, String>>& headers)
{
	AiHttpResponse response;
	try
	{
		asio::io_context context;
		asio::ip::tcp::socket socket(context);
		socket.connect({asio::ip::make_address_v4(host), port});

		std::ostringstream requestStream;
		requestStream << method << " " << path << " HTTP/1.1\r\n";
		requestStream << "Host: " << host << ":" << port << "\r\n";
		requestStream << "Connection: close\r\n";
		for (const auto& [name, value] : headers)
			requestStream << name << ": " << value << "\r\n";
		if (!body.empty())
			requestStream << "Content-Length: " << body.size() << "\r\n";
		requestStream << "\r\n";
		if (!body.empty()) requestStream << body;

		const String requestText = requestStream.str();
		asio::write(socket, asio::buffer(requestText));

		asio::streambuf responseBuffer;
		asio::error_code error;
		asio::read(socket, responseBuffer, asio::transfer_all(), error);

		std::istream responseStream(&responseBuffer);
		String statusLine;
		std::getline(responseStream, statusLine);
		if (statusLine.size() >= 12) response.statusCode = std::stoi(statusLine.substr(9, 3));

		String headerLine;
		SizeT contentLength = 0;
		while (std::getline(responseStream, headerLine))
		{
			if (headerLine == "\r" || headerLine.empty()) break;
			if (headerLine.ends_with("\r")) headerLine.pop_back();
			const SizeT colon = headerLine.find(':');
			if (colon == String::npos) continue;
			const String name = headerLine.substr(0, colon);
			String value = headerLine.substr(colon + 1);
			while (!value.empty() && value.front() == ' ') value.erase(value.begin());
			if (name == "Content-Length") contentLength = static_cast<SizeT>(std::stoull(value));
		}

		std::ostringstream bodyStream;
		bodyStream << responseStream.rdbuf();
		response.body = bodyStream.str();
		if (response.body.empty() && contentLength > 0)
			response.body.assign(contentLength, '\0');
	}
	catch (const std::exception& ex)
	{
		response.error = ex.what();
	}
	return response;
}
