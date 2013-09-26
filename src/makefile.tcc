# MakeNL makefile for Linux
# Tested with GNU Make 3.81 & TinyCC 0.9.26
#
# $Id: makefile.tcc,v 1.1 2013/09/26 18:40:17 ozzmosis Exp $
#
# Build with:
#
# make -f makefile.tcc          (release) or
# make -f makefile.tcc DEBUG=1  (debug)
# make -f makefile.tcc clean    (remove temp files)

TCCROOT=/opt/tcc
CC=$(TCCROOT)/bin/tcc
CFLAGS+= -Wall -W -B$(TCCROOT)/lib/tcc -O2

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
