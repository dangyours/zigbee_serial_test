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
    string send_data = argv[2];
    send_data.append("\n");
    // cout << "Slave name: " << argv[2][0] << argv[2][1] << endl;

    // auto start_time = std::chrono::system_clock::now();
    size_t pos = 0;

    while (1) {
        serial_port.ReadLine(receive_data, '\n');
        cout << "Receive orig: " << receive_data << endl;
        
        pos = receive_data.find(argv[2]);
        if (pos != std::string::npos) {
            // cout << "pos: " << pos << endl;
            // if (receive_data.find(argv[2]) != std::string::npos) {
            // std::strftime(time_buffer, sizeof(time_buffer), "%F %T", std::localtime(&now));
            // std::cout << time_buffer;
            // if (slave_number != 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(pos * 5));
            // }
            cout << " Receive: " << receive_data << endl;
            serial_port.Write(send_data);
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
