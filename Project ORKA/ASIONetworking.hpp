#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

using NetworkingContext = asio::io_context;
using NetworkingTimer = asio::steady_timer;
using NetworkingThread = asio::thread;
using NetworkingAddress = asio::ip::address;

using NetworkingUdpEndpoint = asio::ip::udp::endpoint;
using NetworkingUdpSocket = asio::ip::udp::socket;

using NetworkingTcpEndpoint = asio::ip::tcp::endpoint;
using NetworkingTcpSocket = asio::ip::tcp::socket;
using NetworkingBuffer = asio::mutable_buffer;
using NetworkingErrorCode = asio::error_code;
using NetworkingTcpResolver = asio::ip::tcp::resolver;