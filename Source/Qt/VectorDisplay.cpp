/*	Copyright  (c)	Günter Woigk 2016 - 2019
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
#include "VectorDisplay.h"
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <math.h>
#include <QResizeEvent>
#include <QTimer>
#include <QGLWidget>
#include <QGLFormat>

#define L	(int8)254
#define E	(int8)255
#include "VectorDisplayFont.h"		// int8 vt_font_data[];

static uint vt_font_col1[256];

static void setup_once()
{
	//memset(vt_font_col1,0,sizeof(vt_font_col1));

	for(uint i = 0, c=' '; c<NELEM(vt_font_col1) && i<NELEM(vt_font_data); c++)
	{
		xxlogline(charstr(c));
		vt_font_col1[c] = i;
		i += 2;						// left-side mask and right-side mask

		while(vt_font_data[i] == L)
		{
			i++;
			while(vt_font_data[i]!=L && vt_font_data[i]!=E) i += 2;
		}

		assert(vt_font_data[i] == E);
		i++;
	}
}
#undef L
#undef E


/*	Notes:

  Base Class QWidget:

	Ein richtiges, transparentes Fenster bekommt man nur mit BaseClass QWidget.
	Der Fensterschatten lässt sich NICHT mit setAttribute(Qt::WA_MacNoShadow,true) ausschalten.
	Der Fensterschatten wird gemäß dem Fensterinhalt beim ersten Anzeigen bzw. bei Wechsel von
	Selected/Deselected (nur wenn nicht FramelessWindowHint) berechnet und danach "eingefroren".
	Hier wird man manuell einen ersten Screen anzeigen müssen, der komplett leer ist.

	Die Zeichenoberfläche wird vor jedem paintEvent() gelöscht (transparent/schwarz).
	Ein Nachleuchteffekt ist mit QWidget nicht möglich.

	Eine Synchronisierung mit dem FFB ist für QWidget nicht (?) möglich.

	Die Prozessorlast ist deutlich höher.

  Base Class QGLWidget

	Die gezeichneten Linien sind schlechter geglättet als beim QWidget.

	Ein transparentes Fenster lässt sich nicht (?) darstellen.
	TODO: eventuell die Zeichenoberfläche vorab alpha=0 löschen.

	Die Zeichenoberfläche wird nicht vorab gelöscht und enthält den alten Inhalt
	(bei Double-Buffering den des vor-dem-alten) der abgedimmt werden kann.
	Ein Nachleuchteffekt ist mit QQGWidget in einem opaquen Fenster einfach möglich.

	Eine Synchronisierung mit dem FFB ist möglich:
		QGLFormat.setSwapInterval(1) und QTimer.setInterval(0)
*/

//#define USE_OGL 1
//#define TRANSPARENT 0 	// transparent window:  only QWidget
//#define LUMINESCENT 1 	// lumnescenct drawing: only QGLWidget
//#define FRAMELESS 0		// only QWidget. QGLWidget: TODO ?
//#define GREEN 1

#if LUMINESCENT && !USE_OGL
  #error Lumineszenz geht nur mit QGLWidget!
#endif
#if TRANSPARENT && USE_OGL
  #error Transparente Fenster gehen nur mit QWidget!
#endif
#if TRANSPARENT && LUMINESCENT
  #error Transparent und Lumineszenz gehen nicht zusammen!
#endif


#define PHOSPHOR BLUE

#if PHOSPHOR==PAPERWHITE
#define red		0xFF
#define green	0xFF
#define blue	0xFF
#define alpha	0x80
#define BLUR	0xFF	// lumnescenct drawing: alpha = 0x40 … 0x80
#define lc		999
#define bg		1		// background brightness
#define gr		3		// grid lines brightness
#define dk		5		// dark color brightness
#elif PHOSPHOR==WHITE
#define red		0xFF
#define green	0xFF
#define blue	0xFF
#define alpha	0x80
#define BLUR	0x80
#define lc		1
#define bg		6		// background brightness
#define gr		9		// grid lines brightness
#define dk		33		// dark color brightness
#elif PHOSPHOR==AMBER
#define red		0xed
#define green	0xb8
#define blue	0x09
#define alpha	0x80
#define BLUR	0x40
#define lc		1
#define bg		4
#define gr		6
#define dk		8
#elif PHOSPHOR==BLUE
#define red		0x63//19
#define green	0x9f//90
#define blue	0xff
#define alpha	0x80
#define BLUR	0x60
#define lc		1
#define bg		4
#define gr		6
#define dk		8
#else//PHOSPHOR==GREEN
#define red		0xCC
#define green	0xFF
#define blue	0x22
#define alpha	0x80
#define BLUR	0x60
#define lc		1
#define bg		4
#define gr		6
#define dk		8
#endif
#define white 0xff



