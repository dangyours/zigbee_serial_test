#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

using namespace LibSerial;
using namespace std;
int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <port_name>" << argv[1] << "<slave_number>" << std::endl;
        return 1;
    }
    LibSerial::SerialPort serial_port;
    serial_port.Open(argv[1]);
    serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_115200);

    // std::string send_data = "01\n";
    std::string receive_data;
    std::time_t now = std::time(nullptr);
    char time_buffer[30];
    int slave_number = atoi(argv[2]);
    cout << "Slave number: " << slave_number << endl;
    // cout << "Slave name: " << argv[2][0] << argv[2][1] << endl;

    // auto start_time = std::chrono::system_clock::now();

    while (1) {
        serial_port.ReadLine(receive_data, '\n');
        // cout << "Receive: " << receive_data << endl;
        if (receive_data[0] == argv[2][0] && receive_data[1] == argv[2][1]) {
            // std::strftime(time_buffer, sizeof(time_buffer), "%F %T", std::localtime(&now));
            // std::cout << time_buffer;
            // if (slave_number != 1) {
            //     std::this_thread::sleep_for(std::chrono::seconds(slave_number * 20));
            // }
            cout << " Receive: " << receive_data << endl;
            serial_port.Write(receive_data);
        }
    }

    // auto end_time = std::chrono::system_clock::now();
    // auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // std::time_t now = std::time(nullptr);
    // char time_buffer[30];
    // std::strftime(time_buffer, sizeof(time_buffer), "%F %T", std::localtime(&now));

    // std::cout << time_buffer << " Send: " << send_data << " Receive: " << receive_data << " Elapsed time: " << elapsed_time << " ms" << std::endl;

    // std::this_thread::sleep_for(std::chrono::seconds(1));

    serial_port.Close();

    return 0;
}
