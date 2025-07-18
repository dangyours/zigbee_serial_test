#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <libserial/SerialPort.h>

/**
 * @brief Manages the process of establishing and checking a network.
 */
class NetworkManager {
public:
    // Constructor that takes a reference to an already-opened serial port.
    explicit NetworkManager(LibSerial::SerialPort& port);

    // Performs the full network establishment and checking sequence.
    void establish_network();

private:
    LibSerial::SerialPort& serial_port;
};

#endif // NETWORK_MANAGER_H