#ifndef ID_ALLOCATOR_H
#define ID_ALLOCATOR_H

#include <libserial/SerialPort.h>
#include <string>

class IdAllocator {
public:
    // Constructor takes an open serial port
    explicit IdAllocator(LibSerial::SerialPort& port);

    // Main loop to listen for and handle ID requests
    void run();

private:
    LibSerial::SerialPort& serial_port;
    const std::string config_file = "config.json";

    // Helper function to get or create an ID for an address
    int getIDForAddress(const std::string& address);
};

#endif // ID_ALLOCATOR_H