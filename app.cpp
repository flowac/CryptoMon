#include "lib.h"

App::App()
{
	QString base, css1, css2, css3, css4, fontsize, logsize;
	QBrush brush(QColor(0,0,0), Qt::SolidPattern);
	QPalette cback = QPalette(QPalette::Background, Qt::black);
	QSizePolicy szpol = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QLabel *site = new QLabel("Source: 1btc.win", this);
	QTimer *systimer = new QTimer(this);
	QPushButton *but1, *but2, *but4;
	QWidget *hbox1, *vbox1;
	QHBoxLayout *hlay1, *hlay2;
	QVBoxLayout *vlay1;

	base = "red";
	fontsize = QString::number(FONT_SZ);
	logsize = QString::number(LOG_SZ);
	css1 = "background: black; border:1px solid " + base + "; color: " + base + "; font-size: " + fontsize + "px;";//button
	css2 = "background: black; border:1px solid purple; color: " + base + "; font-size: " + fontsize + "px;";//line edit
	css3 = "background: black; border:0px; color: " + base + "; font-size: " + fontsize + "px;";//label
	css4 = "background: black; border:0px; color: " + base + "; font-family: monospace; font-size: " + logsize + "px;";//text area

	hlay2	= new QHBoxLayout(this);
	ctext	= new QTextEdit("Empty", this);
	vbox1	= new QWidget(this);
	vlay1	= new QVBoxLayout(vbox1);
	hbox1	= new QWidget(vbox1);
	cview1	= new Chart(vbox1);
	cview2	= new Chart(vbox1);
	hlay1	= new QHBoxLayout(hbox1);
	combo1	= new QComboBox(hbox1);
	combo2	= new QComboBox(hbox1);
	syst	= new QLabel(QDateTime::currentDateTime().toString(), hbox1);
	tick	= new QLabel(hbox1);
//	fpath	= new QLineEdit(hbox1);
	but1	= new QPushButton("Exit", hbox1);
	but2	= new QPushButton("Search", hbox1);
//	but3	= new QPushButton("Load", hbox1);
	but4	= new QPushButton("Save", hbox1);


	hbox1->setFixedHeight(TAB_H);
	hlay1->setContentsMargins(SPACE, 0, SPACE, 0);
	hlay2->setContentsMargins(0, 0, 0, 0);
	vlay1->setContentsMargins(0, 0, 0, 0);
	hlay1->setSpacing(SPACE);
	hlay2->setSpacing(0);
	vlay1->setSpacing(0);

	but1->setFixedWidth(TAB_H*4);
	but2->setFixedWidth(TAB_H*4);
//	but3->setFixedWidth(TAB_H*4);
	but4->setFixedWidth(TAB_H*4);
	combo1->setFixedWidth(TAB_H*5);
	combo2->setFixedWidth(TAB_H*5);
	ctext->setFixedWidth(FONT_SZ*10);
//	fpath->setFixedWidth(TAB_H*6);
	syst->setFixedWidth(TAB_H*12);
	syst->setAlignment(Qt::AlignCenter);
	tick->setFixedWidth(TAB_H*15);
	tick->setAlignment(Qt::AlignCenter);
	site->setFixedWidth(TAB_H*6);
	site->setAlignment(Qt::AlignCenter);

	but1->setSizePolicy(szpol);
	but2->setSizePolicy(szpol);
//	but3->setSizePolicy(szpol);
	but4->setSizePolicy(szpol);
	cview1->setSizePolicy(szpol);
	cview2->setSizePolicy(szpol);
	combo1->setSizePolicy(szpol);
	combo2->setSizePolicy(szpol);
//	fpath->setSizePolicy(szpol);
	but1->setStyleSheet(css1);
	but2->setStyleSheet(css1);
//	but3->setStyleSheet(css1);
	but4->setStyleSheet(css1);
	ctext->setStyleSheet(css4);
	cview1->setStyleSheet(css3);
	cview2->setStyleSheet(css3);
	combo1->setStyleSheet(css1);
	combo2->setStyleSheet(css1);
//	fpath->setStyleSheet(css1);
	syst->setStyleSheet(css3);
	tick->setStyleSheet(css3);
	site->setStyleSheet(css3);

	hlay1->addWidget(but1);
	hlay1->addWidget(combo1);
	hlay1->addWidget(combo2);
	hlay1->addWidget(but2);
//	hlay1->addWidget(but3);
//	hlay1->addWidget(fpath);
	hlay1->addWidget(but4);
	hlay1->addWidget(site);
	hlay1->addWidget(tick);
	hlay1->addWidget(syst);
	hbox1->setLayout(hlay1);

	vlay1->addWidget(hbox1);
	cview1->ctype = "None";
	cview2->ctype = "None";
	vlay1->addWidget(cview1);
	vlay1->addWidget(cview2);
	vbox1->setLayout(vlay1);

	hlay2->addWidget(ctext);
	hlay2->addWidget(vbox1);

	combo1->addItem("XXBTZUSD");
	combo1->addItem("XETHZUSD");
	combo1->addItem("XXBTZEUR");
	combo1->addItem("");
	combo1->setEditable(true);
	combo1->setInsertPolicy(QComboBox::InsertAtTop);

	combo2->addItem("XETHXXBT");
	combo2->addItem("XETHZUSD");
	combo2->addItem("");
	combo2->setEditable(true);
	combo2->setInsertPolicy(QComboBox::InsertAtTop);
//	fpath->setText(combo->currentText() + ".csv");

	connect(but1, SIGNAL(released()), this, SLOT(_quit()));
	connect(but2, SIGNAL(released()), this, SLOT(_get_pair()));
//	connect(but3, SIGNAL(released()), this, SLOT();
	connect(but4, SIGNAL(released()), this, SLOT(_save()));
//	connect(combo, SIGNAL(currentTextChanged(QString)), this, SLOT(_fpath(QString)));
	connect(systimer, SIGNAL(timeout()), this, SLOT(_time()));
	systimer->start(1000);

	this->setPalette(cback);
	this->setWindowFlag(Qt::FramelessWindowHint);
	this->setLayout(hlay2);
	this->showMaximized();

	_get_pair();
}

