/*	Copyright  (c)	Günter Woigk 2013 - 2019
					mailto:kio@little-bat.de

	This file is free software.

	Permission to use, copy, modify, distribute, and sell this software
	and its documentation for any purpose is hereby granted without fee,
	provided that the above copyright notice appears in all copies and
	that both that copyright notice, this permission notice and the
	following disclaimer appear in supporting documentation.

	THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT ANY WARRANTY, NOT EVEN THE
	IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
	AND IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DAMAGES
	ARISING FROM THE USE OF THIS SOFTWARE,
	TO THE EXTENT PERMITTED BY APPLICABLE LAW.
*/

#include "kio/kio.h"
#include "Display404.h"
#include "Display404Font.h"
#include <QBitmap>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QPixmap>
#include <QTransform>
//#include "Sio.h"



const int zoom = 3;		// 2
const int hpadding = 6;
const int hspacing = 1;
const int width = (40*5 + 39*hspacing + 2*hpadding) * zoom;
const int vpadding = 6;
const int vspacing = 3;
const int height = (4*8 + 3*vspacing + 2*vpadding ) * zoom;

const QColor bgcolor(0x66,0x88,0x00,0xff);	// rgba
const QColor fgcolor(0x11,0x33,0x00,0xff);	// rgba



Display404::Display404(QWidget* parent)
:	QWidget(parent)
{
	setFixedWidth(::width);
	setFixedHeight(::height);

	//QWidget::setEnabled(true);                // enable mouse & kbd events (true=default)
	setFocusPolicy(Qt::StrongFocus);            // sonst kriegt das Toolwindow manchmal keine KeyEvents
//	setAttribute(Qt::WA_OpaquePaintEvent,on);   // wir malen alle Pixel. Qt muss nicht vorher löschen.

	x=y=0;

	canvas  = new QPixmap(width(),height()/*,QImage::Format_RGB32*/);
	painter = new QPainter(canvas);
	painter->setBackground(QBrush(bgcolor));

	painter->setPen(Qt::NoPen);
	painter->setBrush(fgcolor);

	QTransform trans;
	trans.scale(zoom,zoom);
	painter->setWorldTransform(trans);

	painter->eraseRect(canvas->rect());
	print("1234567890123456789012345678901234567890");
	print("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn");
//	print("This is the 404 Display!\n\r");
}


Display404::~Display404()
{
	delete painter;
	delete canvas;
}


void Display404::scrollUp()
{
	int y0 = vpadding*zoom;
	int lh = 8*zoom;			// line height
	int ls = vspacing*zoom; 	// line separation
	int dy = lh+ls;
	int bh = 3*lh + 2*ls;		// moving block height

	QTransform trans;
	painter->setWorldTransform(trans);

	painter->drawPixmap(QPoint(0,y0),canvas->copy(0, y0+dy, width(), bh));
	painter->eraseRect(0,y0+bh+ls, width(),lh);
	update(rect());

	trans.scale(zoom,zoom);
	painter->setWorldTransform(trans);
}


void Display404::printAt (int x, int y, uint8 c)
{
	int x0 = hpadding + x * (5+hspacing);
	int y0 = vpadding + y * (8+vspacing);

	uint8* q = font_data + c*8;
	uint8 m;

	painter->eraseRect(x0,y0, 5,8);
	for (y = y0; y < y0+8; y++)
	{
		if ((c = *q++))
		{
			for (x = x0, m = 0x10; m; x++, m>>=1)
			{
				if (c & m) painter->drawRect(x,y,1,1);
			}
		}
	}
	update(x, y, 5*zoom, 8*zoom);
}


void Display404::print (uchar c)
{
	if (c>=' ')
	{
		if (x>=40) { x%=40; y++; }
		if (y>=4)  { scrollUp(); y=3; }
		printAt(x,y,c); x++;
	}
	else
	{
		switch (c)
		{
		case 10:	y++; break;
		case 13:	x=0; break;
		default:	break;
		}
	}
}

void Display404::print (cstr s)
{
	while(*s) print(uchar(*s++));
}


// virtual protected
void Display404::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.drawPixmap(0,0,*canvas);

	if(hasFocus())
	{
		QPen pen(QColor(0x88,0xff,0x00,0x88));
		pen.setWidth(6);
		painter.setPen(pen);
//		painter.setCompositionMode(QPainter::CompositionMode_Darken);
		painter.drawRect(rect());
	}
}

// virtual protected
void Display404::focusInEvent(QFocusEvent* e)
{
	update(rect());
	QWidget::focusInEvent(e);
	emit focusChanged(1);
}

// virtual protected
void Display404::focusOutEvent(QFocusEvent* e)
{
	update(rect());
	QWidget::focusOutEvent(e);
	emit focusChanged(0);
}

















