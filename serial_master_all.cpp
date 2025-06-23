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
#include <algorithm>

#define MAX_RETRY_TIMES 5
#define TIMEOUT_PER_RECEIVE 30

using namespace LibSerial;
using namespace std;

int fail_receive_count = 0;
void receive_data_process(LibSerial::SerialPort &serial_port, vector<string> &slave_nums)
{
    string receive_data;
    vector<string> slave_num_dup = slave_nums;
    int first_valid = 0;
    string receive_data_temp = "OK";
    int max_failure_retry = MAX_RETRY_TIMES;
    std::string slave_num_dup_str;
    while (slave_num_dup.size() && max_failure_retry) {
        try {
            serial_port.ReadLine(receive_data, '\n', slave_num_dup.size()*TIMEOUT_PER_RECEIVE );
//            cout << " Receive: " << receive_data << endl;

            first_valid = receive_data.find_first_not_of('\0');
            if (first_valid != std::string::npos) {
                receive_data_temp = receive_data.substr(first_valid, 2);
//                cout << "receive_data_temp: " << receive_data_temp << endl;
                // cout << "slave size :" << slave_num_dup.size() << endl;
                slave_num_dup.erase(
                    remove_if(slave_num_dup.begin(), slave_num_dup.end(),
                              [&](const string &s) { return s == receive_data_temp; }),
                    slave_num_dup.end());
                // cout << "slave size :" << slave_num_dup.size() << endl;
                if (slave_num_dup.size() == 0) {
                    cout << "get all value success!" << endl;
                    break;
                }
            }
            // receive_data2 = receive_data;
            // if (strncpy(receive_data.c_str(), slave_num.c_str(), 2) == 0) {
            //     break;
            // }
        } catch (const ReadTimeout &) {
            // cout << "Slave" << slave_num << "Receive timeout" << endl;
            max_failure_retry--;
          // cout << " Receive timeout" << endl;
          //  for (int i = 0; i < slave_num_dup.size(); i++) {
          //      cout << "lack reply : " << slave_num_dup[i] << endl;
          //  }
            slave_num_dup_str="";
            for (const auto &str : slave_num_dup) {
                slave_num_dup_str += str + " ";
            }
            slave_num_dup_str.pop_back(); // remove the last space
            slave_num_dup_str.push_back('\n');
            cout << "resend: " <<  slave_num_dup_str << endl;

            serial_port.Write(slave_num_dup_str);
            
            //return;
        }
    }
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

    // std::string send_data = "01\n";
    std::string receive_data;
    auto start_time = std::chrono::system_clock::now();

    int slaves = atoi(argv[2]);

    std::vector<std::string> slave_nums;
    for (int i = 1; i <= slaves; ++i) {
        std::ostringstream ss;
        ss << std::setw(2) << std::setfill('0') << i;
        slave_nums.push_back(ss.str());
    }
    // 将slava_nums 按 空格 组合成字符串
    std::string slave_nums_str;
    for (const auto &str : slave_nums) {
        slave_nums_str += str + " ";
    }
    slave_nums_str.pop_back(); // remove the last space
    slave_nums_str.push_back('\n');
    cout << slave_nums_str << endl;

    serial_port.Write(slave_nums_str);

    receive_data_process(serial_port, slave_nums);
    // send_receive_data(serial_port, slave_nums_str, slave_nums);

    // print slave_nums
    // for (int i = 0; i < slave_nums.size(); ++i) {
    //     std::cout << slave_nums[i] << " ";
    //     send_receive_data(serial_port, slave_nums[i], slave_nums[i]);
    // }
    // std::cout << std::endl;

    auto end_time = std::chrono::system_clock::now();

    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Total elapsed time: " << elapsed_time << " ms" << endl;

    // cout << "Fail receive count: " << fail_receive_count << endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    serial_port.Close();

    return 0;
}