static const QColor darkcolor(red/dk,green/dk,blue/dk,0xff);	// maybe Osci grid lines
static const QColor gridcolor(red/gr,green/gr,blue/gr,0xff);	// maybe Osci grid lines
static const QColor fillcolor(red/bg,green/bg,blue/bg,0xff);	// background color
static const QColor blurcolor(red/bg,green/bg,blue/bg,BLUR);	// background color: with alpha: 0x40 … 0x80
static const QColor linecolor(red/lc,green/lc,blue/lc,alpha);	// foreground color: with alpha!


static const uint	SIZE = 256;


#if USE_OGL
QGLFormat myQGLformat()
{
	QGLFormat fmt;
	fmt.setSwapInterval(1);		// -> QTimer.setInterval(0) --> sync to FFB
	fmt.setDoubleBuffer(false);
	return fmt;
}
#endif


VectorDisplay::VectorDisplay(QWidget* parent)
:
#if USE_OGL
	QGLWidget(myQGLformat(),parent),
#else
	QWidget(parent),
#endif
	wbucount(0),
	rbucount(0),
	move_pending(true),
	DACs(0,0),
	a(0),b(0),c(0),d(0),
	e1(this)
{
	setup_once();

	float f = 2.0;

	resize(f*SIZE,f*SIZE);
	setMaximumWidth(1024);
	setMaximumHeight(1024);
	setMinimumWidth(SIZE);
	setMinimumHeight(SIZE);

	cstr title = USE_OGL ? "Vector OpenGL" : "Vector";
	if(TRANSPARENT) title = catstr(title," transparent");
	if(LUMINESCENT) title = catstr(title," luminescent");

	setWindowTitle(title);

	//QWidget::setEnabled(true);					// enable mouse & kbd events (true=default)
	setFocusPolicy(Qt::StrongFocus);				// sonst kriegt das Toolwindow manchmal keine KeyEvents

#if TRANSPARENT || LUMINESCENT
	setStyleSheet("background:transparent;");
	setAttribute(Qt::WA_TranslucentBackground);
#endif

#if FRAMELESS
	//setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
#endif

	//setAttribute(Qt::WA_OpaquePaintEvent,true);	// doesn't word: wir malen alle Pixel. Qt muss nicht vorher löschen.
	//setAttribute(Qt::WA_NoSystemBackground,true);	// ?
	//setAttribute(Qt::WA_MacNoShadow,true);		// doesn't work
	//setAttribute(Qt::WA_TintedBackground,false);	// ?

	// Configure the timer
	QObject::connect(&t, &QTimer::timeout, /*&Receiver::updateValue*/ [this](){e1.test();});

#if USE_OGL
	if(format().swapInterval() == -1)
	{
		// V_blank synchronization not available (tearing likely to happen)
		qDebug("Swap Buffers at v_blank not available: refresh at approx 60fps.");
		t.setInterval(17);
		t.start();
	}
	else
	{
		// V_blank synchronization available
		qDebug("Swap Buffers at v_blank AVAILABLE :-)");
		t.setInterval(0);
		t.start();
	}
#else
	t.setInterval(17);
	t.start();
#endif
}

VectorDisplay::~VectorDisplay()
{}

void VectorDisplay::focusInEvent(QFocusEvent* e)
{
	update();
	QWidget::focusInEvent(e);
	emit focusChanged(1);
}

void VectorDisplay::focusOutEvent(QFocusEvent* e)
{
	update();
	QWidget::focusOutEvent(e);
	emit focusChanged(0);
}

void VectorDisplay::resizeEvent(QResizeEvent*)
{
	QPainter(this).fillRect(this->rect(),fillcolor);
	update();
}

