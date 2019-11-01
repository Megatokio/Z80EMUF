/*	Copyright  (c)	Günter Woigk 2013 - 2015
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


#ifndef _MainWindow_h_
#define _MainWindow_h_

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
virtual void keyPressEvent(QKeyEvent*);
virtual void keyReleaseEvent(QKeyEvent*);



public:
	MainWindow(QWidget* parent, cstr romfilepath);
	~MainWindow();


public slots:
	void	update();
	void	slotPowerOn();
	void	slotReset();
	void	slotNmi();
	void	slotSpeed(QAction*);
	void	slotQuitAppl();
};

#endif





















