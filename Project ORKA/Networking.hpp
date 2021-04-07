
#pragma once

#include "Debug.hpp"
#include "Basics.hpp"
#include "Threading.hpp"
#include "GameSimulation.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/thread/thread.hpp>

#define PORT_NUMBER 12297
#define PACKET_SIZE_IN_BYTES 1024

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

using ASIOContext = boost::asio::io_context;
using ASIOTimer = boost::asio::steady_timer;
using ASIOThread = boost::thread;
using ASIOEndpoint = boost::asio::ip::udp::endpoint;
using ASIOSocket = boost::asio::ip::udp::socket;
using ASIOBuffer = boost::asio::mutable_buffer;

struct Server
{
    //networking stuff
    ASIOContext context;
    ASIOEndpoint endpoint = ASIOEndpoint(boost::asio::ip::make_address("127.0.0.1"), 80);
    ASIOSocket socket = ASIOSocket(context);

    //game simulation
    GameSimulation gameSimulation;

    void create();
    void connect();
    void send();
};

struct Message {
    Index messageID = 0;
    UInt bodyByteSize = 0;
    Byte body[PACKET_SIZE_IN_BYTES];
};

struct Client {

};

extern Server server;
extern Client client;