#pragma once

#include <iostream>
#include "Basics.hpp"
#include "ASIONetworking.hpp"

struct Message
{
	static constexpr SizeT headerLength = 4;
	SizeT length = 0;
	String data;

	Vector<Byte> serialize()
	{
		length = data.size();
		Vector<Byte> buffer(headerLength + length);
		for(SizeT i = 0; i < headerLength; ++i)
		{
			buffer[i] = (length >> ((headerLength - i - 1) * 8)) & 0xFF;
		}
		std::ranges::copy(data, buffer.begin() + headerLength);
		return buffer;
	}

	static Message deserialize(const Vector<Byte>& buffer)
	{
		Message message;
		for(SizeT i = 0; i < headerLength; ++i)
		{
			message.length |= static_cast<SizeT>(buffer[i]) << ((headerLength - i - 1) * 8);
		}
		message.data.assign(buffer.begin() + headerLength, buffer.end());
		return message;
	}
};

inline void readMessage(asio::ip::tcp::socket& socket, Message& message)
{
	Vector<Byte> headerBuffer(Message::headerLength);
	asio::read(socket, asio::buffer(headerBuffer));
	message = Message::deserialize(headerBuffer);
	Vector<Byte> dataBuffer(message.length);
	asio::read(socket, asio::buffer(dataBuffer));
	message.data.assign(dataBuffer.begin(), dataBuffer.end());
}

inline void writeMessage(asio::ip::tcp::socket& socket, Message& message)
{
	Vector<Byte> buffer = message.serialize();
	asio::write(socket, asio::buffer(buffer));
}

inline void handleRead(const Bool & keepRunning, asio::ip::tcp::socket& socket)
{
	while(keepRunning)
	{
		Message message;
		readMessage(socket, message);
		std::cout << "Received: " << message.data << "\n";
	}
}

inline void handleWrite(const Bool& keepRunning, asio::ip::tcp::socket& socket)
{
	while(keepRunning)
	{
		Message message;
		//std::cout << "Enter message: ";
		std::getline(std::cin, message.data);
		std::cout << "Sending message: " << message.data << "\n";
		writeMessage(socket, message);
	}
}

inline void client()
{
	try
	{
		asio::io_context context;
		asio::ip::tcp::socket socket(context);

		String ipAddress;
		std::cout << "Enter ip address:" << "\n";
		std::cin >> ipAddress;

		socket.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(ipAddress), 12297));
		if(socket.is_open()) std::cout << "Connected to server!" << "\n";

		Thread readThread(handleRead, true, std::ref(socket));
		readThread.detach();

		handleWrite(true, socket);
	}
	catch(const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << "\n";
	}
}

//////////////////////////////////////////////////SERVER/////////////////////////////////////////////////

inline void handleClient(asio::ip::tcp::socket socket)
{
	try
	{
		while(true)
		{
			Message message;
			readMessage(socket, message);
			std::cout << "Received: " << message.data << "\n";
			writeMessage(socket, message);
		}
	}
	catch(const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << "\n";
	}
}

inline void server()
{
	try
	{
		//create context
		asio::io_context ioContext;

		//create acceptor
		asio::ip::tcp::acceptor acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 12297));

		std::cout << "Server started. Listening on port 12297..." << "\n";

		while(true)
		{
			//create new socket
			asio::ip::tcp::socket socket(ioContext);

			//wait for connection
			acceptor.accept(socket);

			//send feedback
			std::cout << "Client connected: " << socket.remote_endpoint().address().to_string() << "\n";

			//create separate thread for client
			std::thread clientThread(handleClient, std::move(socket));

			//and let it run in the background
			clientThread.detach();
		}
	}
	catch(const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << "\n";
	}
}