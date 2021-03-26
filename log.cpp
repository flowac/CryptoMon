#include "lib.h"

void LOGF(const char *str, int level)
{
	static bool _INIT_ = false;
	static QFile fd("log.txt");
	static QTextStream outs;
	if (! _INIT_) {
		if (!fd.open(QIODevice::WriteOnly | QIODevice::Text)) return;
		outs.setDevice(&fd);
		_INIT_ = true;
	}
	switch (level) {
	case  0: outs << str; outs.flush(); break;
	case -1: outs << str << "\n_EOF_\n\n"; _INIT_ = false; fd.close(); break;
	default: break;
	}
}

//Note: First byte determines the height of the rectangle
void SEND(const char *str)
{
	static bool _INIT_ = false;
	static QSerialPort sp;
	if (! _INIT_) {
		char str[BUF_80];
		QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
		LOGF(str);
		while (!ports.isEmpty()) {
			QSerialPortInfo tp = ports.takeFirst();
			snprintf(str, BUF_80, "COM: %u %u %s\n", tp.productIdentifier(),
				tp.vendorIdentifier(), tp.portName().toLocal8Bit().data());
			LOGF(str);

			if (tp.vendorIdentifier() != 9025) continue;
			LOGF("SETTING SERIAL PORT: ");
			sp.setPort(tp);
			sp.setPortName(tp.portName());
			sp.setBaudRate(9600);
			_INIT_ = sp.open(QIODevice::ReadWrite);
			if (_INIT_) LOGF("SUCCESS\n");
			else LOGF("FAIL\n");
			break;
		}
	}
	if (!sp.write(str)) LOGF("Serial write failed\n");
}

