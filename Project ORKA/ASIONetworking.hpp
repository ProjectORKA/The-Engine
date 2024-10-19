#pragma once

//#ifdef _WIN32
//#define _WIN32_WINNT 0x0A00
//#endif

//#define ASIO_NO_TS_EXECUTORS
//#define ASIO_USE_TS_EXECUTOR_AS_DEFAULT
#define ASIO_STANDALONE
#include "asio.hpp"

using NetworkingContext = asio::io_context;
using UDPSocket = asio::ip::udp::socket;
using UDPEndpoint = asio::ip::udp::endpoint;
using UDPResolver = asio::ip::udp::resolver;