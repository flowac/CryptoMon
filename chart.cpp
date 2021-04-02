#include "lib.h"

Chart::Chart(QWidget *p):QGraphicsView(p)
{
	ov = new Overlay(&ps, this);
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

void Chart::graph()
{
	scn.clear();
	if (ps.len == 0) return;
	double w0 = width();
	double h0 = height(), h1 = h0 * ps.div1, h2 = h0 * ps.div2;
	double xp = w0 / (ps.len - XOFFSET);
	double ypp = h1 / (ps.pmax - ps.pmin), yp2 = h2 / 100;
	double ypv = h2 / (ps.vmax - ps.vmin);
	double i0;
	double tmp_y, tmp_y0, tmp_p, tmp_p0;
	int32_t i;
	QFont fnt;
	QGraphicsTextItem *tmp;

	for (i = 20; i < 90; i+=20) {
		scn.addLine(4, h0 - yp2 * i, w0, h0 - yp2 * i, pgray);

		//Volume
		tmp = new QGraphicsTextItem();
		tmp->setDefaultTextColor(Qt::gray);
		tmp->setPos(w0 - 18, h0 - yp2 * i);
		tmp->setPlainText(QString::number(i));
		scn.addItem(tmp);
	}

	tmp = new QGraphicsTextItem();
	tmp->setDefaultTextColor(Qt::white);
	tmp->setPos(w0 - FONT_SZ, h0 - yp2 * i);
	tmp->setPlainText("RSI");
	scn.addItem(tmp);

	tmp = new QGraphicsTextItem();
	tmp->setDefaultTextColor(Qt::white);
	tmp->setPos(2, h0 - yp2 * i);
	tmp->setPlainText("Volume");
	scn.addItem(tmp);

	tmp = new QGraphicsTextItem();
	tmp->setDefaultTextColor(Qt::white);
	tmp->setPos(2, 2);
	tmp->setPlainText(ctype);
	scn.addItem(tmp);

	scn.addLine(0, h1, w0, h1, ppink);
	scn.addLine(w0 - 1, 1, w0 - 1, 3, pblack);//vertical anchor

	for (i = 1 + XOFFSET, i0 = xp; i < ps.len; i++, i0+=xp) {
		//Open close price, volume weighted price
		scn.addLine(i0, std::max(1, (int) (h1 - (ps.ph[i].phi - ps.pmin) * ypp)),
				i0, std::min((int)h0, (int) (h1 - (ps.ph[i].plo - ps.pmin) * ypp)), ppink);
		scn.addLine(i0 - xp, h1 - (ps.ph[i-1].poc - ps.pmin) * ypp,
				i0, h1 - (ps.ph[i].poc - ps.pmin) * ypp, pred);
		scn.addLine(i0 - 1, h1 - (ps.ph[i].price - ps.pmin) * ypp,
				i0 + 3, h1 - (ps.ph[i].price - ps.pmin) * ypp, pyellow);
//		scn.addLine(i0 - xp, h1 - (ps.ph[i-1].price * 0.95 - ps.pmin) * ypp,
//				i0, h1 - (ps.ph[i].price * 0.95 - ps.pmin) * ypp, pgray2);
		scn.addLine(i0 - xp, h1 - (ps.ph[i-1].r30 - ps.pmin) * ypp,
				i0, h1 - (ps.ph[i].r30 - ps.pmin) * ypp, pgray);
		scn.addLine(i0 - xp, h1 - (ps.ph[i-1].r50 - ps.pmin) * ypp,
				i0, h1 - (ps.ph[i].r50 - ps.pmin) * ypp, pgray);
		scn.addLine(i0 - xp, h1 - (ps.ph[i-1].r70 - ps.pmin) * ypp,
				i0, h1 - (ps.ph[i].r70 - ps.pmin) * ypp, pgray);
		scn.addLine(i0 - xp, h1 - (ps.ph[i-1].rm20 - ps.pmin) * ypp,
				i0, h1 - (ps.ph[i].rm20 - ps.pmin) * ypp, pyellow);
		scn.addLine(i0 - xp, h1 - (ps.ph[i-1].rp20 - ps.pmin) * ypp,
				i0, h1 - (ps.ph[i].rp20 - ps.pmin) * ypp, pyellow);

		//RSI
		scn.addLine(i0 - xp, h0 - ps.ph[i-1].rsi * yp2, i0, h0 - ps.ph[i].rsi * yp2, pyellow);
		scn.addLine(i0 - xp, h0 - (ps.ph[i].volume - ps.vmin) * ypv, i0 - xp, h0, pred);
	}
	i--;
	tmp_p	= ps.ph[i].price;
	tmp_p0	= ps.ph[i - 1].price;
	tmp_y	= h1 - (tmp_p - ps.pmin) * ypp;
	scn.addLine(2, tmp_y, w0 - 2, tmp_y, tmp_p < tmp_p0 ? pred : pgreen);

	tmp = new QGraphicsTextItem();
	fnt = tmp->font();
	fnt.setPixelSize(FONT_SZ);
	tmp->setFont(fnt);

	tmp->setDefaultTextColor(tmp_p < tmp_p0 ? Qt::red : Qt::green);
	tmp->setPos(w0 - FONT_SZ * 4, std::min(h0 - FONT_SZ, tmp_y + FONT_SZ * 4));
	tmp->setPlainText(QString::number((int) tmp_p));
	scn.addItem(tmp);
}

void Chart::_read_ohlc(QString str)
{
	int32_t		i;
	double		mult = 0;
	QJsonArray	jarr, jidx;
	QJsonDocument	jdoc;
	QJsonObject	jobj;
	QJsonValue	jval;

	resetPS();
	jdoc = QJsonDocument::fromJson((str).toUtf8());
	if (jdoc.isNull()) goto ERRC;
	jobj = jdoc.object();

	if ((jval = jobj["result"]) == QJsonValue::Undefined) goto ERRC;
	jobj = jval.toObject();
//	ctext->append(jobj.keys().join("-"));

	if ((jval = jobj[ps.name]) == QJsonValue::Undefined) goto ERRC;
	jarr = jval.toArray();
	ps.len = jarr.size();
//	ctext->append(QString("List length:")+QString::number(ps.len));
	if (ps.len < XOFFSET) goto ERRC;

	jidx = jarr[0].toArray();
	if (jidx.count() != 8) goto ERRC;
	ps.ph = (PH *)malloc(sizeof(PH) * ps.len);

	for (i = 0; i < ps.len; i++) {
		jidx = jarr[i].toArray();
		ps.ph[i].utime	= jidx[0].toInt();
		if (i == 0) ps.mult = mult = jidx[2].toString().toDouble() > 1 ? 1.0 : 1000000.0;
		ps.ph[i].phi	= jidx[2].toString().toDouble() * mult;
		ps.ph[i].plo	= jidx[3].toString().toDouble() * mult;
		ps.ph[i].price	= jidx[5].toString().toDouble() * mult;
		ps.ph[i].poc	= (jidx[4].toString().toDouble() + jidx[1].toString().toDouble()) * mult / 2.0;
		ps.ph[i].volume	= jidx[6].toString().toDouble();
		if (ps.ph[i].price == 0) ps.ph[i].price = ps.ph[i].poc;
	}
	do_calc();
	return;

ERRC:	resetPS();
}

static inline double r_gain(double target, double rsi1, double rsi2, double gain, double loss, double price)
{
	double tmp1 = 100.0 / (100 - target) - 1;
	double tmp2 = loss * rsi2 / rsi1 * tmp1;
	return tmp2 * rsi1 + price - gain * rsi2;
}

static inline double r_loss(double target, double rsi1, double rsi2, double gain, double loss, double price)
{
	double tmp1 = 100.0 / (100 - target) - 1;
	double tmp2 = gain * rsi2 / rsi1 / tmp1;
	return loss * rsi2 + price - tmp2 * rsi1;
}

void Chart::do_last()
{
	int32_t		i = ps.len - 1;
	const int	rsi_p = 14;
	double		prev, price, gain = ps.gain, loss = ps.loss;
	double		rsi1, rsi2, rm20, rp20;

	if (ps.len == 0) return;

	rsi1 = rsi_p;
	rsi2 = rsi1 - 1;

	prev = ps.ph[i - 1].price;
	ps.last = price = ps.ph[i].price;

	//Relative Strength Index
	if (price > prev) {
		gain = (gain * rsi2 + price - prev) / rsi1;
		loss = loss * rsi2 / rsi1;
	} else {
		gain = gain * rsi2 / rsi1;
		loss = (loss * rsi2 + prev - price) / rsi1;
	}
	ps.rsi = ps.ph[i].rsi = (loss > 0) ? 100 - 100.0 / (1 + gain / loss) : 100;
	rm20 = ps.rsi - 20;
	rp20 = ps.rsi + 20;
	ps.ph[i].r30 = r_loss(30.0, rsi1, rsi2, gain, loss, price);
	ps.ph[i].r50 = r_loss(50.0, rsi1, rsi2, gain, loss, price);
	ps.ph[i].r70 = r_gain(70.0, rsi1, rsi2, gain, loss, price);
//Note: the signs are wrong on the next two lines, but it gives desired results
	ps.ph[i].rm20 = (rm20 > 50) ? r_loss(rm20, rsi1, rsi2, gain, loss, price) : r_gain(rm20, rsi1, rsi2, gain, loss, price);
	ps.ph[i].rp20 = (rp20 < 50) ? r_gain(rp20, rsi1, rsi2, gain, loss, price) : r_loss(rp20, rsi1, rsi2, gain, loss, price);

	graph();
}

void Chart::do_calc()
{
	int32_t		i;
	const int	rsi_p = 14;
	double		prev, price, volume, pxvol;
	double		ema_v1a, ema_v1b, rsi1, rsi2, rm20, rp20, rtmp;
	double		num, den, gain, loss;

	rsi1 = rsi_p;
	rsi2 = rsi1 - 1;
	ema_v1b = ((int)(200000.0 / (1 + 10)))/100000.0;
	ema_v1a = 1.0 - ema_v1b;//EMA 10 V-EMA

	ps.ph[0].ema = price = prev = ps.ph[0].price;
	ps.ph[0].rsi = 50;
	den = ps.ph[0].volume;
	num = price * den;
	gain = loss = 0;

	for (i = 0; i < ps.len; i++) {
		price = ps.ph[i].price;
		volume = ps.ph[i].volume;

		//Volume Weighted EMA
		pxvol = ema_v1a * volume;
		num = ema_v1b * num + pxvol * price;
		den = ema_v1b * den + pxvol;
		ps.ph[i].ema = num / den;

		//Relative Strength Index
		if (price > prev) {
			gain = (gain * rsi2 + price - prev) / rsi1;
			loss = loss * rsi2 / rsi1;
		} else {
			gain = gain * rsi2 / rsi1;
			loss = (loss * rsi2 + prev - price) / rsi1;
		}
		ps.ph[i].rsi = rtmp = (loss > 0) ? 100 - 100.0 / (1 + gain / loss) : 100;

		rm20 = rtmp - 20;
		rp20 = rtmp + 20;
		ps.ph[i].r30 = r_loss(30.0, rsi1, rsi2, gain, loss, price);
		ps.ph[i].r50 = r_loss(50.0, rsi1, rsi2, gain, loss, price);
		ps.ph[i].r70 = r_gain(70.0, rsi1, rsi2, gain, loss, price);
//Note: the signs are wrong on the next two lines, but it gives desired results
		ps.ph[i].rm20 = (rm20 > 50) ? r_loss(rm20, rsi1, rsi2, gain, loss, price) : r_gain(rm20, rsi1, rsi2, gain, loss, price);
		ps.ph[i].rp20 = (rp20 < 50) ? r_gain(rp20, rsi1, rsi2, gain, loss, price) : r_loss(rp20, rsi1, rsi2, gain, loss, price);

		prev = price;
		if (i < XOFFSET) continue;
		else if (i == ps.len - 2) {
			ps.gain = gain;
			ps.loss = loss;
		}
		ps.pmax = std::max(ps.pmax, ps.ph[i].price * 1.02);
		ps.pmin = std::min(ps.pmin, ps.ph[i].price * 0.98);
		ps.vmax = std::max(ps.vmax, ps.ph[i].volume);
		ps.vmin = std::min(ps.vmin, ps.ph[i].volume);
	}

	ps.last	= ps.ph[i].price;
	ps.rsi	= ps.ph[i].rsi;

	ps.pmin = ceil(ps.pmin / 10) * 10;
	ps.pmax = ceil((ps.pmax + (ps.pmax - ps.pmin) * 0.04) / 10.0) * 10;

	graph();
}

void Chart::resetPS()
{
	if (ps.ph) free(ps.ph);
	ps.ph = 0;
	ps.len = 0;
	ps.pmax = ps.vmax = 0;
	ps.pmin = ps.vmin = 999999999;
}

