#include "lib.h"

void Web::get(QNetworkRequest req)
{
	mgr = new QNetworkAccessManager(this);
	mgr->get(req);
	connect(mgr, SIGNAL(finished(QNetworkReply *)), this, SLOT(_read(QNetworkReply *)));
}

void Web::post(QNetworkRequest req, QByteArray data)
{
	mgr = new QNetworkAccessManager(this);
	mgr->post(req, data);
	connect(mgr, SIGNAL(finished(QNetworkReply *)), this, SLOT(_read(QNetworkReply *)));
}

void Web::_read(QNetworkReply *ret)
{
	QString str(ret->readAll());
	ret->close();
	emit read_ok(str);
}

