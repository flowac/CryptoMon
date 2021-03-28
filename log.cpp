#include "lib.h"

void LOGF(const char *str, int level)
{
	static bool _INIT_ = false;
	static QFile fd("log.txt");
	static QTextStream outs;
	QString buf = QDateTime::currentDateTime().toString() + "\t" + str + "\n";

	if (! _INIT_) {
		if (!fd.open(QIODevice::WriteOnly | QIODevice::Text)) return;
		outs.setDevice(&fd);
		_INIT_ = true;
	}

	switch (level) {
	case  0: outs << buf; outs.flush(); break;
	case  1: outs << str << "\t"; break;
	case -1: outs << buf << "_EOF_\n\n"; _INIT_ = false; fd.close(); break;
	default: outs << "WARNING: Invalid log level\t" << buf; outs.flush(); break;
	}
}

//Note: First byte determines the height of the rectangle
void SEND(const char *str)
{
	static bool _INIT_ = false;
	static bool _STAT_ = true;
	static QSerialPort sp;

	if (! _INIT_) {
		char str[BUF_80];
		QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
		while (!ports.isEmpty()) {
			QSerialPortInfo tp = ports.takeFirst();
			if (_STAT_) {
				snprintf(str, BUF_80, "COM: %u %u %s", tp.productIdentifier(),
					tp.vendorIdentifier(), tp.portName().toLocal8Bit().data());
				LOGF(str);
				_STAT_ = false;
			}

			if (tp.vendorIdentifier() != 9025) continue;
			LOGF("SETTING SERIAL PORT: ", 1);
			sp.setPort(tp);
			sp.setPortName(tp.portName());
			sp.setBaudRate(9600);
			_INIT_ = sp.open(QIODevice::ReadWrite);
			if (_INIT_) LOGF("SUCCESS");
			else LOGF("FAIL");
			break;
		}
	}
	if (!sp.write(str)) LOGF("Serial write failed");
}

