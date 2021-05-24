#include "lib.h"

void App::_read_tick(QString str)
{
	char		buf[BUF_80];
	QJsonArray	jarr;
	QJsonDocument	jdoc;
	QJsonObject	jobj, jp1, jp2;
	QJsonValue	jval;

	jdoc = QJsonDocument::fromJson(str.toUtf8());
	if (jdoc.isNull()) goto ERRC;
	jobj = jdoc.object();

	if ((jval = jobj["result"]) == QJsonValue::Undefined) goto ERRC;
	jobj = jval.toObject();

	if ((jval = jobj[cview1->name]) == QJsonValue::Undefined) goto ERRC;
	jp1 = jval.toObject();
	if ((jval = jobj[cview3->name]) == QJsonValue::Undefined) goto ERRC;
	jp2 = jval.toObject();

	if ((jval = jp1["c"]) == QJsonValue::Undefined) goto ERRC;
	jarr = jval.toArray();
	cview1->ps->last = cview2->ps->last = jarr[0].toString().toDouble() * (cview1->ps->mult);

	if ((jval = jp2["c"]) == QJsonValue::Undefined) goto ERRC;
	jarr = jval.toArray();
	cview3->ps->last = cview4->ps->last = jarr[0].toString().toDouble() * (cview3->ps->mult);

	str.clear();
	cview1->do_last();
	cview2->do_last();
	cview3->do_last();
	cview4->do_last();

	snprintf(buf, BUF_80, "%.0f %.1f %.0f %.1f",
		cview2->ps->last, cview2->ps->rsi,
		cview4->ps->last, cview4->ps->rsi);
	LOGF(buf, 2);

ERRC:	return;
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

	if ((jval = jobj[name]) == QJsonValue::Undefined) goto ERRC;
	jarr = jval.toArray();
	ps->len = jarr.size();
	if (ps->len < XOFFSET) goto ERRC;

	jidx = jarr[0].toArray();
	if (jidx.count() != 8) goto ERRC;
	ps->ph = (PH *)malloc(sizeof(PH) * ps->len);

	for (i = 0; i < ps->len; i++) {
		jidx = jarr[i].toArray();
		ps->ph[i].utime	= jidx[0].toInt();
		if (i == 0) ps->mult = mult = jidx[2].toString().toDouble() > 1 ? 1.0 : 1000000.0;
		ps->ph[i].phi	= jidx[2].toString().toDouble() * mult;
		ps->ph[i].plo	= jidx[3].toString().toDouble() * mult;
		ps->ph[i].price	= jidx[5].toString().toDouble() * mult;
		ps->ph[i].poc	= (jidx[4].toString().toDouble() + jidx[1].toString().toDouble()) * mult / 2.0;
		ps->ph[i].volume= jidx[6].toString().toDouble();
		if (ps->ph[i].price == 0) ps->ph[i].price = ps->ph[i].poc;
	}

	str.clear();
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
	int32_t		i = ps->len - 1;
	const double	rsi1 = 14, rsi2 = rsi1 - 1;
	double		prev, price, gain = ps->gain, loss = ps->loss;

	if (ps->len == 0) return;

	prev = ps->ph[i - 1].price;
	ps->ph[i].price = price = ps->last;

	//Relative Strength Index
	if (price > prev) {
		gain = (gain * rsi2 + price - prev) / rsi1;
		loss = loss * rsi2 / rsi1;
	} else {
		gain = gain * rsi2 / rsi1;
		loss = (loss * rsi2 + prev - price) / rsi1;
	}
	ps->rsi = ps->ph[i].rsi = (loss > 0) ? 100 - 100.0 / (1 + gain / loss) : 100;
	ps->ph[i].r30 = r_loss(30.0, rsi1, rsi2, gain, loss, price);
	ps->ph[i].r50 = r_loss(50.0, rsi1, rsi2, gain, loss, price);
	ps->ph[i].r70 = r_gain(70.0, rsi1, rsi2, gain, loss, price);

	ov->update();
}

