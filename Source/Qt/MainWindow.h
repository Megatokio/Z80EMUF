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

#include <QMainWindow>
#include "kio/kio.h"
class QKeyEvent;


class MainWindow : public QMainWindow
{
	Q_OBJECT

	class QTimer* timer;
	class Display404* display;
	class Machine* machine;

protected:
	virtual void keyPressEvent (QKeyEvent*) override;
	virtual void keyReleaseEvent (QKeyEvent*) override;

public:
	MainWindow (QWidget* parent, cstr romfilepath);
	~MainWindow() override;

public slots:
	void update();
	void slotPowerOn();
	void slotReset();
	void slotNmi();
	void slotSpeed(QAction*);
	void slotQuitAppl();
};























