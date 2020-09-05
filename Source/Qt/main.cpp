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


static __attribute((noreturn)) void usage (cstr msg = nullptr)
{
	if (msg) fprintf(stderr,"%s\n", msg);
	fprintf(stderr,"Z80EMUF: sample Z80 project\n");
	fprintf(stderr,"usage: Z80EMUF [-v|--verbose] <epromfile>\n");
	fprintf(stderr,"https://k1.spdns.de/Develop/Projects/Z80EMUF\n");
	abort();
}

int main (int argc, char *argv[])
{
	try
	{
		QByteArray ba = settings.value("romfile").toString().toUtf8();
		cstr romfilepath = ba.data();

		for (int i=1;i<argc;i++)
		{
			cstr arg = argv[i];
			if (arg[0]=='-')
			{
				if (eq(arg,"-h") || eq(arg,"--help")) usage();
				else if (eq(arg,"-v") || eq(arg,"--verbose")) verbose++;
				else usage(catstr("unknown option: ",arg));
			}
			else
			{
				romfilepath = arg;
			}
		}

		if (eq(romfilepath,"")) usage("rom file missing");
		romfilepath = fullpath(romfilepath);
		if (errno) abort("%s: %s", errorstr(errno), romfilepath);
		if (!is_file(romfilepath)) abort("not a regular file: %s", romfilepath);

	// =========================================================
	//	Initialize & run:
	// =========================================================

		QApplication a(argc, argv);	appl = &a;
		MainWindow w(nullptr, romfilepath);
		w.show();

		VectorDisplay d1(nullptr, QSize(1200,1000), VectorDisplay::BLUE);
		d1.setWindowTitle("VectorDisplay - Engine1");
		d1.show();
		runEngine1Demo(&d1);

		VectorDisplay d2(nullptr, QSize(500,500), VectorDisplay::WHITE);
		d2.setWindowTitle("VectorDisplay - Clock");
		d2.show();
		runClockDemo(&d2);

		VectorDisplay d3(nullptr, QSize(500,500), VectorDisplay::AMBER);
		d3.setWindowTitle("VectorDisplay - Clock");
		d3.show();
		runClockDemo(&d3);

		VectorDisplay d4(nullptr, QSize(500,500), VectorDisplay::PAPERWHITE);
		d4.setWindowTitle("VectorDisplay - Clock");
		d4.show();
		runClockDemo(&d4);

		return a.exec();
	}
	catch(AnyError& e)
	{
		abort("%s", e.what());
	}
}






















