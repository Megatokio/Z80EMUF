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


#include "MainWindow.h"
#include <QTimer>
#include <QSettings>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QActionGroup>
#include "Display404.h"
#include "Machine.h"
#include "Sio.h"
#include "Z80/Z80.h"
#include <QCoreApplication>

static int32 ccps	= 1000020 * DEFAULT_CLOCK_MHZ;	// cc per second
static int   fps 	= 100;							// frames per second
static int32 ccpf	= ccps/fps;						// cc per frame

static int  speed[] = {1,2,4,8,12,16,24,40,80,120,160,200,240,300};


MainWindow::MainWindow(QWidget* parent, cstr romfilepath)
:
	QMainWindow(parent),
	timer(new QTimer(this)),
	display(new Display404(this)),
	machine(new Machine(ccps,fps,romfilepath))
{
	QMenu* mControl = new QMenu("Control",this);
	mControl->addAction("Power On", this, &MainWindow::slotPowerOn,Qt::CTRL|Qt::Key_R|Qt::SHIFT);
	mControl->addAction("Reset",	this, &MainWindow::slotReset,  Qt::CTRL|Qt::Key_R);
	mControl->addAction("Nmi",		this, &MainWindow::slotNmi,    Qt::CTRL|Qt::Key_N);
	mControl->addSeparator();
	mControl->addAction("Quit",		this, &MainWindow::slotQuitAppl, Qt::CTRL|Qt::Key_Q);

	QMenu* mSpeed = new QMenu("Speed",this);

	QActionGroup* speedActionGroup = new QActionGroup(this);
	for(uint i=0;i<NELEM(speed);i++)
	{
		QAction* a = new QAction(usingstr("%i MHz",speed[i]),mSpeed);
		a->setCheckable(true);
		a->setChecked(speed[i] == ccps/1000000);
		a->setData(QVariant(speed[i]));
		speedActionGroup->addAction(a);
		mSpeed->addAction(a);
	}
	connect(speedActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(slotSpeed(QAction*)));

	QMenuBar* mbar = new QMenuBar(this);
	mbar->setNativeMenuBar(true);
	mbar->addMenu(mControl);
	mbar->addMenu(mSpeed);
	setMenuBar(mbar);

	move(QPoint(200,200));
	setCentralWidget(display);
	setFixedSize(minimumSizeHint());

	connect(timer,SIGNAL(timeout()),this,SLOT(update()));
	timer->start(1000/25);
}

MainWindow::~MainWindow()
{
	timer->stop();
	delete machine;
}


/*	Timer interrupt:
*/
void MainWindow::update()
{
	Sio* sio = machine->sio_A;

	if(sio->obu_avail())
	{
		do { display->print(sio->get_byte()); } while(sio->obu_avail());
		display->update();
	}
}


// virtual
void MainWindow::keyPressEvent(QKeyEvent* e)
{
	if(e->text().count()==0) return;
	Sio* sio = machine->sio_A;
	if(sio->ibu_free())
		sio->store_byte(uchar(e->text().at(0).toLatin1()));
	else
		logline("sio A: input overflow");
}

// virtual
void MainWindow::keyReleaseEvent(QKeyEvent*)
{
}


void MainWindow::slotNmi()
{
	machine->nmi();
}


void MainWindow::slotReset()
{
	machine->reset();
}


void MainWindow::slotPowerOn()
{
	machine->poweron();
}


void MainWindow::slotSpeed (QAction* a)
{
	int speed = a->data().toInt();
	machine->setSpeed(speed*1000000);
}


void MainWindow::slotQuitAppl()
{
	QCoreApplication::quit();
}











