# MakeNL makefile for TinyCC on Linux
#
# Tested using:
#
# TinyCC 0.9.27
# GNU Make 4.2.1
# Ubuntu 20.04 LTS
#
# Build with:
#
# make -f makefile.tcc          (release) or
# make -f makefile.tcc DEBUG=1  (debug)
# make -f makefile.tcc clean    (remove temp files)
# make -f makefile.tcc I386=1   (32-bit Linux build)
# make -f makefile.tcc WIN32=1  (cross-compile to Win32)
# make -f makefile.tcc WIN64=1  (cross-compile to Win64)

CC=tcc

ifdef I386
CC=i386-tcc
endif

ifdef WIN32
CC=i386-win32-tcc
endif

ifdef WIN64
CC=x86_64-win32-tcc
endif

CFLAGS+= -Wall -W -O2

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
