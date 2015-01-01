# ########## MakeNL Documentation directory ##########

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# ########## MakeNL documentation ##########
# doc files:
SET(makenl_DOCS
    hist31x.txt
    history.txt
    makenl.txt
    read.me
    whatsnew.txt
)
# doc files:
SET(makenl_EXAMPLES
    cpyright.txt
    distrib.ctl
    epilog.txt
    hub.ctl
    net-l.ctl
    net-s.ctl
    node.ctl
    prolog.txt
    regional.ctl
    zone.ctl
)
# historical doc files:
SET(makenl_HISTORY
    felten.txt
    fts-5000.txt
    karcher.eng
    karcher.txt
    makenl.prn
)

# ######### Installation ##########
# add install documentation:
INSTALL(FILES ${makenl_DOCS} DESTINATION share/doc/makenl)
INSTALL(FILES ${makenl_EXAMPLES} DESTINATION share/doc/makenl/examples)
INSTALL(FILES ${makenl_HISTORY} DESTINATION share/doc/makenl/history)

