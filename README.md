# CryptoMon
Cross platform crypto monitor with Arduino support

If a correct device was found on the serial port then real time data will be sent periodically.
An OLED display such as 128x32 pixel can be used. See wiki for details.

Requires:
QT5 core

Recommends:
MXE if cross-compiling for Windows.

Compile:
qmake -makefile project.pro
make

