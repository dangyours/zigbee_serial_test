#include "MasterPoller.h"
#include "json.hpp" // nlohmann/json library
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include <cmath>     // For ceil

using json = nlohmann::json;

std::string MasterPoller::encode_to_hex_bitmap(const std::vector<std::string>& slaves_to_poll) const
{
    if (slaves_to_poll.empty()) {
        return "";
    }

    // 1. Find the highest slave ID to determine bitmap size
    int max_id = 0;
    for (const auto& id_str : slaves_to_poll) {
        try {
            int id = std::stoi(id_str);
            if (id > max_id) {
                max_id = id;
            }
        } catch (const std::invalid_argument&) {
            // Handle error if string is not a valid number
        }
    }

    // 2. Create the bitmap
    int bitmap_size_bytes = static_cast<int>(ceil(max_id / 8.0));
    std::vector<unsigned char> bitmap(bitmap_size_bytes, 0); // Initialize to all zeros

    for (const auto& id_str : slaves_to_poll) {
        int id = std::stoi(id_str);
        if (id <= 0) continue;

        int byte_index = (id - 1) / 8;
        int bit_index = (id - 1) % 8;

        // Set the bit for this slave ID
        if (byte_index < bitmap.size()) {
            bitmap[byte_index] |= (1 << bit_index);
        }
    }

    // 3. Convert the binary bitmap to a hex string for safe transmission
    std::ostringstream hex_stream;
    for (unsigned char byte : bitmap) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    return hex_stream.str();
}

MasterPoller::MasterPoller(LibSerial::SerialPort &port, int slave_count, const std::string& config_file, const std::vector<int> &slaves_to_skip)
    : serial_port(port)
{
    // Read slaves_to_skip from config file if available
    std::vector<int> config_slaves_to_skip = slaves_to_skip;
    
    try {
        json config;
        std::ifstream inFile(config_file);
        if (inFile.is_open() && inFile.peek() != std::ifstream::traits_type::eof()) {
            inFile >> config;
            
            // Check if polling_settings and slaves_to_skip exist in config
            if (config.contains("polling_settings") && config["polling_settings"].contains("slaves_to_skip")) {
                config_slaves_to_skip = config["polling_settings"]["slaves_to_skip"].get<std::vector<int>>();
                std::cout << "Loaded slaves_to_skip from config: ";
                for (int id : config_slaves_to_skip) {
                    std::cout << id << " ";
                }
                std::cout << std::endl;
            }
        }
        inFile.close();
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not read config file " << config_file << ": " << e.what() << std::endl;
        std::cerr << "Using provided slaves_to_skip parameter." << std::endl;
    }
    
    // Build the initial list of slaves to poll
    for (int i = 1; i <= slave_count; ++i) {
        // Check if the current slave ID is in the skip list
        if (std::find(config_slaves_to_skip.begin(), config_slaves_to_skip.end(), i) == config_slaves_to_skip.end()) {
            std::ostringstream ss;
            ss << std::setw(2) << std::setfill('0') << i;
            this->pending_slaves.push_back(ss.str());
        }
    }
}

void MasterPoller::send_uart_data(std::vector<std::string> slaves_to_poll)
{
    // Instead of a long list of IDs, we now send a compact hex bitmap.
    std::string encoded_payload = encode_to_hex_bitmap(slaves_to_poll);

    // Using a new prefix "MBMP:" (Master BitMaP) to indicate the new format.
    std::string command_str = "MBMP:" + encoded_payload + "\n";

    std::cout << "Sending command: " << command_str;
    serial_port.Write(command_str);
}

void MasterPoller::poll_slaves()
{
    if (pending_slaves.empty()) {
        std::cout << "No slaves to poll." << std::endl;
        return;
    }

    auto start_time = std::chrono::steady_clock::now();
    int retries_left = MAX_RETRY_TIMES;
    std::vector<std::string> slaves_to_poll = this->pending_slaves;
    send_uart_data(slaves_to_poll);

    while (!slaves_to_poll.empty() && retries_left > 0) {
        // Build the data payload (the space-separated list of slave IDs)
        try {
            std::string response;
            serial_port.ReadLine(response, '\n', slaves_to_poll.size() * 120);
            if (strncmp(response.c_str(), "OK", 2) == 0) {
                std::cout << "  -> Received OK response. Waiting for ACK from slaves..." << std::endl;
                serial_port.ReadLine(response, '\n', slaves_to_poll.size() * 600);
            }
            std::cout << "  -> Received response: " << response;

            size_t first_valid = response.find_first_not_of('\0');
            if (first_valid != std::string::npos) {
                std::string response_id = response.substr(first_valid, 2);

                auto it = std::remove(slaves_to_poll.begin(), slaves_to_poll.end(), response_id);
                if (it != slaves_to_poll.end()) {
                    // std::cout << "  -> Received ACK from: " << response_id << std::endl;
                    slaves_to_poll.erase(it, slaves_to_poll.end());
                }
            }
        } catch (const LibSerial::ReadTimeout &) {
            std::cerr << "  -> Poll timed out. Retrying..." << std::endl;
            send_uart_data(slaves_to_poll);
            retries_left--;
        }
    } 

    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    if (slaves_to_poll.empty()) {
        std::cout << "\nSuccess! All slaves responded." << std::endl;
    } else {
        std::cerr << "\nError: The following slaves did not respond:" << std::endl;
        for (const auto &id : slaves_to_poll) {
            std::cerr << "- " << id << std::endl;
        }
    }
    std::cout << "Total time: " << elapsed.count() << " ms" << std::endl;
}