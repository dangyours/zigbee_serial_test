#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace LibSerial;
using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <port_name>" << argv[1] << "hex" << std::endl;
        return 1;
    }
    LibSerial::SerialPort serial_port;
    serial_port.Open(argv[1]);
    serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    serial_port.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
    serial_port.SetParity(LibSerial::Parity::PARITY_NONE);

    std::string send_data = argv[2];
    // send_data.append("\n");
    serial_port.Write("+++");
    std::string receive_data;
    // sleep 1s
    std::this_thread::sleep_for(std::chrono::seconds(1));
    serial_port.Write(send_data);
    while (1) {
        try {
            serial_port.ReadLine(receive_data, '\r\n', 1000);
            cout << "Receive: " << receive_data << endl;
        } catch (const ReadTimeout &) {
            break;
        }
    }

    serial_port.Close();

    return 0;
}
