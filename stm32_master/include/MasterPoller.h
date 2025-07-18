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

private:
    LibSerial::SerialPort& serial_port;
    std::vector<std::string> pending_slaves;

    // Constants for configuration
    static const int MAX_RETRY_TIMES = 10;
    static const int TIMEOUT_PER_RECEIVE = 60; // ms
};

#endif // MASTER_POLLER_H