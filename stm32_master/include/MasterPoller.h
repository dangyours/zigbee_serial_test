#ifndef MASTER_POLLER_H
#define MASTER_POLLER_H

#include <libserial/SerialPort.h>
#include <string>
#include <vector>

class MasterPoller {
public:
    // Constructor takes the port, number of slaves, and a list of slaves to skip
    MasterPoller(LibSerial::SerialPort& port, int slave_count, const std::vector<int>& slaves_to_skip = {});

    // Main function to perform the polling operation
    void poll_slaves();
    void send_uart_data(std::vector<std::string> slaves_to_poll);

private:
    LibSerial::SerialPort& serial_port;
    std::vector<std::string> pending_slaves;
    std::string encode_to_hex_bitmap(const std::vector<std::string> &slaves_to_poll) const;
    // Constants for configuration
    static const int MAX_RETRY_TIMES = 10;
    static const int TIMEOUT_PER_RECEIVE = 60; // ms
};

#endif // MASTER_POLLER_H