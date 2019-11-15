#pragma once
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
#include <QWidget>


class Display404 : public QWidget
{
	Q_OBJECT

	QColor	 clear_color;	// color used to clear background
	QPainter* painter;		// painter in use
	QPixmap* canvas;

	int x,y;
	void printAt (int x, int y, uchar c);
	void scrollUp();

public:
	explicit Display404(QWidget* parent);
	~Display404() override;

	void print (cstr);
	void print (uchar);

protected:
	void paintEvent		(QPaintEvent*) override;
	//void resizeEvent 	(QResizeEvent*) override;
	void focusInEvent	(QFocusEvent*) override;
	void focusOutEvent	(QFocusEvent*) override;

signals:
	void focusChanged (bool);
	void resized ();

public slots:
	//void update();
};

