void App::_get_depth()
{
	if (wdepth) delete wdepth;
	QString addr = "https://api.kraken.com/0/public/Ticker?pair="
	+ cview1->ps.name + ","	+ cview2->ps.name;
//	QString addr = "https://api.kraken.com/0/public/Depth?pair="\
	+ combo->itemText(combo->currentIndex()) + "&count=500";
	QUrl url(addr);
	QNetworkRequest req(url);

	wdepth = new Web();
	wdepth->get(req);
	connect(wdepth, SIGNAL(read_ok(QString)), this, SLOT(_read_depth(QString)));
}

void App::_get_pair()
{
	if (wohlc1) delete wohlc1;
	if (wohlc2) delete wohlc2;
	QString pair1 = combo1->itemText(combo1->currentIndex());
	QString pair2 = combo2->itemText(combo2->currentIndex());
	tick->setText(QDateTime::currentDateTime().toString());
	cview1->ctype = pair1;
	cview2->ctype = pair2;
ctext->clear();

	cview1->ps.name = pair1;
	QString addr1 = "https://api.kraken.com/0/public/OHLC?pair=" + pair1 + "&interval=60&since="
		+ QString::number(QDateTime::currentSecsSinceEpoch() - 60 * 60 * 300);
	QUrl url1(addr1);
	QNetworkRequest req1(url1);
	wohlc1 = new Web();
	wohlc1->get(req1);
	connect(wohlc1, SIGNAL(read_ok(QString)), cview1, SLOT(_read_ohlc(QString)));

	cview2->ps.name = pair2;
	QString addr2 = "https://api.kraken.com/0/public/OHLC?pair=" + pair2 + "&interval=60&since="
		+ QString::number(QDateTime::currentSecsSinceEpoch() - 60 * 60 * 300);
	QUrl url2(addr2);
	QNetworkRequest req2(url2);
	wohlc2 = new Web();
	wohlc2->get(req2);
	connect(wohlc2, SIGNAL(read_ok(QString)), cview2, SLOT(_read_ohlc(QString)));
}

void App::_quit()
{
	_save();
	LOGF("quit", -1);
	cview1->resetPS();
	cview2->resetPS();

	QApplication::quit();
}

void App::_read_depth(QString str)
{
	QJsonArray	jarr;
	QJsonDocument	jdoc;
	QJsonObject	jobj, jp1, jp2;
	QJsonValue	jval;

	jdoc = QJsonDocument::fromJson((str).toUtf8());
	if (jdoc.isNull()) goto ERRC;
	jobj = jdoc.object();

	if ((jval = jobj["result"]) == QJsonValue::Undefined) goto ERRC;
	jobj = jval.toObject();

	if ((jval = jobj[cview1->ps.name]) == QJsonValue::Undefined) goto ERRC;
	jp1 = jval.toObject();
	if ((jval = jobj[cview2->ps.name]) == QJsonValue::Undefined) goto ERRC;
	jp2 = jval.toObject();

	if ((jval = jp1["c"]) == QJsonValue::Undefined) goto ERRC;
	jarr = jval.toArray();
	cview1->ps.ph[cview1->ps.len-1].price = jarr[0].toString().toDouble() * (cview1->ps.mult);

	if ((jval = jp2["c"]) == QJsonValue::Undefined) goto ERRC;
	jarr = jval.toArray();
	cview2->ps.ph[cview2->ps.len-1].price = jarr[0].toString().toDouble() * (cview2->ps.mult);
ctext->append(QString::number(cview1->ps.ph[cview1->ps.len-1].price)
	+"\t"+ QString::number(cview2->ps.ph[cview2->ps.len-1].price));

	cview1->do_last();
	cview2->do_last();

ERRC:	return;
}

void App::_save(bool clearMark)
{
/*
	const int	blen = 300;
	char		buf[blen];
	int32_t		i;
	PH		*tmp;
	QFile		fout(fpath->text());
	QTextStream	sout;

	ctext->append(QString("Saving to:")+fpath->text());
	if (!fout.open(QIODevice::WriteOnly)) goto ERRC;
	sout.setDevice(&fout);

	sout << "test save";

	sout.flush();
	fout.close();
	return;
ERRC:	ctext->append("Save error");
*/
}

void App::_time()
{
	char buf[BUF_80];
	qint64 utime = QDateTime::currentSecsSinceEpoch() % (15 * 60);
	syst->setText(QDateTime::currentDateTime().toString());
	if (utime == 0) _get_pair();
	else if (utime % 5 == 0) _get_depth();

	utime %= 60;
	if ((utime + 2) % 5 == 0) {
		snprintf(buf, BUF_80, "%c%5d  %2d\n%5d  %2d\r", (char) utime,
			(int) cview1->ps.last, (int) cview1->ps.rsi,
			(int) cview2->ps.last, (int) cview2->ps.rsi);
	} else	snprintf(buf, BUF_80, "%c", (char) utime);

	SEND(buf);
//	LOGF(buf);
}

void App::resizeEvent(QResizeEvent *evt) {
	cview1->graph();
	cview1->ov->resize(cview1->size());
	cview2->graph();
	cview2->ov->resize(cview2->size());
	QWidget::resizeEvent(evt);
}

