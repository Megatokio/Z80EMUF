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


#include "MainWindow.h"
#include "QTimer"
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


uint32	ccps	= 1000020 * DEFAULT_CLOCK_MHZ;
uint	fps		= 100;
uint32	ccpf	= ccps/fps;

uint speed[] = {1,2,4,8,12,16,24,40,80,120,160,200,240};


MainWindow::MainWindow(QWidget* parent, cstr romfilepath)
:
	QMainWindow(parent),
	timer(new QTimer(this)),
	display(new Display404(this)),
	machine(new Machine(ccps,fps,romfilepath))
{
	resize(QSize(42*6*2,5*10*2));
	move(QPoint(64,64));

	setCentralWidget(display);
	setFixedSize(display->size());

	QMenu* mControl = new QMenu("Control",this);
	mControl->addAction("Power On", this, SLOT(slotPowerOn()),Qt::CTRL|Qt::Key_R|Qt::SHIFT);
	mControl->addAction("Reset",	this, SLOT(slotReset()),  Qt::CTRL|Qt::Key_R);
	mControl->addAction("Nmi",		this, SLOT(slotNmi()),    Qt::CTRL|Qt::Key_N);


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
	mbar->addMenu(mControl);
	mbar->addMenu(mSpeed);
	setMenuBar(mbar);

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
		sio->store_byte(e->text().at(0).toLatin1());
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
	machine->reset(machine->cpu->cc);
}


void MainWindow::slotPowerOn()
{
	machine->init();
}


void MainWindow::slotSpeed(QAction* a)
{
	int speed = a->data().toInt();
	machine->setSpeed(speed*1000000);
}













