
#pragma once

#include "Basics.hpp"
#include "DebugConsole.hpp"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

#define PORT_NUMBER 12297
#define PACKET_SIZE_IN_BYTES 1024

using ASIOContext = asio::io_context;
using ASIOTimer = asio::steady_timer;
using ASIOThread = asio::thread;
//udp
using ASIOUDPEndpoint = asio::ip::udp::endpoint;
using ASIOUDPSocket = asio::ip::udp::socket;
//tcp
using ASIOTCPEndpoint = asio::ip::tcp::endpoint;
using ASIOTCPSocket = asio::ip::tcp::socket;
using ASIOBuffer = asio::mutable_buffer;
using ASIOErrorCode = asio::error_code;
using ASIOTCPResolver = asio::ip::tcp::resolver;

#include <cstdlib>
#include <thread>
#include <utility>
#include <cstring>
#include <iostream>

enum { maxPacketSize = 1024 };

void runServer();
void runClient();