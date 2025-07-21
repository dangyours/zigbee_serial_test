#include "NetworkManager.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

NetworkManager::NetworkManager(LibSerial::SerialPort& port) : serial_port(port) {}

void NetworkManager::establish_network() {
    std::cout << "--- Starting Network Establishment Sequence ---" << std::endl;

    // 1. Send "AT" to check basic communication
    std::cout << "Sending command: +AT" << std::endl;
    serial_port.Write("+AT");
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Short wait for device to process

    // 2. Send "AT+JOIN" to open the network
    std::cout << "Sending command: AT+JOIN to open the network." << std::endl;
    serial_port.Write("AT+JOIN");
    
    // 3. Wait for 50 seconds to allow other devices to join
    std::cout << "\nNetwork is open. Waiting for 50 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(50));
    std::cout << "Wait complete." << std::endl;
    
    // 4. Send "AT+NODE?" to request the list of connected nodes
    std::cout << "\nSending command: AT+NODE? to check nodes." << std::endl;
    serial_port.Write("AT+NODE?");

    // 5. Read and print the result from the serial port
    std::cout << "\n--- Network Node List ---" << std::endl;
    try {
        while (true) {
            std::string response;
            // A timeout here is the expected way to know when the device is done sending data.
            serial_port.ReadLine(response, '\n', 2000); // 2-second timeout

            // Clean up leading/trailing whitespace and newlines from the response
            size_t first = response.find_first_not_of(" \t\r\n");
            if (std::string::npos == first) continue; // Skip if the line is only whitespace
            size_t last = response.find_last_not_of(" \t\r\n");
            
            std::cout << response.substr(first, (last - first + 1)) << std::endl;
        }
    } catch (const LibSerial::ReadTimeout&) {
        // This is expected and indicates the end of the data stream.
        std::cout << "--- End of List ---" << std::endl;
        serial_port.Write("AT+EXIT");
    }
    
    std::cout << "\nNetwork establishment process complete." << std::endl;
}