#include "lib.h"

App::App()
{
	QString base, css1, css2, css3, css4, fontsize, logsize;
	QBrush brush(QColor(0,0,0), Qt::SolidPattern);
	QPalette cback = QPalette(QPalette::Window, Qt::black);
	QSizePolicy szp1, szp2, szpol = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QLabel *site = new QLabel("1btc.win", this);
	QTimer *systimer = new QTimer(this);
	QPushButton *but1, *but2;
	QWidget *hbox1, *hbox2, *hbox3, *vbox1;
	QHBoxLayout *hlay1, *hlay2, *hlay3, *hlay4;
	QVBoxLayout *vlay1;

	szp1 = szp2 = szpol;
	szp1.setHorizontalStretch(1);
	szp2.setHorizontalStretch(2);
	base = "red";
	fontsize = QString::number(FONT_SZ);
	logsize = QString::number(LOG_SZ);
	css1 = "background: black; border:1px solid " + base + "; color: " + base + "; font-size: " + fontsize + "px;";//button
	css2 = "background: black; border:1px solid purple; color: " + base + "; font-size: " + fontsize + "px;";//line edit
	css3 = "background: black; border:0px; color: " + base + "; font-size: " + fontsize + "px;";//label
	css4 = "background: black; border:0px; color: " + base + "; font-family: monospace; font-size: " + logsize + "px;";//text area

	hlay2	= new QHBoxLayout(this);
	vbox1	= new QWidget(this);
	vlay1	= new QVBoxLayout(vbox1);
	hbox1	= new QWidget(vbox1);
	hbox2	= new QWidget(vbox1);
	hbox3	= new QWidget(vbox1);
	cview1	= new Chart(hbox2, &ps1);
	cview2	= new Chart(hbox2, &ps2);
	cview3	= new Chart(hbox3, &ps3);
	cview4	= new Chart(hbox3, &ps4);
	hlay1	= new QHBoxLayout(hbox1);
	hlay3	= new QHBoxLayout(hbox2);
	hlay4	= new QHBoxLayout(hbox3);
	combo1	= new QComboBox(hbox1);
	combo2	= new QComboBox(hbox1);
	syst	= new QLabel(QDateTime::currentDateTime().toString(), hbox1);
	tick	= new QLabel(hbox1);
	but1	= new QPushButton("Exit", hbox1);
	but2	= new QPushButton("Search", hbox1);


	hbox1->setFixedHeight(TAB_H);
	hlay1->setContentsMargins(SPACE, 0, SPACE, 0);
	hlay2->setContentsMargins(0, 0, 0, 0);
	hlay3->setContentsMargins(0, 0, 0, 0);
	hlay4->setContentsMargins(0, 0, 0, 0);
	vlay1->setContentsMargins(0, 0, 0, 0);
	hlay1->setSpacing(SPACE);
	hlay2->setSpacing(0);
	hlay3->setSpacing(0);
	hlay4->setSpacing(0);
	vlay1->setSpacing(0);

	but1->setFixedWidth(TAB_H*4);
	but2->setFixedWidth(TAB_H*4);
	combo1->setFixedWidth(TAB_H*6);
	combo2->setFixedWidth(TAB_H*6);
	syst->setFixedWidth(TAB_H*11);
	syst->setAlignment(Qt::AlignCenter);
	tick->setFixedWidth(TAB_H*11);
	tick->setAlignment(Qt::AlignCenter);
	site->setFixedWidth(TAB_H*3);
	site->setAlignment(Qt::AlignCenter);

	but1->setSizePolicy(szpol);
	but2->setSizePolicy(szpol);
	cview1->setSizePolicy(szp1);
	cview2->setSizePolicy(szp2);
	cview3->setSizePolicy(szp1);
	cview4->setSizePolicy(szp2);
	combo1->setSizePolicy(szpol);
	combo2->setSizePolicy(szpol);
	but1->setStyleSheet(css1);
	but2->setStyleSheet(css1);
	cview1->setStyleSheet(css3);
	cview2->setStyleSheet(css3);
	cview3->setStyleSheet(css3);
	cview4->setStyleSheet(css3);
	combo1->setStyleSheet(css1);
	combo2->setStyleSheet(css1);
	syst->setStyleSheet(css3);
	tick->setStyleSheet(css3);
	site->setStyleSheet(css3);

	hlay1->addWidget(but1);
	hlay1->addWidget(combo1);
	hlay1->addWidget(combo2);
	hlay1->addWidget(but2);
	hlay1->addWidget(site);
	hlay1->addWidget(tick);
	hlay1->addWidget(syst);
	hbox1->setLayout(hlay1);

	vlay1->addWidget(hbox1);
	cview1->ctype = cview2->ctype = cview3->ctype = cview4->ctype = "None";
	hlay3->addWidget(cview2);
	hlay3->addWidget(cview1);
	hlay4->addWidget(cview4);
	hlay4->addWidget(cview3);
	hbox2->setLayout(hlay3);
	hbox3->setLayout(hlay4);
	vlay1->addWidget(hbox2);
	vlay1->addWidget(hbox3);
	vbox1->setLayout(vlay1);

	hlay2->addWidget(vbox1);

	combo1->addItem("XXBTZUSD");
	combo1->addItem("XETHXXBT");
	combo1->addItem("XETHZUSD");
	combo1->addItem("");
	combo1->setEditable(true);
	combo1->setInsertPolicy(QComboBox::InsertAtTop);

	combo2->addItem("XETHXXBT");
	combo2->addItem("XETHZUSD");
	combo2->addItem("");
	combo2->setEditable(true);
	combo2->setInsertPolicy(QComboBox::InsertAtTop);

	connect(but1, SIGNAL(released()), this, SLOT(_quit()));
	connect(but2, SIGNAL(released()), this, SLOT(_get_pair()));
	connect(systimer, SIGNAL(timeout()), this, SLOT(_time()));
	systimer->start(1000);

	this->setPalette(cback);
	this->setWindowFlag(Qt::FramelessWindowHint);
	this->setLayout(hlay2);
	this->showMaximized();

	_get_pair();
}

