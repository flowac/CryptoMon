#include "lib.h"

void Overlay::config(bool forceLoad)
{
	int i;
	QString buf = "", cpath = "." + ps->name + ".cfg";
	QStringList blist;
	QTextStream sio;
	QFile cfg(cpath);

	if (forceLoad || !nlock) goto LOAD;
	if (!cfg.open(QIODevice::WriteOnly)) goto RETURN;
	sio.setDevice(&cfg);
	for (i = 0; i < std::min(maxlock, nlock); i++) buf += QString::number(ylock[i]) + ",";
	sio << buf;
	sio.flush();
	cfg.close();
	goto RETURN;

LOAD:	if (!cfg.open(QIODevice::ReadOnly | QIODevice::Text)) goto RETURN;
	sio.setDevice(&cfg);
	buf = sio.readLine();
	blist = buf.split(",");
	for (i = nlock = 0; i < blist.size() && nlock < maxlock; i++) {
		ylock[nlock] = blist[i].toInt();
		if (ylock[nlock] != 0) nlock++;
	}
	cfg.close();
RETURN:	update();
}

void Overlay::enterEvent(QEvent *e) {(void)e; view_active = 1; this->update(); setFocus();}
void Overlay::leaveEvent(QEvent *e) {(void)e; view_active = 0; this->update(); clearFocus();}

void Overlay::paintEvent(QPaintEvent *)
{
	double h0 = height();
	int i, mx = mpos.x(), my = mpos.y(), ly, xp;
	char buf[BUF_200];
	double h1 = h0 * ps->div1, h1b = h1 + FONT_SZ - 2;
	double w0 = width(), w1 = (mx > w0 * 0.7) ? mx - FONT_SZ * 10 : mx + 3;
	double percent, price, target;
	QFont fnt;
	QPainter pnt;

	if (!ps->len) return;
	pnt.begin(this);
	pnt.setPen(pwhite);
	fnt = pnt.font();
	fnt.setPixelSize(FONT_SZ);
	pnt.setFont(fnt);

	if (!view_active) goto HIDE;
	xp = (int)(( ((double)mx) / w0) * (ps->len - XOFFSET)) + XOFFSET;
	price = ps->ph[xp].price;
	pnt.drawLine(1, my, w0 - 1, my);
	pnt.drawLine(mx, 0, mx, height());

	snprintf(buf, BUF_200, "%s", QDateTime::fromSecsSinceEpoch(ps->ph[xp].utime).toString().toUtf8().constData());
	pnt.drawText(w1, FONT_SZ, buf);
	snprintf(buf, BUF_200, "%.0f  %.0f-%.0f", price, ps->ph[xp].plo, ps->ph[xp].phi);
	pnt.drawText(w1, FONT_SZ * 2, buf);

	pnt.setPen(pyellow);
	snprintf(buf, BUF_200, "%.2f, %.2f", ps->ph[xp].rsi, ps->ph[xp].r30);
	pnt.drawText(w1, h1b, buf);
	pnt.setPen(pyellow);
	snprintf(buf, BUF_200, "%.2f, %.2f", ps->ph[xp].r50, ps->ph[xp].r70);
	pnt.drawText(w1, h1b + FONT_SZ, buf);
	pnt.setPen(pred);
	snprintf(buf, BUF_200, "%.1f", ps->ph[xp].volume);
	pnt.drawText(w1, h1b + FONT_SZ * 2, buf);
	pnt.setPen(pwhite);

	target = ps->pmin + (h1 - my) / h1 * (ps->pmax - ps->pmin);
	percent = (target - price) * 100 / price;
	snprintf(buf, BUF_200, "%.0f  %.2f%%", target, percent);
	pnt.drawText(w1, std::max(FONT_SZ * 3, std::min(my, (int)h1 - 2)), buf);

HIDE:	for (i = 0; i < std::min(nlock, maxlock); i++) {
		ly = h1 - (ylock[i] - ps->pmin) * h1 / (ps->pmax - ps->pmin);
		pnt.drawLine(1, ly, w0 - 1, ly);
		snprintf(buf, BUF_200, "%d", ylock[i]);
		pnt.drawText(1, ly + FONT_SZ, buf);
	}
	pnt.end();
}

void Overlay::keyPressEvent(QKeyEvent *evt)
{
	switch (evt->key()) {
	case Qt::Key_Shift:
		precision = 1000.0;
		break;
	}
}

void Overlay::keyReleaseEvent(QKeyEvent *evt)
{
	switch (evt->key()) {
	case Qt::Key_Shift:
		precision = 100.0;
		break;
	}
}

void Overlay::mouseMoveEvent(QMouseEvent *evt)
{
	char mxy[BUF_80];
	mpos = evt->pos();
	gpos = evt->globalPos();

	snprintf(mxy, BUF_80, "%d,%d\t\t%d,%d\n", mpos.x(), mpos.y(), gpos.x(), gpos.y());
	LOGF(mxy);
	this->update();
}

void Overlay::mousePressEvent(QMouseEvent *evt)
{
	double h1 = height() * ps->div1;
	int lpos = nlock % maxlock;
	int my = mpos.y();

	switch (evt->button()) {
	case Qt::RightButton:
		ylock[lpos] = ps->pmin + (h1 - my) / h1 * (ps->pmax - ps->pmin);
		ylock[lpos] = round(ylock[lpos] / precision) * (int)precision;
		nlock++;
		break;
	case Qt::MiddleButton:
		nlock = 0;
		LOGF("middle click\n");
		break;
	default:break;
	}
	this->update();
}

