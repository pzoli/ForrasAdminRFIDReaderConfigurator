# ForrasAdminRFIDReaderConfigurator

This PlatformIO project use Arduino AtMega-2560, but you can configure this project using Arduino Uno board, too.
Project receive and send JSON strings over the serial port, and configure the EEPROM of board and after that you shoult use [ForrasAdminRFIDReader](https://github.com/pzoli/ForrasAdminRFIDReader) project to forward card IDs over the network to [forras-admin](https://github.com/pzoli/forras-admin) server.