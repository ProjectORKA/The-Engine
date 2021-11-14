
#include "Networking.hpp"

//console commands
void commandPrintIP(String s) {
	system("ipconfig");
}
void commandRunClient(String s) {
	runClient();
};
void commandRunServer(String s) {
	std::thread(runServer).detach();
}

struct AddNetworkingConsoleCommands {
	AddNetworkingConsoleCommands() {
		debugConsole.addCommand("ip", "shows this computers ip configuration", commandPrintIP);
		debugConsole.addCommand("runClient", "starts a client that tries to connect to a server", commandRunClient);
		debugConsole.addCommand("runServer", "starts a server thread that clients can connect to", commandRunServer);
	}
};
AddNetworkingConsoleCommands a;

void session(ASIOTCPSocket socket)
{
	try {
		while (true)
		{
			Char data[maxPacketSize];

			ASIOErrorCode error;
			size_t length = socket.read_some(asio::buffer(data), error);
			if (error == asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw asio::system_error(error); // Some other error.
			std::cout << "Client says :";
			std::cout.write(data, length);
			std::cout << std::endl;
			asio::write(socket, ASIOBuffer(data, length));
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}

void runServer() {
	try
	{
		ASIOContext context;

		asio::ip::tcp::acceptor a(context, ASIOTCPEndpoint(asio::ip::tcp::v4(), PORT_NUMBER));
		while (true)
		{
			std::thread(session, a.accept()).detach();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}
void runClient() {
	try
	{

		ASIOContext context;

		std::cout << "Enter IP: \n";
		String s;
		std::cin >> s;

		ASIOTCPSocket socket = ASIOTCPSocket(context);
		ASIOTCPResolver resolver = ASIOTCPResolver(context);

		auto t = resolver.resolve(s, std::to_string(PORT_NUMBER));


		asio::connect(socket, t);

		Sleep(100);

		std::cout << "Enter message: ";
		while (true) {
			char request[maxPacketSize];
			std::cin.getline(request, maxPacketSize);
			size_t request_length = std::strlen(request);
			asio::write(socket, asio::buffer(request, request_length));

			char reply[maxPacketSize];
			size_t reply_length = asio::read(socket, ASIOBuffer(reply, request_length));
			std::cout << "Reply is: ";
			std::cout.write(reply, reply_length);
			std::cout << "\n";
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}