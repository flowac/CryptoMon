#include "lib.h"

void Web::get(QNetworkRequest req)
{
//Note: make the manager static or delete it once reply is read.
	QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
	mgr->get(req);
	connect(mgr, SIGNAL(finished(QNetworkReply *)), this, SLOT(_read(QNetworkReply *)));
}

void Web::post(QNetworkRequest req, QByteArray data)
{
	QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
	mgr->post(req, data);
	connect(mgr, SIGNAL(finished(QNetworkReply *)), this, SLOT(_read(QNetworkReply *)));
}

void Web::_read(QNetworkReply *ret)
{
	QString out(ret->readAll());
	ret->deleteLater();
	emit read_ok(out);
}

