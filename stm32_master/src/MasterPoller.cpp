#include "MasterPoller.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>

MasterPoller::MasterPoller(LibSerial::SerialPort& port, int slave_count, const std::vector<int>& slaves_to_skip)
    : serial_port(port) {
    // Build the initial list of slaves to poll
    for (int i = 1; i <= slave_count; ++i) {
        // Check if the current slave ID is in the skip list
        if (std::find(slaves_to_skip.begin(), slaves_to_skip.end(), i) == slaves_to_skip.end()) {
            std::ostringstream ss;
            ss << std::setw(2) << std::setfill('0') << i;
            this->pending_slaves.push_back(ss.str());
        }
    }
}

void MasterPoller::poll_slaves() {
    if (pending_slaves.empty()) {
        std::cout << "No slaves to poll." << std::endl;
        return;
    }

    auto start_time = std::chrono::steady_clock::now();
    int retries_left = MAX_RETRY_TIMES;
    std::vector<std::string> slaves_to_poll = this->pending_slaves;

    while (!slaves_to_poll.empty() && retries_left > 0) {
        // Build the data payload (the space-separated list of slave IDs)
        std::string data_payload;
        for (const auto& slave_id : slaves_to_poll) {
            data_payload += slave_id + " ";
        }
        data_payload.pop_back(); // Remove the last space

        // Construct the final command string with the "MDATA:" prefix
        std::string command_str = "MDATA:" + data_payload + "\r\n";

        std::cout << "Sending command: " << command_str;
        serial_port.Write(command_str);

        try {
            std::string response;
            serial_port.ReadLine(response, '\n', slaves_to_poll.size() * 1000);

            size_t first_valid = response.find_first_not_of('\0');
            if (first_valid != std::string::npos) {
                std::string response_id = response.substr(first_valid, 2);
                
                auto it = std::remove(slaves_to_poll.begin(), slaves_to_poll.end(), response_id);
                if (it != slaves_to_poll.end()) {
                    std::cout << "  -> Received ACK from: " << response_id << std::endl;
                    slaves_to_poll.erase(it, slaves_to_poll.end());
                }
            }
        } catch (const LibSerial::ReadTimeout&) {
            std::cerr << "  -> Poll timed out. Retrying..." << std::endl;
            retries_left--;
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    if (slaves_to_poll.empty()) {
        std::cout << "\nSuccess! All slaves responded." << std::endl;
    } else {
        std::cerr << "\nError: The following slaves did not respond:" << std::endl;
        for (const auto& id : slaves_to_poll) {
            std::cerr << "- " << id << std::endl;
        }
    }
    std::cout << "Total time: " << elapsed.count() << " ms" << std::endl;
}