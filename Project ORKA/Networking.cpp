
#include "Networking.hpp"

////console commands
//void commandPrintIP(String s) {
//	system("ipconfig");
//}
//
//
////struct AddNetworkingConsoleCommands {
////	AddNetworkingConsoleCommands() {
////		debugConsole.addCommand("ip", "shows this computers ip configuration", commandPrintIP);
////	}
////};
//
////AddNetworkingConsoleCommands a;
//
//void session(ASIOTCPSocket socket)
//{
//	try {
//		while (true)
//		{
//			Char data[maxPacketSize];
//
//			ASIOErrorCode error;
//			size_t length = socket.read_some(asio::buffer(data), error);
//			if (error == asio::error::eof)
//				break; // Connection closed cleanly by peer.
//			else if (error)
//				throw asio::system_error(error); // Some other error.
//			std::cout << "Client says :";
//			std::cout.write(data, length);
//			std::cout << std::endl;
//			asio::write(socket, ASIOBuffer(data, length));
//		}
//	}
//	catch (std::exception& e)
//	{
//		std::cerr << "Exception in thread: " << e.what() << "\n";
//	}
//}