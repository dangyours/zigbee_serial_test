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

int fail_receive_count = 0;
void send_receive_data(LibSerial::SerialPort &serial_port, const std::string &send_data, std::string slave_num)
{
    auto start_time = std::chrono::system_clock::now();
    std::string receive_data = "";
    std::string receive_data2 = "OKOOO";
    serial_port.Write(send_data);
    size_t first_valid = 0;
    cout << " slave_num: " << slave_num << endl;
    while (1) {
        try {
            serial_port.ReadLine(receive_data, '\n', 2000);
            // cout << " Receive: " << receive_data << endl;
            // receive_data2 = receive_data;
            // if (strncpy(receive_data.c_str(), slave_num.c_str(), 2) == 0) {
            //     break;
            // }
        } catch (const ReadTimeout &) {
            // cout << "Slave" << slave_num << "Receive timeout" << endl;
            cout << " Receive timeout" << endl;
            fail_receive_count++;
            return;
        }
        // cout << "slave_num[0] :" << slave_num[0] << " " << "receive_data[0] :" << receive_data[1] << endl;
        //         cout << "receive_data[0] (ASCII): " << static_cast<int>(receive_data[0]) << endl;
        // cout << "receive_data[1] (ASCII): " << static_cast<int>(receive_data[1]) << endl;
        // receive 数据可能会包含前导\0 
        // cout << "receive_data[0] (ASCII): " << static_cast<int>(receive_data[0]) << endl;
        // cout << "receive_data[1] (ASCII): " << static_cast<int>(receive_data[1]) << endl;
        // cout << "receive_data[2] (ASCII): " << static_cast<int>(receive_data[2]) << endl;
        // cout << "receive_data[3] (ASCII): " << static_cast<int>(receive_data[3]) << endl;
        // cout << atoi(slave_num.c_str()) << " " << atoi(receive_data.c_str()) << endl;

        // 跳过 \0 填充字符，直到有数据为止
        first_valid = receive_data.find_first_not_of('\0');
        if (first_valid != std::string::npos) {
            if (receive_data[first_valid] == slave_num[0] && receive_data[first_valid + 1] == slave_num[1]) {
                break;
            }
        }
    }

    auto end_time = std::chrono::system_clock::now();

    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::time_t now = std::time(nullptr);
    char time_buffer[30];
    std::strftime(time_buffer, sizeof(time_buffer), "%F %T", std::localtime(&now));

    std::cout << time_buffer << " Send: " << send_data << " Receive: " << receive_data << " Elapsed time: " << elapsed_time << " ms" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <port_name>" << argv[1] << "<slaves>" << std::endl;
        return 1;
    }
    LibSerial::SerialPort serial_port;
    serial_port.Open(argv[1]);
    serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    serial_port.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
    serial_port.SetParity(LibSerial::Parity::PARITY_NONE);

    std::string send_data = "01\n";
    std::string receive_data;
    auto start_time = std::chrono::system_clock::now();

    int slaves = atoi(argv[2]);

    std::vector<std::string> slave_nums;
    for (int i = 1; i <= slaves; ++i) {
        std::ostringstream ss;
        ss << std::setw(2) << std::setfill('0') << i << "\n";
        slave_nums.push_back(ss.str());
    }

    // print slave_nums
    for (int i = 0; i < slave_nums.size(); ++i) {
        std::cout << slave_nums[i] << " ";
        send_receive_data(serial_port, slave_nums[i], slave_nums[i]);
    }
    std::cout << std::endl;

    auto end_time = std::chrono::system_clock::now();

    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Total elapsed time: " << elapsed_time << " ms" << endl;

    cout << "Fail receive count: " << fail_receive_count << endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    serial_port.Close();

    return 0;
}
