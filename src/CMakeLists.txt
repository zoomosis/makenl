CMAKE_MINIMUM_REQUIRED(VERSION 2.8...3.19)
PROJECT(makenl-src C)

ADD_EXECUTABLE (makenl
    config.c
    crc16.c
    fileutil.c
    fts5.c
    lsttool.c
    makenl.c
    merge.c
    mkdiff.c
    mklog.c
    msgtool.c
    output.c
    procfile.c
    stack.c
    strtool.c
    upcont.c
    os.c
)

INSTALL(TARGETS makenl DESTINATION bin)

INSTALL(FILES makenl.1 DESTINATION share/man/man1)
