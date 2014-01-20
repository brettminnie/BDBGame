if OIS_BUILD_OGRE_SAMPLE
SUBDIRS = ogre
endif

INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/includes $(CFLAGS) -I/usr/X11R6/include

noinst_PROGRAMS = ConsoleApp FFConsoleTest

ConsoleApp_SOURCES = OISConsole.cpp
ConsoleApp_LDFLAGS = -L$(top_builddir)/src -L/usr/X11R6/lib
ConsoleApp_LDADD = -lOIS -lX11 -lXaw

FFConsoleTest_SOURCES = FFConsoleDemo.cpp
FFConsoleTest_LDFLAGS = -L$(top_builddir)/src -L/usr/X11R6/lib
FFConsoleTest_LDADD = -lOIS -lX11 -lXaw
