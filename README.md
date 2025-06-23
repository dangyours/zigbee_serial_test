## serial_at

use to control at commnunication. usage: ./serial_at /dev/ttyUSB0 AT+NWK?

## serial_master_all

the master zigbee serial control. usage: ./serial_master_all /dev/ttyUSB0

## serial_slave_all  start_slaves.sh

the serial zigbee serial control. usage: ./start_slaves.sh [start num] [start dev num]

the start_num will response to start_dev_num. If start num is 0, and start_dev_num is /dev/ttyUSB0, that will be
00 /dev/ttyUSB0
01 /dev/ttyUSB1
...


