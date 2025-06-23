#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

int main()
{
    LibSerial::SerialPort serial_port;
    serial_port.Open( "/dev/ttyUSB0" );
    serial_port.SetBaudRate( LibSerial::BaudRate::BAUD_115200 );

    std::string send_data = "1\n";
    std::string receive_data;

    // while (true)
    // {
        auto start_time = std::chrono::system_clock::now();
        serial_port.Write( send_data );
        serial_port.Read( receive_data, send_data.size() );

        auto end_time = std::chrono::system_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count();

        std::time_t now = std::time( nullptr );
        char time_buffer[30];
        std::strftime( time_buffer, sizeof( time_buffer ), "%F %T", std::localtime( &now ) );

        std::cout << time_buffer << " Send: " << send_data << " Receive: " << receive_data << " Elapsed time: " << elapsed_time << " ms" << std::endl;

        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    // }

    serial_port.Close();

    return 0;
}
