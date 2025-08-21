#include "IdAllocator.h"
#include "json.hpp" // nlohmann/json library
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

IdAllocator::IdAllocator(LibSerial::SerialPort& port) : serial_port(port) {}

void IdAllocator::run() {
    const std::string GETID_PREFIX = "GETID:";
    std::cout << "ID Allocator is running. Waiting for requests..." << std::endl;

    while (true) {
        try {
            std::string received_line;
            serial_port.ReadLine(received_line, '\n', 15000); // 15-second timeout

            if (received_line.empty()) continue;

            size_t end_pos = received_line.find_last_not_of("\r\n");
            if (end_pos != std::string::npos) {
                received_line = received_line.substr(0, end_pos + 1);
            }

            if (received_line.rfind(GETID_PREFIX, 0) == 0) {
                std::string address = received_line.substr(GETID_PREFIX.length());
                std::cout << "-> Request for address: " << address << std::endl;
                
                int id = getIDForAddress(address);
                
                std::stringstream id_stream;
                id_stream << std::setw(2) << std::setfill('0') << id;
                std::string response = address + ":" + id_stream.str() + "\n";
                
                serial_port.Write(response);
                std::cout << "<- Sent response: " << response;
            }
        } catch (const LibSerial::ReadTimeout&) {
            // It's normal to time out when no device is asking for an ID.
        } catch (const std::exception& e) {
            std::cerr << "ERROR: An exception occurred: " << e.what() << std::endl;
        }
    }
}

// Your getIDForAddress function from the original file goes here
int IdAllocator::getIDForAddress(const std::string& address) {
    json config;
    std::ifstream inFile(config_file);
    if (inFile.is_open() && inFile.peek() != std::ifstream::traits_type::eof()) {
        inFile >> config;
    } else {
        config = {{"next_id", 1}, {"devices", json::object()}};
    }
    inFile.close();

    if (config["devices"].contains(address)) {
        return config["devices"][address].get<int>();
    } else {
        int newId = config["next_id"].get<int>();
        config["devices"][address] = newId;
        config["next_id"] = newId + 1;
        std::ofstream outFile(config_file);
        outFile << std::setw(4) << config << std::endl;
        return newId;
    }
}
