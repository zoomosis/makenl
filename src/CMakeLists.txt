# ########## MakeNL Source Directory ##########

# ########## MakeNL Executable ##########
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

# add install target for executable:
INSTALL(TARGETS makenl DESTINATION bin)
# add install target for man page:
INSTALL(FILES makenl.1 DESTINATION share/man/man1)