void Chart::do_calc()
{
	char		buf[BUF_80];
	int32_t		i;
	const int	rsi_p = 14;
	double		prev, price, volume, pxvol;
	double		ema_v1a, ema_v1b, rsi1, rsi2, rm20, rp20, rtmp;
	double		num, den, gain, loss;

	rsi1 = rsi_p;
	rsi2 = rsi1 - 1;
	ema_v1b = ((int)(200000.0 / (1 + 10)))/100000.0;
	ema_v1a = 1.0 - ema_v1b;//EMA 10 V-EMA

	ps->ph[0].ema = price = prev = ps->ph[0].price;
	ps->ph[0].rsi = 50;
	den = ps->ph[0].volume;
	num = price * den;
	gain = loss = 0;

	for (i = 0; i < ps->len; i++) {
		price = ps->ph[i].price;
		volume = ps->ph[i].volume;

		//Volume Weighted EMA
		pxvol = ema_v1a * volume;
		num = ema_v1b * num + pxvol * price;
		den = ema_v1b * den + pxvol;
		ps->ph[i].ema = num / den;

		//Relative Strength Index
		if (price > prev) {
			gain = (gain * rsi2 + price - prev) / rsi1;
			loss = loss * rsi2 / rsi1;
		} else {
			gain = gain * rsi2 / rsi1;
			loss = (loss * rsi2 + prev - price) / rsi1;
		}
		ps->ph[i].rsi = rtmp = (loss > 0) ? 100 - 100.0 / (1 + gain / loss) : 100;

		//Use a graphing calculator to tweak these four formulas
		if (rtmp > 50) {
			rm20 = rtmp - 20;
			rp20 = rtmp * 0.3 + 55;
		} else {
			rm20 = rtmp * 0.3 + 15;
			rp20 = rtmp + 20;
		}

		ps->ph[i].r30 = r_loss(30.0, rsi1, rsi2, gain, loss, price);
		ps->ph[i].r50 = r_loss(50.0, rsi1, rsi2, gain, loss, price);
		ps->ph[i].r70 = r_gain(70.0, rsi1, rsi2, gain, loss, price);

		ps->ph[i].rm20 = (rm20 < 50) ? r_loss(rm20, rsi1, rsi2, gain, loss, price) : r_gain(rm20, rsi1, rsi2, gain, loss, price);
		ps->ph[i].rp20 = (rp20 > 50) ? r_gain(rp20, rsi1, rsi2, gain, loss, price) : r_loss(rp20, rsi1, rsi2, gain, loss, price);

		prev = price;
		if (i < XOFFSET) continue;
		else if (i == ps->len - 2) {
			ps->gain = gain;
			ps->loss = loss;
		}
		ps->pmax = std::max(ps->pmax, ps->ph[i].price * 1.02);
		ps->pmin = std::min(ps->pmin, ps->ph[i].price * 0.98);
		ps->vmax = std::max(ps->vmax, ps->ph[i].volume);
		ps->vmin = std::min(ps->vmin, ps->ph[i].volume);
	}

	ps->last = ps->ph[ps->len-1].price;
	ps->rsi = ps->ph[ps->len-1].rsi;

	ps->pmin = ceil(ps->pmin / 10) * 10;
	ps->pmax = ceil((ps->pmax + (ps->pmax - ps->pmin) * 0.04) / 10.0) * 10;

	snprintf(buf, BUF_80, "%s   Price: %f   RSI: %f", name.toStdString().c_str(), ps->last, ps->rsi);
	LOGF(buf);

	graph();
	ov->update();
}

void Chart::resetPS()
{
	if (ps->ph) free(ps->ph);
	ps->ph = 0;
	ps->len = 0;
	ps->pmax = ps->vmax = 0;
	ps->pmin = ps->vmin = 999999999;
}

