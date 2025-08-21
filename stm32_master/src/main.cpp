#include "IdAllocator.h"
#include "MasterPoller.h"
#include "NetworkManager.h" // Add the new header
#include <iostream>

void print_usage(const std::string& program_name) {
    std::cerr << "Usage: " << program_name << " <port> <mode> [options]\n\n"
              << "Modes:\n"
              << "  allocate              Run the ID allocation server.\n"
              << "  poll <num_slaves>     Poll a number of slave devices.\n"
              << "  network               Establish and check the network.\n" // Add new mode description
              << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    std::string port_name = argv[1];
    std::string mode = argv[2];

    LibSerial::SerialPort serial_port;
    try {
        serial_port.Open(port_name);
        serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    } catch (const std::exception& e) {
        std::cerr << "Error opening serial port " << port_name << ": " << e.what() << std::endl;
        return 1;
    }

    if (mode == "allocate") {
        IdAllocator allocator(serial_port);
        allocator.run();
    } else if (mode == "poll") {
        if (argc < 4) {
            std::cerr << "Error: 'poll' mode requires <num_slaves>." << std::endl;
            print_usage(argv[0]);
            return 1;
        }
        int num_slaves = std::stoi(argv[3]);
        // MasterPoller now reads slaves_to_skip from config.json
        MasterPoller poller(serial_port, num_slaves);
        poller.poll_slaves();
    } else if (mode == "network") { // Add this block for the new mode
        NetworkManager manager(serial_port);
        manager.establish_network();
    } else {
        std::cerr << "Error: Unknown mode '" << mode << "'." << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    serial_port.Close();
    return 0;
}