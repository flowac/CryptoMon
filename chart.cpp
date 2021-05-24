#include "lib.h"

Chart::Chart(QWidget *p, PS *ps):QGraphicsView(p)
{
	this->ps = ps;
	ov = new Overlay(ps, this);
	bred	= QBrush(QColor(255,0,0), Qt::SolidPattern);
	bgreen	= QBrush(QColor(0,255,0), Qt::SolidPattern);
	byellow	= QBrush(QColor(255,255,0), Qt::SolidPattern);
	bblack	= QBrush(QColor(0,0,0), Qt::SolidPattern);
	bwhite	= QBrush(QColor(255,255,255), Qt::SolidPattern);
	pred	= QPen(QColor(255,0,0), 1);
	pgreen	= QPen(QColor(0,255,0), 1);
	pyellow	= QPen(QColor(255,255,0), 1);
	pwhite	= QPen(QColor(255,255,255), 1);
	pgray	= QPen(QColor(155,155,155), 1);
	pgray2	= QPen(QColor(100,100,100), 1);
	ppink	= QPen(QColor(255,105,180), 1);
	pblack	= QPen(QColor(0,0,0), 1);

	setScene(&scn);
	scn.setBackgroundBrush(Qt::black);
	setContentsMargins(0, 0, 0, 0);
	setInteractive(false);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void Chart::clear()
{
	while (n_items > 0) {
		n_items--;
		if (g_items[n_items]) delete g_items[n_items];
		g_items[n_items] = 0;
	}
	scn.clear();
	resetCachedContent();
}

void Chart::graph()
{
	clear();
	if (ps->len == 0) return;
	double w0 = width();
	double h0 = height(), h1 = h0 * ps->div1, h2 = h0 * ps->div2;
	double xp = w0 / (ps->len - XOFFSET);
	double ypp = h1 / (ps->pmax - ps->pmin), yp2 = h2 / 100;
	double ypv = h2 / (ps->vmax - ps->vmin);
	double i0;
	int32_t i, path;
	QGraphicsTextItem *tmp;

	for (i = 20; i < 90; i+=20) {
		scn.addLine(4, h0 - yp2 * i, w0, h0 - yp2 * i, pgray);

		//Volume
		tmp = new QGraphicsTextItem();
		g_items[n_items++] = tmp;
		tmp->setDefaultTextColor(Qt::gray);
		tmp->setPos(w0 - 18, h0 - yp2 * i);
		tmp->setPlainText(QString::number(i));
		scn.addItem(tmp);
	}

	tmp = new QGraphicsTextItem();
	g_items[n_items++] = tmp;
	tmp->setDefaultTextColor(Qt::white);
	tmp->setPos(w0 - FONT_SZ, h0 - yp2 * i);
	tmp->setPlainText("RSI");
	scn.addItem(tmp);

	tmp = new QGraphicsTextItem();
	g_items[n_items++] = tmp;
	tmp->setDefaultTextColor(Qt::white);
	tmp->setPos(2, h0 - yp2 * i);
	tmp->setPlainText("Volume");
	scn.addItem(tmp);

	tmp = new QGraphicsTextItem();
	g_items[n_items++] = tmp;
	tmp->setDefaultTextColor(Qt::white);
	tmp->setPos(2, 2);
	tmp->setPlainText(ctype);
	scn.addItem(tmp);

	scn.addLine(0, h1, w0, h1, ppink);
	scn.addLine(w0 - 1, 1, w0 - 1, 3, pblack);//vertical anchor

	for (i = 1 + XOFFSET, i0 = xp; i < ps->len; i++, i0+=xp) {
		//Open close price, volume weighted price
		scn.addLine(i0, std::max(1, (int) (h1 - (ps->ph[i].phi - ps->pmin) * ypp)),
				i0, std::min((int)h0, (int) (h1 - (ps->ph[i].plo - ps->pmin) * ypp)), ppink);
		scn.addLine(i0 - xp, h1 - (ps->ph[i-1].poc - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].poc - ps->pmin) * ypp, pred);
		scn.addLine(i0 - 1, h1 - (ps->ph[i].price - ps->pmin) * ypp,
				i0 + 3, h1 - (ps->ph[i].price - ps->pmin) * ypp, pyellow);

		scn.addLine(i0 - xp, h1 - (ps->ph[i-1].r50 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].r50 - ps->pmin) * ypp, pgray);

		path = 0;
		if (ps->ph[i].r30 < ps->ph[i].rm20)	path+=1;
		if (ps->ph[i-1].r30 < ps->ph[i-1].rm20)	path+=2;
		switch (path) {
		case 1: scn.addLine(i0 - xp, h1 - (ps->ph[i-1].rm20 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].r30 - ps->pmin) * ypp, pgray); break;
		case 2: scn.addLine(i0 - xp, h1 - (ps->ph[i-1].r30 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].rm20 - ps->pmin) * ypp, pgray); break;
		case 3: scn.addLine(i0 - xp, h1 - (ps->ph[i-1].r30 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].r30 - ps->pmin) * ypp, pgray); break;
		default:break;
		}

		path = 0;
		if (ps->ph[i].r70 > ps->ph[i].rp20)	path+=1;
		if (ps->ph[i-1].r70 > ps->ph[i-1].rp20)	path+=2;
		switch (path) {
		case 1: scn.addLine(i0 - xp, h1 - (ps->ph[i-1].rp20 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].r70 - ps->pmin) * ypp, pgray); break;
		case 2: scn.addLine(i0 - xp, h1 - (ps->ph[i-1].r70 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].rp20 - ps->pmin) * ypp, pgray); break;
		case 3: scn.addLine(i0 - xp, h1 - (ps->ph[i-1].r70 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].r70 - ps->pmin) * ypp, pgray); break;
		default:break;
		}

		scn.addLine(i0 - xp, h1 - (ps->ph[i-1].rm20 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].rm20 - ps->pmin) * ypp, pyellow);
		scn.addLine(i0 - xp, h1 - (ps->ph[i-1].rp20 - ps->pmin) * ypp,
				i0, h1 - (ps->ph[i].rp20 - ps->pmin) * ypp, pyellow);

		//RSI
		scn.addLine(i0 - xp, h0 - ps->ph[i-1].rsi * yp2, i0, h0 - ps->ph[i].rsi * yp2, pyellow);
		scn.addLine(i0 - xp, h0 - (ps->ph[i].volume - ps->vmin) * ypv, i0 - xp, h0,
				ps->ph[i].price < ps->ph[i-1].price ? pred : pgreen);
	}
}