void App::_get_tick()
{
	if (wdepth) delete wdepth;
	QString addr = "https://api.kraken.com/0/public/Ticker?pair="
		+ cview1->name + ","	+ cview3->name;
	QUrl url(addr);
	QNetworkRequest req(url);

	wdepth = new Web();
	wdepth->get(req);
	connect(wdepth, SIGNAL(read_ok(QString)), this, SLOT(_read_tick(QString)));
}

void App::_get_pair_helper(QString pair, int interval, int length, Chart *cview, Web *wohlc)
{
	if (wohlc) delete wohlc;
	//Time offset is (length + offset) * 60 * interval
	//Maximum of 720 entries without authentication
	QString addr = "https://api.kraken.com/0/public/OHLC?pair=" + pair + "&interval="
		+ QString::number(interval) + "&since="	+ QString::number(
		QDateTime::currentSecsSinceEpoch() - (length + XOFFSET) * 60 * interval);
	QUrl url(addr);
	QNetworkRequest req(url);
	wohlc = new Web();
	wohlc->get(req);
	connect(wohlc, SIGNAL(read_ok(QString)), cview, SLOT(_read_ohlc(QString)));
}

void App::_get_pair()
{
	QString pair1 = combo1->itemText(combo1->currentIndex());
	QString pair2 = combo2->itemText(combo2->currentIndex());
	tick->setText(QDateTime::currentDateTime().toString());
	cview1->ctype = pair1 + " 15 Minutes";
	cview2->ctype = pair1 + " 1 Hour";
	cview3->ctype = pair2 + " 15 Minutes";
	cview4->ctype = pair2 + " 1 Hour";

	cview1->name = cview2->name = pair1;
	cview3->name = cview4->name = pair2;
	_get_pair_helper(pair1, 15, P1_LEN, cview1, wohlc1);
	_get_pair_helper(pair1, 60, P2_LEN, cview2, wohlc2);
	_get_pair_helper(pair2, 15, P1_LEN, cview3, wohlc3);
	_get_pair_helper(pair2, 60, P2_LEN, cview4, wohlc4);
}

void App::_quit()
{
	LOGF("quit", -1);

	cview1->clear();
	cview2->clear();
	cview3->clear();
	cview4->clear();

	cview1->resetPS();
	cview2->resetPS();
	cview3->resetPS();
	cview4->resetPS();

	if (wdepth) delete wdepth;
	if (wohlc1) delete wohlc1;
	if (wohlc2) delete wohlc2;
	if (wohlc3) delete wohlc3;
	if (wohlc4) delete wohlc4;

	QApplication::quit();
}

void App::_time()
{
	qint64 utime = (QDateTime::currentSecsSinceEpoch() - 1) % (15 * 30);
	syst->setText(QDateTime::currentDateTime().toString());
	if (utime == 0) _get_pair();
	else if (utime % 6 == 0) _get_tick();
}

void App::resizeEvent(QResizeEvent *evt) {
	cview1->graph();
	cview1->ov->resize(cview1->size());
	cview2->graph();
	cview2->ov->resize(cview2->size());
	cview3->graph();
	cview3->ov->resize(cview3->size());
	cview4->graph();
	cview4->ov->resize(cview4->size());
	QWidget::resizeEvent(evt);
}

