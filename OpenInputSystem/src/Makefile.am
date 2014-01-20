INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/includes $(CFLAGS) -I/usr/X11R6/include

#libOIS_LTLIBRARIES=libOIS.la
libOISdir = $(libdir)
libOIS_LIBRARIES = libOIS.a

libOIS_a_SOURCES = OISInputManager.cpp \
		 OISObject.cpp \
		 OISEffect.cpp \
		 OISKeyboard.cpp \
		 OISForceFeedback.cpp \
		 ./linux/EventHelpers.cpp \
		 ./linux/LinuxInputManager.cpp \
		 ./linux/LinuxJoyStick.cpp \
 		 ./linux/LinuxJoyStickEvents.cpp \
		 ./linux/LinuxForceFeedback.cpp \
		 ./linux/LinuxKeyboard.cpp \
		 ./linux/LinuxMouse.cpp

#libOIS_a_LIBFLAGS = -L/usr/X11R6/lib
#libOIS_a_LIBADD = -lX11 -lXaw
