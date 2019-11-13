#pragma once
/*	Copyright  (c)	Günter Woigk 2016 - 2019
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
#include <QWidget>
#include <QPointF>
#include <QTimer>
#include <QGLWidget>
#include "Templates/Array.h"
#include "unix/pthreads.h"

#ifndef USE_OGL			// settings.h
#define USE_OGL 1
#endif

enum DisplayStyle : uint
{
	OPAQUE=0,
	FRAMELESS=1,		// only QWidget. QGLWidget: TODO ?
	TRANSPARENT=2,		// only QWidget
	FADING=4,			// only QGLWidget
	DEFAULT = USE_OGL ? FADING : OPAQUE
};
inline DisplayStyle operator| (DisplayStyle a, uint b) { return DisplayStyle(uint(a) | b); }
inline DisplayStyle operator& (DisplayStyle a, uint b) { return DisplayStyle(uint(a) & b); }
inline void operator&= (DisplayStyle& a, uint b) { reinterpret_cast<uint&>(a) &= b; }
inline void operator|= (DisplayStyle& a, uint b) { reinterpret_cast<uint&>(a) |= b; }


#if USE_OGL
class VectorDisplay : public QGLWidget
#else
class VectorDisplay : public QWidget
#endif
{
	Q_OBJECT

	enum DrawingCmd
	{
		PushTransformation,
		PopTransformation,
		SetTransformation,
		SetScale,
		SetScaleAndRotation,
		ResetTransformation,

		SelectPen,		// inline: idx
		SelectBrush,	// inline: idx

		MoveTo,			// point
		DrawTo,			// point

		Move,			// dist
		Draw,			// dist
		DrawHor,		// dx
		DrawVer, 		// dy
		DrawText,		// inline: f_centered, char, ... '\0'
		SetTextSize,	// inline: scale_x, scale_y

		DrawLine,		// 2 points
		DrawPolyline,	// inline: count, args: count points
		DrawLines,		// inline: count, args: count * 2 points

		// closed:
		DrawRectangle,	// p1, p2
		DrawTriangle,	// p1, p2, p3
		DrawPolygon,	// inline: count, args: count points
		DrawConvexPolygon, // inline: count, args: count points
		DrawEllipse,	// p1, rx, ry
	};

	struct Display
	{	QSize  size;				// nominal scene size
		QColor background_color;  	// background color w/o alpha
		QColor blur_color;			// background color used for clearing / fading before drawing
		QPen   default_pen;			// default pen color
		QBrush default_brush;
		DisplayStyle style{OPAQUE};
		bool   synchronizes_with_vblank{no};
	} display;

	// HW scaling (transformation)
	qreal a=1,b=0,c=0,d=1;

	// defined values:
	Array<QPen> pens{0u,8u};
	Array<QBrush> brushes{0u,8u};

	// Drawing Pipeline:
	PLock pipeline_lock;

	// completed frame ready for display:
	Array<qreal> new_drawing_data;
	Array<uint8> new_drawing_commands;

	// current frame under construction:
	Array<qreal> drawing_data{0u,100000u};
	Array<uint8> drawing_commands{0u,1000u};

	// displayed frame returned from paint():
	Array<qreal> old_drawing_data{0u,100000u};
	Array<uint8> old_drawing_commands{0u,1000u};

public:
	struct ColorSet
	{
		QColor background;		// incl. alpha for fading = 0x40 .. 0x80; opaque = 0xff
		QColor lines;			// incl. alpha, dflt = 0x80
	};

	// predefined color sets:
	static const ColorSet AMBER, GREEN, BLUE, WHITE, PAPERWHITE;

	// ctor, dtor:
	explicit VectorDisplay (QWidget* parent, QSize widgetSize = {1000,750}, const ColorSet& = GREEN, DisplayStyle = DEFAULT);
	~VectorDisplay() override;

	// Display settings:
	void setDisplaySize (QSize size) noexcept	{ display.size = size; }	// logical size
	QSize displaySize() const noexcept			{ return display.size; }	// logical size
	void setDisplayColors (const ColorSet&) noexcept;
	void setBackgroundColor (QColor) noexcept;								// incl. alpha for luminescent fading
	void setBackgroundColor (int r, int g, int b, int a=0xff) noexcept;		// incl. alpha for luminescent fading
	uint setDisplayStyle (DisplayStyle) noexcept;
	bool setFrameless (bool=yes) noexcept;		// only QWidget
	bool setTransparent (bool=yes) noexcept;	// only QWidget
	bool setFading (bool=yes) noexcept;			// only QGLWidget
	bool synchronizesWithVblank() const noexcept{ return display.synchronizes_with_vblank; } // => drawFrame() is blocking

	// Define Pens (lines) and Brushes (fills):
	void definePen (uint idx, QPen) noexcept;
	void defineBrush (uint idx, QBrush) noexcept;
	void definePen (uint idx, QColor, qreal width=1.0) noexcept;
	void defineBrush(uint idx, QColor color) noexcept { defineBrush(idx, QBrush(color)); }

	// Drawing Commands:

	void setTransformation	(qreal a, qreal b, qreal c, qreal d) noexcept;	// display scaling & transformation
	void setScale			(qreal f=1.0) noexcept;							// undistorted, not rotated, normally f ≤ 1.0
	void setScaleAndRotation (qreal f=1.0, qreal rad=0.0) noexcept;			// cw
	void resetTransformation() noexcept;

	void selectPen (uint idx) noexcept;
	void setTextSize (qreal size=8.0, qreal width=1.0) noexcept;

	// drawing using the 'current position':
	void moveTo (qreal x, qreal y) noexcept;
	void drawTo (qreal x, qreal y) noexcept;
	void moveTo (QPointF) noexcept;
	void drawTo (QPointF) noexcept;
	void move (qreal dx, qreal dy) noexcept;
	void draw (qreal dx, qreal dy) noexcept;
	void drawHor (qreal dx) noexcept;
	void drawVer (qreal dy) noexcept;
	void print (char c) noexcept;
	void print (cstr s, bool centered=no) noexcept;

	// drawing at arbitrary positions:
	void drawLine (QLineF) noexcept;
	void drawLine (QPointF, QPointF) noexcept;
	void drawLine (qreal x1, qreal y1, qreal x2, qreal y2) noexcept;
	void drawLines (const QLineF* lines, uint count) noexcept;		// draw bunch of lines
	void drawPolyline (const QPointF* points, uint count) noexcept;	// draw count-1 connected lines

	// draw closed + filled shapes:
	void drawRectangle (const QRectF&) noexcept;
	void drawRectangle (const QPointF&, const QPointF&) noexcept;
	void drawRectangle (const QPointF&, const QSizeF&) noexcept;
	void drawRectangle (qreal x1, qreal y1, qreal x2, qreal y2) noexcept;
	void drawTriangle (const QPointF&, const QPointF&, const QPointF&) noexcept;
	void drawPolygon (const QPointF* points, uint count) noexcept;
	void drawPolygon (const Array<QPointF>& points) noexcept;
	void drawConvexPolygon (const QPointF* points, uint count) noexcept;
	void drawConvexPolygon (const Array<QPointF>& points) noexcept;
	void drawEllipse (const QPointF& center, qreal rx, qreal ry) noexcept;
	void drawCircle (const QPointF& center, qreal r) noexcept { drawEllipse(center,r,r); }

	void vsync() noexcept;		// doit!

protected:
	void	showEvent		(QShowEvent *) override;
	void	paintEvent		(QPaintEvent*) override;
	void	focusInEvent	(QFocusEvent*) override;
	void	focusOutEvent	(QFocusEvent*) override;
	//int  	heightForWidth	(int w) const override;
	void	resizeEvent		(QResizeEvent*) override;

signals:
	void	focusChanged	(bool);
	void	resized			();

public slots:
	//void	update();
};


class Engine1
{
	// Drawing Engine:
	// reads drawing commands from "video ram"
	// can be used as a co-routine
	// drawing commands may be implementable in real hardware:
	//	 dx and dy contain absolute value plus direction bit
	//	 decoded command to directly load X and Y into DAC registers
	//	 and to disable beam for one command and to stop drawing and raise an interrupt

public:
	explicit Engine1 (VectorDisplay*, uint vram_size = 16 kB, uint8* ram = nullptr);
	~Engine1()			{ if (vram_self_allocated) delete[] vram; }

// display:
	VectorDisplay*	display;

// virtual time:
	uint32	now;

// emulated shared video ram:
	uint	vram_mask;
	uint8*	vram;
	bool	vram_self_allocated;

// emulated registers:
	uint	A;			//	host-writable	address register (program counter)
	qreal 	X, Y;		//					beam position (DAC input values)
	bool	R;			//	reset:0			addressing mode (0=absolute / 1=relative)
	bool	I;			//	reset:0			interrupt active
	bool	C;			//	reset:0			cathode ray beam enable
	bool	E;			//	reset:0			clock enable

//	Control Codes in Drawing Commands:
	static const uint8
		CMD  = 0x80,	// dx = negative zero
		NOP	 = 0x00,	// dy
		Rrel = 8,
		Rabs = 0,
		Ion  = 4,		// raise Interrupt (if clock enable E is off)
		Con  = 2,
		Eon  = 1;

	uint8*	videoRam () const noexcept { return vram; }
	void	reset (uint32 cc=0);					// disable drawing until setAddress() is called
	void	start (uint32 cc=0, uint address=0);	// set address register and start drawing (set register 'E' to 1)
	bool	run (uint32 cc);						// run co-routine. return value of register 'E'
	void	vsync (uint32 cc);
};


extern void runEngine1Demo (VectorDisplay*);

extern void runClockDemo (VectorDisplay*);









