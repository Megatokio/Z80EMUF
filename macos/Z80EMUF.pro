
QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Z80EMUF
TEMPLATE = app
INCLUDEPATH += Source Source/Machine Libraries

CONFIG += c++14
CONFIG += precompiled_header
CONFIG(release,debug|release) { DEFINES += NDEBUG } # ATTN: curly brace must start in same line!

QMAKE_MAC_SDK = macosx10.9


SOURCES +=					\
	Libraries/unix/log.cpp			\
	Libraries/kio/kio.cpp			\
	Libraries/kio/exceptions.cpp	\
	Libraries/cstrings/cstrings.cpp	\
	Libraries/unix/tempmem.cpp		\
	Libraries/unix/os_utilities.cpp	\
	Libraries/unix/FD.cpp			\
	Libraries/unix/files.cpp		\
	Libraries/unix/pthreads.cpp		\
	\
	Source/Qt/main.cpp				\
	Source/Qt/MainWindow.cpp		\
	Source/Qt/Display404.cpp		\
	Source/Qt/VectorDisplay.cpp		\
	Source/Machine/Z80/Z80.cpp		\
	Source/Machine/Item.cpp			\
	Source/Machine/Machine.cpp		\
	Source/Machine/Sio.cpp			\
	Source/Machine/SystemTimer.cpp	\
	Source/Machine/Mmu.cpp			\

HEADERS  +=					\
	config.h						\
	Libraries/kio/kio.h				\
	Libraries/kio/exceptions.h		\
	Libraries/kio/errors.h			\
	Libraries/kio/standard_types.h	\
	Libraries/kio/peekpoke.h		\
	Libraries/cstrings/cstrings.h	\
	Libraries/unix/log.h			\
	Libraries/unix/tempmem.h		\
	Libraries/unix/os_utilities.h	\
	Libraries/unix/FD.h				\
	Libraries/unix/files.h			\
	Libraries/unix/s_type.h			\
	Libraries/unix/pthreads.h		\
	Libraries/unix/MyFileInfo.h		\
	Libraries/Templates/Array.h		\
	Libraries/Templates/sort.h		\
	\
	Source/settings.h				\
	Source/custom_errors.h			\
	Source/util.h					\
	\
	Source/Qt/MainWindow.h			\
	Source/Qt/Display404.h			\
	Source/Qt/Display404Font.h		\
	Source/Qt/VectorDisplay.h		\
	Source/Qt/VectorDisplayFont.h	\
	Source/Machine/Z80/Z80.h		\
	Source/Machine/Z80/Z80macros.h	\
	Source/Machine/Z80/Z80opcodes.h	\
	Source/Machine/Z80options.h		\
	Source/Machine/Z80/Z80opcodes.h	\
	Source/Machine/Z80/Z80core.h	\
	Source/Machine/Item.h			\
	Source/Machine/IsaObject.h		\
	Source/Machine/Machine.h		\
	Source/Machine/Sio.h			\
	Source/Machine/SystemTimer.h	\
	Source/Machine/Mmu.h			\

