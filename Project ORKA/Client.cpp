#include "Client.hpp"
#include "Debug.hpp"

void Client::run() const
{
	try
	{
		NetworkingContext   context;
		NetworkingTcpSocket socket(context);

		String ipAddress;
		logDebug("Enter ip address:\n");
		std::cin >> ipAddress;

		socket.connect(NetworkingTcpEndpoint(NetworkingAddress::from_string(ipAddress), port));
		if(socket.is_open()) logDebug("Connected to server!\n");

		Thread readThread(handleRead, true, std::ref(socket));
		readThread.detach();

		handleWrite(true, socket);
	}
	catch(const std::exception& ex)
	{
		logError("Exception: " + toString(ex.what()) + "\n");
	}
}

void readMessage(NetworkingTcpSocket& socket, Message& message)
{
	Vector<Byte> headerBuffer(Message::headerLength);
	asio::read(socket, asio::buffer(headerBuffer));

	message = Message::deserialize(headerBuffer);

	Vector<Byte> dataBuffer(message.header.bufferSize);
	asio::read(socket, asio::buffer(dataBuffer));
	message.data.assign(dataBuffer.begin(), dataBuffer.end());
}

void writeMessage(asio::ip::tcp::socket& socket, Message& message)
{
	Vector<Byte> buffer = message.serialize();
	asio::write(socket, asio::buffer(buffer));
}

void handleRead(const Bool& keepRunning, asio::ip::tcp::socket& socket)
{
	while(keepRunning)
	{
		Message message;
		readMessage(socket, message);
		logDebug("Received: " + message.data + "\n");
	}
}

void handleWrite(const Bool& keepRunning, asio::ip::tcp::socket& socket)
{
	while(keepRunning)
	{
		Message message;
		// std::cout << "Enter message: ";
		std::getline(std::cin, message.data);
		std::cout << "Sending message: " << message.data << "\n";
		writeMessage(socket, message);
	}
}
