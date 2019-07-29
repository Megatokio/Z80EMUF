/*	Copyright  (c)	Günter Woigk 2013 - 2019
					mailto:kio@little-bat.de

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	Permission to use, copy, modify, distribute, and sell this software and
	its documentation for any purpose is hereby granted without fee, provided
	that the above copyright notice appear in all copies and that both that
	copyright notice and this permission notice appear in supporting
	documentation, and that the name of the copyright holder not be used
	in advertising or publicity pertaining to distribution of the software
	without specific, written prior permission.  The copyright holder makes no
	representations about the suitability of this software for any purpose.
	It is provided "as is" without express or implied warranty.

	THE COPYRIGHT HOLDER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
	INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
	EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
	CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
	DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
	TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
	PERFORMANCE OF THIS SOFTWARE.
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



const uint zoom = 3;		// 2
const uint hpadding = 6;
const uint hspacing = 1;
const uint width = (40*5 + 39*hspacing + 2*hpadding) * zoom;
const uint vpadding = 6;
const uint vspacing = 3;
const uint height = (4*8 + 3*vspacing + 2*vpadding ) * zoom;

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
//	print("1234567890123456789012345678901234567890");
//	print("ABCDEFGHIJKLMN\n\r");
//	print("abcdefghijklmn\n\r");
	print("This is the 404 Display!\n\r");
}


Display404::~Display404()
{
	delete painter;
	delete canvas;
}


void Display404::scrollUp()
{
	uint y0 = vpadding*zoom;
	uint lh = 8*zoom;			// line height
	uint ls = vspacing*zoom;	// line separation
	uint dy = lh+ls;
	uint bh = 3*lh + 2*ls;		// moving block height

	QTransform trans;
	painter->setWorldTransform(trans);

	painter->drawPixmap(QPoint(0,y0),canvas->copy(0, y0+dy, width(), bh));
	painter->eraseRect(0,y0+bh+ls, width(),lh);
	update(rect());

	trans.scale(zoom,zoom);
	painter->setWorldTransform(trans);
}


void Display404::printAt(int x, int y, uint8 c)
{
	int x0 = (hpadding + x*(5+hspacing));
	int y0 = (vpadding + y*(8+vspacing));

	uint8* q = font_data + c*8;
	uint8 m;

	painter->eraseRect(x0,y0, 5,8);
	for(y=y0;y<y0+8;y++)
	{
		if((c=*q++))
		{
			for(x=x0,m=0x10;m;x++,m>>=1)
			{
				if(c&m) painter->drawRect(x,y,1,1);
			}
		}
	}
	update(x,y,5*zoom,8*zoom);
}


void Display404::print(char c)
{
	if(c>=' ')
	{
		if(x>=40) { x%=40; y++; }
		if(y>=4)  { scrollUp(); y=3; }
		printAt(x,y,c); x++;
	}
	else
	{
		xxlog("<%u>",(uint)c);
		switch(c)
		{
		case 10:	y++; break;
		case 13:	x=0; break;
		default:	break;
		}
	}
}

void Display404::print(cstr s)
{
	while(*s) print(*s++);
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

















