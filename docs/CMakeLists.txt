# ########## MakeNL Documentation directory ##########

# ########## MakeNL documentation ##########
# doc files:
SET(makenl_DOCS
    cpyright.txt
    distrib.ctl
    epilog.txt
    felten.txt
    fts-5000.txt
    hist31x.txt
    history.txt
    hub.ctl
    karcher.eng
    karcher.txt
    makenl.prn
    makenl.txt
    net-l.ctl
    net-s.ctl
    node.ctl
    prolog.txt
    read.me
    regional.ctl
    whatsnew.txt
    zone.ctl
)

# ######### Installation ##########
# add install documentation:
INSTALL(FILES ${makenl_DOCS} DESTINATION doc/makenl)

