#ifndef _MAIN_LIB_H_
#define _MAIN_LIB_H_

#include <cmath>
//#include <cstdio>
//#include <cstdint>

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
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QTimer>

#define BUF_80	80
#define BUF_200	200
#define FONT_SZ	24
#define LOG_SZ	18
#define TAB_H	(FONT_SZ + 6)
#define SPACE	5
#define XOFFSET	50
#define TD_LEN	16

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
	QString name;
} PS;

void LOGF(const char *str, int level = 0);
void SEND(const char *str);

class Web : public QObject
{
	Q_OBJECT
public:
	void get(QNetworkRequest req);
	void post(QNetworkRequest req, QByteArray data);
signals:
	void read_ok(QString str);
private slots:
	void _delete_reply(QNetworkReply *ret) {delete ret; delete mgr;};
	void _read(QNetworkReply *ret);
private:
	QNetworkAccessManager *mgr;
};

class Overlay : public QWidget
{
	Q_OBJECT
public:
	Overlay(PS *ps, QWidget *p):QWidget(p) {this->ps=ps; setFocusPolicy(Qt::StrongFocus);};
	void config(bool forceLoad = false);
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);
private:
	static const int maxlock = 5;
	bool view_active = 1;
	int ylock[maxlock] = {0}, nlock = 0;
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
	PS ps;
	QString ctype = "";
	Overlay *ov;
	Chart(QWidget *p);
	void clear();
	void do_calc();
	void do_last();
	void graph();
	void resetPS();
private slots:
	void _read_ohlc(QString str);
private:
	int32_t n_items = 0;
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
	void _fpath(QString str) {fpath->setText(str + ".csv");};
	void _get_depth();
	void _get_pair();
	void _quit();
	void _read_depth(QString str);
	void _save(bool clearMark = false);
	void _time();
private:
	Chart *cview1, *cview2;
	QComboBox *combo1, *combo2;
	QLabel *syst, *tick;
	QLineEdit *fpath;
	QTextEdit *ctext;
	Web *wdepth = 0, *wohlc1 = 0, *wohlc2 = 0;
};

#endif

