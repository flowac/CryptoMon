#ifndef _MAIN_LIB_H_
#define _MAIN_LIB_H_

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdint>

#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QSizePolicy>
#include <QTabBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QIODevice>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageLogger>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QTextStream>
#include <QTimer>

#define BUF_80	80
#define BUF_200	200
#define FONT_SZ	24
#define LOG_SZ	18
#define TAB_H	(FONT_SZ + 6)
#define SPACE	5
#define XOFFSET	40
#define TD_LEN	16
#define MAXLOCK	5
#define P1_LEN	60	//Elements to show for short period
#define P2_LEN	120	//Elements to show for long period

typedef struct PRICE_HISTORY
{
	double phi, plo, poc, price, volume, ema, rsi, r30, r50, r70, rm20, rp20;
	unsigned char td[TD_LEN];
	int64_t utime;
} PH;

typedef struct PRICE_SUMMARY
{
	PH *ph = 0;
	double gain, loss, mult = 0, pmax, pmin, vmax, vmin;
	double last = 0, rsi = 0;
	double div1 = 0.8, div2 = 0.2;
	int32_t len = 0;
} PS;

void LOGF(const char *str, int level = 0);

class Web : public QObject
{
	Q_OBJECT
public:
	~Web() {if (mgr) delete mgr;};
	void get(QNetworkRequest req);
	void post(QNetworkRequest req, QByteArray data);
signals:
	void read_ok(QString str);
private slots:
	void _read(QNetworkReply *ret);
private:
	QNetworkAccessManager *mgr = 0;
};

class Overlay : public QWidget
{
	Q_OBJECT
public:
	Overlay(PS *ps, QWidget *p):QWidget(p) {this->ps=ps;setFocusPolicy(Qt::StrongFocus);};
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);
private:
	bool view_active = false;
	int ylock[MAXLOCK] = {0}, nlock = 0;
	double precision = 100.0;
	PS *ps;
	QPoint mpos, gpos;
	QPen pred = QPen(Qt::red, 1), pgreen = QPen(Qt::green, 1);
	QPen pwhite = QPen(Qt::white, 1), pyellow = QPen(Qt::yellow, 1);
	void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *evt);
	void keyReleaseEvent(QKeyEvent *evt);
	void mouseMoveEvent(QMouseEvent *evt);
	void mousePressEvent(QMouseEvent *evt);
};

class Chart : public QGraphicsView
{
	Q_OBJECT
public:
	PS *ps;
	QString ctype, name;
	Overlay *ov;
	Chart(QWidget *p, PS *ps);
	void clear();
	void do_calc();
	void do_last();
	void graph();
	void resetPS();
private slots:
	void _read_ohlc(QString str);
private:
	int n_items = 0;
	QBrush bred, bgreen, byellow, bblack, bwhite;
	QGraphicsScene scn;
	QPen pred, pgreen, pyellow, pwhite, pgray, pgray2, ppink, pblack;
	QGraphicsTextItem *g_items[10];
};

class App : public QWidget
{
	Q_OBJECT
public:
	App();
	void resizeEvent(QResizeEvent *evt);
private slots:
	void _get_tick();
	void _get_pair();
	void _quit();
	void _read_tick(QString str);
	void _time();
private:
	Chart *cview1, *cview2, *cview3, *cview4;
	//TODO: move PS back into Chart class
	PS ps1, ps2, ps3, ps4;//Note: Charts get messed up if malloc'ed in Chart class
	QComboBox *combo1, *combo2;
	QLabel *syst, *tick;
	Web *wdepth = 0, *wohlc1 = 0, *wohlc2 = 0, *wohlc3 = 0, *wohlc4 = 0;
	void _get_pair_helper(QString pair, int interval, int length, Chart *cview, Web *wohlc);
};
#endif

