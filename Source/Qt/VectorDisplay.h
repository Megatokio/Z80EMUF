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


#define USE_OGL		0
#define TRANSPARENT 1		// transparent window:  only QWidget
#define LUMINESCENT 0		// lumnescenct drawing: only QGLWidget
#define FRAMELESS   1		// only QWidget. QGLWidget: TODO ?

#define GREEN		1
#define WHITE		2
#define	AMBER		3
#define	BLUE		4
#define PAPERWHITE	5



#if USE_OGL
class VectorDisplay : public QGLWidget
#else
class VectorDisplay : public QWidget
#endif

{
	Q_OBJECT

// drawing commands
// Polylines are drawn. Last point of each Polyline is x = -0x8000.
	QPointF	wbu[8000];	// write buffer (unter construction)
	QPointF	rbu[8000];	// read buffer  (displayed)
	uint	wbucount;
	uint	rbucount;
	bool	move_pending;
	QPointF	DACs;


// Selftest animation timer:
	QTimer	t;

// HW scaling (transformation)
	qreal	a,b,c,d;

// Engine 1:
// read drawing commands from video ram
// can be used as a co-routine
// drawing commands are as believed to be most useful for real hardware:
//	dx and dy contain absolute value plus direction bit
//	decoded command to directly load X and Y into DAC registers
//	and to disable beam for one command and to stop drawing and raise an interrupt
	class Engine1
	{
	public:
		Engine1(VectorDisplay* d) :
			display(d), now(0), vram_mask(0), vram(nullptr), vram_self_allocated(false),
			A(0), X(0),Y(0), R(0),I(0),C(0),E(0) {}

		~Engine1() { if(vram_self_allocated) delete[] vram; }

	// back pointer:
		VectorDisplay*	display;

	// virtual time:
		uint32	now;

	// shared video ram
		uint	vram_mask;
		uint8*	vram;
		bool	vram_self_allocated;

	// registers:
		uint	A;			//	host-writable	address register (program counter)
		qreal 	X, Y;		//					beam position (DAC input values)
		bool	R;			//	reset:0			addressing mode (0=absolute / 1=relative)
		bool	I;			//	reset:0			interrupt active
		bool	C;			//	reset:0			cathode ray beam enable
		bool	E;			//	reset:0			clock enable

		uint8*	setVideoRam(uint size=16 kB, uint8* ram=nullptr);
		uint8*	getVideoRam()						{ return vram; }

		void	reset(uint32 cc=0);						// disable drawing until setAddress() is called
		void	setAddress(uint32 cc=0, uint addr=0);	// and start drawing (set register 'E' to 1)
		bool	run(uint32 cc);							// run co-routine. return value of register 'E'
		void	ffb(uint32 cc);
		void	test();
	};

	Engine1 e1;


public:
	explicit VectorDisplay(QWidget* parent);
			~VectorDisplay() override;

	void	setTransformation	(qreal a, qreal b, qreal c, qreal d);	// HW scaling & transformation
	void	setScale			(qreal f=1.0);							// undistorted, normally f ≤ 1.0
	void	setScaleAndRotation	(qreal f=1.0, qreal rad=0.0);			// ccw

	void	swap_buffers();

	void	lineto(const QPointF&);
	void	lineto(qreal x, qreal y);
	void	draw(qreal dx, qreal dy)	{ lineto(DACs.x()+dx,DACs.y()+dy); }

	void	moveto(const QPointF&);
	void	moveto(qreal x, qreal y);
	void	move(qreal dx, qreal dy)	{ moveto(DACs.x()+dx,DACs.y()+dy); }

	uint	print(uchar c, uint mask=0, qreal scale_x=1, qreal scale_y=1);	// append single char
	uint	print(cstr,    uint mask=0, qreal scale_x=1, qreal scale_y=1);	// append text


protected:
//	void	showEvent		(QShowEvent *) override;
	void	paintEvent		(QPaintEvent*) override;	// virtual protected
//	void	resizeEvent		(QResizeEvent*) override;	// virtual protected
	void	focusInEvent	(QFocusEvent*) override;		// virtual protected
	void	focusOutEvent	(QFocusEvent*) override;		// virtual protected
//	int		heightForWidth	(int w) const override;		// virtual
	void	resizeEvent		(QResizeEvent*) override;	// virtual protected

signals:
	void	focusChanged	(bool);
	void	resized			();

public slots:
//	void	update();

};



