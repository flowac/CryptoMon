#include "lib.h"

void LOGF(const char *str, int level)
{
	static bool _INIT_ = false;
	static QFile fd("log.txt");
	static QTextStream outs;
	QString buf1 = QDateTime::currentDateTime().toString() + "\t" + str + "\n";
	QString buf2 = QString::number(QDateTime::currentSecsSinceEpoch()) + " " + str + "\n";

	if (! _INIT_) {
		if (!fd.open(QIODevice::WriteOnly | QIODevice::Text)) return;
		outs.setDevice(&fd);
		_INIT_ = true;
	}

	switch (level) {
	case  0: outs << buf1; outs.flush(); break;
	case  1: outs << str << "\t"; break;
	case  2: outs << buf2; outs.flush(); break;
	case -1: outs << buf1 << "_EOF_\n\n"; _INIT_ = false; fd.close(); break;
	default: outs << "WARNING: Invalid log level\t" << buf1; outs.flush(); break;
	}
}

