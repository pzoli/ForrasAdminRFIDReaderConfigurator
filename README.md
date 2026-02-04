# ForrasAdminRFIDReaderConfigurator

This PlatformIO project use Arduino AtMega-2560, but you can configure this project using Arduino Uno board, too.
Project receive and send JSON strings over the serial port, and configure the EEPROM of board and 
after that you shoult use [ForrasAdminRFIDReader](https://github.com/pzoli/ForrasAdminRFIDReader) or [ForrasAdminRFIDReaderWithPN532](https://github.com/pzoli/ForrasAdminRFIDReaderWithPN532) project 
to forward NFC card or cell phone IDs over the network to [login-server](https://github.com/pzoli/login-server).
You can use [NFCReaderSettingsSwing](https://github.com/pzoli/NFCReaderSettingsSwing) JavaSE 8 app to communicate the Uno and configure the board with [ForrasAdminRFIDReaderConfigurator](https://github.com/pzoli/ForrasAdminRFIDReaderConfigurator).