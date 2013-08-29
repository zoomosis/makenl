cmake_minimum_required (VERSION 2.6)
project (makenl)

add_executable (makenl config.c crc16.c fileutil.c fts5.c lsttool.c
makenl.c merge.c mkdiff.c mklog.c msgtool.c output.c process.c stack.c
strtool.c upcont.c os.c)
