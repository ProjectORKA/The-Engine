#include <asio.hpp>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <cstring>

using asio::ip::udp;
using json = nlohmann::json;

class JsonSender {
public:
    JsonSender(asio::io_context& io_context, const std::string& host, int port)
        : socket_(io_context) {
        udp::resolver resolver(io_context);
        receiver_endpoint_ = *resolver.resolve(udp::v4(), host, std::to_string(port)).begin();
        socket_.open(udp::v4());
    }

    void send_json(const json& jdata) {
        std::string message = jdata.dump();
        uint32_t message_size = static_cast<uint32_t>(message.size());

        // Prepare buffer with size header
        std::vector<char> buffer(sizeof(message_size) + message_size);
        std::memcpy(buffer.data(), &message_size, sizeof(message_size));  // Add size header
        std::memcpy(buffer.data() + sizeof(message_size), message.data(), message_size);  // Add message

        // Send buffer
        socket_.send_to(asio::buffer(buffer), receiver_endpoint_);
        std::cout << "Sent JSON: " << message << " (Size: " << message_size << " bytes)" << std::endl;
    }

private:
    udp::socket socket_;
    udp::endpoint receiver_endpoint_;
};

class JsonReceiver {
public:
    JsonReceiver(asio::io_context& io_context, int port)
        : socket_(io_context, udp::endpoint(udp::v4(), port)) {}

    json receive_json() {
        udp::endpoint sender_endpoint;

        // Receive header to get message size
        uint32_t message_size = 0;
        socket_.receive_from(asio::buffer(&message_size, sizeof(message_size)), sender_endpoint);
        message_size = ntohl(message_size);  // Convert to host byte order if needed

        // Receive full message based on size
        std::vector<char> buffer(message_size);
        size_t received_bytes = 0;
        while (received_bytes < message_size) {
            received_bytes += socket_.receive_from(asio::buffer(buffer.data() + received_bytes, message_size - received_bytes), sender_endpoint);
        }

        // Parse JSON data
        std::string received_message(buffer.data(), message_size);
        json jdata = json::parse(received_message);
        std::cout << "Received JSON: " << received_message << " (Size: " << message_size << " bytes)" << std::endl;
        
        return jdata;
    }

private:
    udp::socket socket_;
};

int main() {
    asio::io_context io_context;

    // Set up sender and receiver with appropriate IP and port
    JsonSender sender(io_context, "127.0.0.1", 9001);
    JsonReceiver receiver(io_context, 9001);

    // JSON data to send
    json jdata = {
        {"message", "This is a test message that could be quite large."},
        {"value", 42},
        {"data", std::string(3000, 'x')}  // Large data payload
    };

    // Send JSON data
    sender.send_json(jdata);

    // Receive JSON data
    json received_data = receiver.receive_json();
    std::cout << "Processed received data: " << received_data.dump(4) << std::endl;

    return 0;
}