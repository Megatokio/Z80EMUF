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
#include "unix/os_utilities.h"
#include "unix/files.h"
#include "MainWindow.h"
#include <QApplication>
#include "Templates/Array.h"
#include "Z80/Z80.h"
#include "util.h"
#include <QSettings>
#include "Machine.h"
#include "VectorDisplay.h"


static cstr appl_name __attribute__((unused)) = APPL_NAME;
static uint verbose = 0;

static QApplication* appl;
static QSettings settings;


static __attribute((noreturn)) void usage(cstr msg)
{
	logline("%s", msg);
	logline("Z80EMUF: sample Z80 project");
	logline("usage: emuf [-v|--verbose] <epromfile>");
	logline("https://k1.spdns.de/Develop/Projects/Z80EMUF");
	abort();
}


int main(int argc, char *argv[])
{
	try
	{
		QByteArray ba = settings.value("romfile").toString().toUtf8();
		cstr romfilepath = ba.data();

		for(int i=1;i<argc;i++)
		{
			cstr arg = argv[i];
			if(arg[0]=='-')
			{
				if(eq(arg,"-h") || eq(arg,"--help"))		 usage("");
				else if(eq(arg,"-v") || eq(arg,"--verbose")) verbose++;
				else usage(catstr("unknown option ",arg));
			}
			else
			{
				romfilepath = arg;
			}
		}

		if(eq(romfilepath,"")) usage("rom file missing");
		romfilepath = fullpath(romfilepath);
		if(errno) abort("%s: %s", errorstr(errno), romfilepath);
		if(!is_file(romfilepath)) abort("not a regular file: %s", romfilepath);

	// =========================================================
	//	Initialize & run:
	// =========================================================

		QApplication a(argc, argv);	appl = &a;
		MainWindow w(nullptr, romfilepath);
		w.show();

		VectorDisplay d(nullptr);
		d.show();

		return a.exec();
	}
	catch(any_error& e)
	{
		abort("%s", e.what());
	}
}






















