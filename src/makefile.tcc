# MakeNL makefile for TinyCC on Linux
# Tested with GNU Make 3.81 & TinyCC 0.9.26
#
# $Id: makefile.tcc,v 1.2 2013/09/26 20:43:45 ozzmosis Exp $
#
# Build with:
#
# make -f makefile.tcc          (release) or
# make -f makefile.tcc DEBUG=1  (debug)
# make -f makefile.tcc clean    (remove temp files)
# make -f makefile.tcc I386=1   (32-bit build)
# make -f makefile.tcc WIN32=1  (cross-compile to Win32)
# make -f makefile.tcc WIN64=1  (cross-compile to Win64)

TCCROOT=/opt/tcc
CC=$(TCCROOT)/bin/tcc
INTLIB=$(TCCROOT)/lib/tcc

ifdef I386
CC=$(TCCROOT)/bin/i386-tcc
endif

ifdef WIN32
CC=$(TCCROOT)/bin/i386-win32-tcc
INTLIB=$(TCCROOT)/lib/tcc/win32
endif

ifdef WIN64
CC=$(TCCROOT)/bin/x86_64-win32-tcc
INTLIB=$(TCCROOT)/lib/tcc/win32
endif

CFLAGS+= -Wall -W -B$(INTLIB) -O2

ifdef DEBUG
CFLAGS+= -g
else
LDFLAGS+= -s
endif

ifdef TESTING
CFLAGS+= -DTESTING
OBJS+=testing.o
endif

OBJS+=config.o crc16.o fileutil.o fts5.o lsttool.o makenl.o merge.o
OBJS+=mkdiff.o mklog.o msgtool.o os.o output.o procfile.o stack.o
OBJS+=strtool.o upcont.o

.c.o:
	$(CC) $(CFLAGS) -c $<

makenl: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) makenl