//void VectorDisplay::showEvent(QShowEvent* e)
//{
//	QWidget::showEvent(e);
//}

void VectorDisplay::paintEvent(QPaintEvent*)
{
	//printf("VectorDisplay::paintEvent\n");

	if(1)
	{
		static uint8 n = 0;
		static double stime = ::now();
		static double mw = 0.020;
		double d = ::now()-stime;
		stime += d;
		mw = mw*0.99 + d*0.01;
		if(++n==0) printf("d = %.4g mw = %.4g\n",d,mw);
	}

	// blur background (with alpha!):
	QPainter painter(this);
#if TRANSPARENT
//  #if USE_OGL
//	painter.setCompositionMode(QPainter::CompositionMode_Source);		// this only clears
//	painter.fillRect(this->rect(),Qt::transparent);						// to all-black (non-transparent)
//  #endif
#elif LUMINESCENT
	painter.fillRect(this->rect(),blurcolor);
#else // STANDARD
	painter.fillRect(this->rect(),fillcolor);
#endif


	// INFO:
	//	QTransform(m11, m12, m13, m21, m22, m23, m31, m32, m33 = 1.0)
	//	QTransform(m11, m12,      m21, m22,      dx,  dy)
	//	QTransform(a,   c,        b,   d,        dx,  dy)
	//
	//	The coordinates are transformed using the following formulas:
	//
	//	x' = m11*x + m21*y + dx
	//	y' = m22*y + m12*x + dy
	//	if (is not affine)
	//	{
	//		w' = m13*x + m23*y + m33
	//		x' /= w'
	//		y' /= w'
	//	}


	// scale CRT display to widget size
	// and flip vertically (0,0 is bottom left corner)
	// and apply distortion

	float f  = min(width(),height()) / (float)SIZE;
	float dx = width()/2  +f/2;
	float dy = height()/2 -f/2;
	painter.setWorldTransform( QTransform( a*f, c*f, -b*f, -d*f, dx,dy ) );

	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(linecolor, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter.setBrush(Qt::NoBrush);

	// draw test corners:
	// assumes full-size screen is set with a,b,c,d = 1,0,0,1
	painter.drawLine(-128,-128,-128,-118);
	painter.drawLine(-128,-128,-118,-128);
	painter.drawLine(+127,+127,+127,+117);
	painter.drawLine(+127,+127,+117,+127);

	// draw frame:
	QPointF* a = rbu;
	QPointF* e = a + rbucount;

	while(a<e)
	{
		QPointF* p = a;
		while(p<e && p->x() != -0x8000)
		{
			//printf("point %i,%i\n",p->x(),p->y());
			p++;
		}
		//printf("end of line\n");
		painter.drawPolyline(a,p-a);
		a = p+1;
	}

	// draw keyboard focus indicator:
	if(hasFocus())
	{
		painter.resetTransform();
		QPen pen(QColor(0x88,0xff,0x00,0x88));
		pen.setWidth(6);
		painter.setPen(pen);
		//painter.setCompositionMode(QPainter::CompositionMode_Darken);
		painter.drawRect(rect());
	}
}


/*	set or allocate video ram
	normally ram should be allocated by the host.
	then call setVideoRam with size and address.
	else call setVideoRam with size and NULL.
	size must be 2^N, 8kB/16kB (for 4096/8192 points) is recommended.
*/
uint8* VectorDisplay::Engine1::setVideoRam(uint size, uint8* ram)
{
	if(vram_self_allocated) { delete[] vram; vram = NULL; vram_self_allocated = false; }

	if(!ram) { ram = new uint8[size]; vram_self_allocated = true; }
	vram = ram;
	vram_mask = size-1;
	return ram;
}

/*	set HW transformation: scaling, rotation and distortion
	values should not exceed -1 / +1
	x = a*x + b*y
	y = c*x + d*y
	Set to 1,0,0,1 --> display area: -128 … x … +127, -128 … y … +127
*/
void VectorDisplay::setTransformation(qreal a, qreal b, qreal c, qreal d)
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

/*	set HW transformation: scaled only: undistorted and not rotated
	value should be in range -1 … +1
*/
void VectorDisplay::setScale(qreal f)
{
	a = d = f;
	b = c = 0;
}

/*	set HW transformation: undistorted: scaled and rotated
	scaling should be in range -1 … +1
	rotation should be in range -2PI … +2PI
*/
void VectorDisplay::setScaleAndRotation(qreal f, qreal rad)
{
	a = d = f * cosf(rad);
	b =     f * sinf(rad);
	c = -b;
}

// Helper:
void VectorDisplay::swap_buffers()
{
	memcpy(rbu,wbu,wbucount*sizeof(*rbu));
	rbucount = wbucount;
	wbucount = 0;
	move_pending = true;

	update();
}

// Helper:
void VectorDisplay::lineto(qreal x, qreal y)
{
	if(move_pending)
	{
		if(wbucount) wbu[wbucount++] = QPointF(-0x8000,0);	// polyline endmarker
		wbu[wbucount++] = DACs;
		move_pending = false;
	}

	//printf("line to (%i,%i)\n",x,y);

	DACs = QPointF(x,y);
	wbu[wbucount++] = DACs;
}

// Helper:
void VectorDisplay::lineto(const QPointF& p)
{
	if(move_pending)
	{
		if(wbucount) wbu[wbucount++] = QPointF(-0x8000,0);	// polyline endmarker
		wbu[wbucount++] = DACs;
		move_pending = false;
	}

	//printf("line to (%i,%i)\n",p.x(),p.y());

	DACs = p;
	wbu[wbucount++] = p;
}

// Helper:
void VectorDisplay::moveto(qreal x, qreal y)
{
	// Moves werden nicht sofort gespeichert,
	// da mehrere Moves direkt aufeinander folgen können

	//printf("move to (%i,%i)\n",x,y);

	DACs = QPointF(x,y);
	move_pending = true;
}

// Helper:
void VectorDisplay::moveto(const QPointF& p)
{
	//printf("move to (%i,%i)\n",p.x(),p.y());

	DACs = p;
	move_pending = true;
}


// append single char
uint VectorDisplay::print(uchar c, uint mask, qreal scale_x, qreal scale_y)
{
	const uint  i = vt_font_col1[c];
	int8* p = vt_font_data + i;
	const uint lmask = *p++;
	const uint rmask = *p++;

	if(mask & lmask)
		move(scale_x,0);

	int w = 0;					// width of char
	const qreal x0 = DACs.x();
	const qreal y0 = DACs.y();

	#define L	(int8)254
	#define E	(int8)255

	while(*p++ == L)
	{
		w = max(w,(int)*p);
		moveto(x0 + *p * scale_x, y0 + *(p+1) * scale_y);
		p+=2;

		while(*p!=E && *p!=L)
		{
			w = max(w,(int)*p);
			lineto(x0 + *p * scale_x, y0 + *(p+1) * scale_y);
			p+=2;
		}
	}

	assert(*(p-1) == E);

	#undef L
	#undef E

	moveto(x0 + (w+1) * scale_x, y0);

	return rmask;
}

// append text
uint VectorDisplay::print(cstr s, uint mask, qreal scale_x, qreal scale_y)
{
	while(*s) mask = print(*s++, mask, scale_x, scale_y);
	return mask;
}


// =================================================
//					Engine 1
// =================================================


/*	Push Reset
	disable drawing until setAddress() is called
*/
void VectorDisplay::Engine1::reset(uint32 cc)
{
	if(0) run(cc);				// for best emulation
	else  now = cc;				// for everyone else

	E = false;					// stop clock		(required)
	C = false;					// beam off			(safety)
	I = false;					// no interrupt		(required)

	// X = DAC counters are not reset!
	// Y = DAC counters are not reset!

	// rbucount = wbucount = 0;	// will be done by ffb
}

/*	Write to address register
	Set Address for drawing engine and start drawing a frame.
	for reading draw commands from ram the 'machine' must call run(cc).
	for making the current frame 'valid', the machine must call ffb(cc).
*/
void VectorDisplay::Engine1::setAddress(uint32 cc, uint address)
{
	if(0) run(cc);				// for best emulation
	else  now = cc;				// for everyone else

	A = address;
	R = true;		// next (first) command: relative address or control code
	I = false;		// interrupt off
	C = true;		// beam on
	E = true;		// enable drawing

	// X = DAC counters are not reset!
	// Y = DAC counters are not reset!
}

/*	'Frame Flyback'
	Finish a frame and rewind time base by cc
	move wbu[] to rbu[] for paintEvent()
	call update()		for paintEvent()
*/
void VectorDisplay::Engine1::ffb(uint32 cc)
{
	run(cc);		// catch up
	now -= cc;		// rewind time base

	display->swap_buffers();
}

/*	Definition of Control Codes in Drawing Commands:
*/
static const uint8
	CMD  = 0x80,	// dx = negative zero
	NOP	 = 0x00,	// dy
	Rrel = 8,
	Rabs = 0,
	Ion  = 4,		// raise Interrupt (if clock enable E is off)
	Con  = 2,
	Eon  = 1;

#define DRAW(dx,dy)	dx<0 ? 0x80 - dx : dx, dy<0 ? 0x80 - dy : dy
#define	MOVE(dx,dy) CMD, NOP + Eon + Rrel, DRAW(dx,dy)
#define MOVETO(x,y)	CMD, NOP + Eon + Rabs, (uint8)x, (uint8)y			// (0,0) = center of screen
#define END_FRAME	CMD, NOP
#define END_WITH_I	CMD, NOP + Ion

/*	Run Co-routine.
	returns value of register E:
		true  = still drawing
		false = end of frame
*/
bool VectorDisplay::Engine1::run(uint32 cc)
{
	if(!E) { now=cc; return false; }

	while(now<cc)
	{
		// read 2 bytes from ram:
		now += 6;
		A &= vram_mask;
		int x = (int8)vram[A++];
		int y = (int8)vram[A++];

		if(R)			// relative: draw, move or control code:
		{
			if(x == (int8)CMD)	// control code
			{
				R  =  y & 8;
			//	I  = (y & 4);					// interrupt = I=1 && E=0
				C  =  y & 2;
				E  =  y & 1;

				if(E) continue;

				I  = (y & 4);					// interrupt = I=1 && E=0
				return false;					// end of frame
			}

			// draw or move relative:

			now += max(x & 0x7F, y & 0x7F);		// consume time

			if(x & 0x80) x = -(x & 0x7F);		// x and y are stored as
			if(y & 0x80) y = -(y & 0x7F);		// bit7 = sign, bits0…6 = absolute value

			X = int8(X+x);			// TODO: actual HW will draw lines differently
			Y = int8(Y+y);			//		 on overflow

			if(C)	// draw visible line
			{
				display->lineto(X,Y);
				continue;
			}
			//else draw invisible line to x,y
		}
		else // set DAC counters directly to x,y
		{
			X = x;
			Y = y;
		}

		//printf("move to (%i,%i)\n",x,y);

		display->moveto(X,Y);

		R = true;
		C = true;
	}

	return true;	// still drawing
}


// Test drawing:
void VectorDisplay::Engine1::test()
{

	static float rad = 0;
	rad += M_PI/180*0.1;
	display->setScaleAndRotation(0.9,rad);

	display->moveto(-90,-50);
	display->print("Hallo Welt, hier kommt das Vektor-Display!",0,1.5,2.5);
	display->moveto(-90,-70);
	display->print(fromutf8str("[\\]{|}ÄÖÜäöüß"),0,1.5,2.0);

	if(!vram) setVideoRam( 8 * 1 kB );
	now=0;//ffb(now);
	reset(0);

	uint8 program[] =
	{
		#define BOX(W)				\
			MOVETO(-W,-W),			\
			DRAW(W,0), DRAW(W,0),	\
			MOVETO(-W,-W),			\
			DRAW(W,0), DRAW(W,0),	\
			MOVETO(-W,-W),			\
			DRAW(W,0), DRAW(W,0),	\
			DRAW(0,W), DRAW(0,W),	\
			DRAW(-W,0),DRAW(-W,0),	\
			DRAW(0,-W),DRAW(0,-W)	\

		BOX(100),
		BOX(99),

		MOVE(+6,+6),		// -97,-97
		DRAW(93,0),DRAW(93,0),

		MOVETO(0,0),
		DRAW(120,0),

		BOX(97),
		BOX(15),


		END_WITH_I
	};

	memcpy(vram,program,sizeof(program));

	setAddress(0,0);
	ffb(10000);
}











