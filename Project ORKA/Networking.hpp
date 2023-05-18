#pragma once

#include "Basics.hpp"
#include "ASIONetworking.hpp"

//#define PORT_NUMBER 12297
//#define PACKET_SIZE_IN_BYTES 1024
//enum { maxPacketSize = 1024 };
//
//struct Server {
//	ASIOContext context;
//	ASIOTCPSocket socket = ASIOTCPSocket(context);
//
//	String readMessage(){
//		Char data[maxPacketSize];
//		
//		ASIOErrorCode error;
//		size_t length = socket.read_some(asio::buffer(data), error);
//
//		//if (error == asio::error::eof)
//		//	break;
//		//else if (error)
//		//	throw asio::system_error(error);
//	};
//
//	void getError() {
//	}
//
//	void writeMessage(String message) {
//		Char data[maxPacketSize];
//
//		std::strcpy(data, message.c_str());
//		asio::write(socket, ASIOBuffer(data, maxPacketSize));
//	};
//
//	Server() {
//		try {
//			asio::ip::tcp::acceptor a(context, ASIOTCPEndpoint(asio::ip::tcp::v4(), PORT_NUMBER));
//			socket = a.accept();
//
//			{
//
//			}
//		}
//		catch (std::exception& e)
//		{
//			std::cerr << "Exception: " << e.what() << "\n";
//		}
//	}
//};
//
//struct Client {
//	ASIOContext context;
//	ASIOTCPSocket socket = ASIOTCPSocket(context);
//
//	Client() {
//		std::cout << "Enter IP: \n";
//		String s;
//		std::cin >> s;
//
//		try
//		{
//			ASIOTCPResolver resolver = ASIOTCPResolver(context);
//
//			auto t = resolver.resolve(s, toString(PORT_NUMBER));
//
//			asio::connect(socket, t);
//		}
//		catch (std::exception& e)
//		{
//			std::cerr << "Exception: " << e.what() << "\n";
//		}
//	}
//
//	String readMessage() {
//		Char reply[maxPacketSize];
//		size_t replyLength = socket.read_some(ASIOBuffer(reply, maxPacketSize));
//		return String(reply);
//	};
//
//	void sendMessage(String message) {
//		Char request[maxPacketSize];
//		std::strcpy(request, message.c_str());
//		size_t requestLength = std::strlen(request);
//		socket.write_some(asio::buffer(request, requestLength));
//	};
//};
//
//extern Vector<Server> servers;
//extern Vector<Client> clients;
