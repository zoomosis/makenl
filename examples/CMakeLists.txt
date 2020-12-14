PROJECT(makenl-examples)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

SET(makenl_EXAMPLES
    cpyright.txt
    prolog.txt
    epilog.txt
    distrib.ctl
    hub.ctl
    net-l.ctl
    net-s.ctl
    node.ctl
    regional.ctl
    zone.ctl
)

INSTALL(FILES ${makenl_EXAMPLES} DESTINATION share/doc/makenl/examples)
