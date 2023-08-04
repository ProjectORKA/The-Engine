#pragma once
#include "ASIONetworking.hpp"
#include "Message.hpp"
#include "Settings.hpp"
#include "Client.hpp"
#include "Debug.hpp"

// inline void handleClient(NetworkingTcpSocket socket)
// {
//    asio::async_read(socket, asio::buffer(buffer),
//        [&](const std::error_code& error, std::size_t bytesRead) {
//            handleRead(error, bytesRead);
//        });
//
//    try
//    {
//        while (true)
//        {
//            Message message;
//            readMessage(socket, message);
//            std::cout << "Received: " << message.data << "\n";
//            writeMessage(socket, message);
//        }
//    }
//    catch (const std::exception& ex)
//    {
//        std::cerr << "Exception: " << ex.what() << "\n";
//    }
// }
//
// inline void acceptConnectionBehaviour(NetworkingTcpAcceptor & acceptor)
// {
//    acceptor.async_accept([&](const std::error_code& error, NetworkingTcpSocket socket)
//    {
//        if (!error)
//        {
//            //std::cout << "Client connected: " << socket.remote_endpoint().address().to_string() << "\n";
//            Thread clientThread(handleClient, std::move(socket));
//            clientThread.detach();
//        }
//        else
//        {
//            logError(error.message());
//        }
//
//        acceptConnectionBehaviour(acceptor);
//    });
// }
//
// struct Server
// {
//    void run() const
//    {
//        try
//        {
//            NetworkingContext context;
//
//            asio::io_context::work idleWork(context);
//
//            Thread asioWorker([&]() { context.run(); });
//
//            NetworkingTcpAcceptor acceptor(context, NetworkingTcpEndpoint(asio::ip::tcp::v4(), port));
//            NetworkingTcpSocket socket(context);
//
//            context.stop();
//            asioWorker.join();
//
//            //acceptConnectionBehaviour(acceptor);
//            //std::cout << "Server started. Listening on port" << port << "..." << "\n";
//
//            //while(true)
//            //{
//            // context.run();
//            // logDebug("1");
//            //}
//        }
//        catch (const std::exception& ex)
//        {
//            std::cerr << "Exception: " << ex.what() << "\n";
//        }
//    }
// };
