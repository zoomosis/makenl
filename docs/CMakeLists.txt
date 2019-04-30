# ########## MakeNL Documentation directory ##########

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
INCLUDE(pandocology)

add_document(
    TARGET makenl
    SOURCES makenl.md
    OUTPUT_FILE makenl.html
    PANDOC_DIRECTIVES --self-contained
    NO_EXPORT_PRODUCT
)

# ########## MakeNL documentation ##########
# doc files:
SET(makenl_DOCS
    hist31x.txt
    history.txt
    makenl.md
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
)

# ######### Installation ##########
# add install documentation:
INSTALL(FILES ${makenl_DOCS} DESTINATION share/doc/makenl)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/makenl.html DESTINATION share/doc/makenl)
INSTALL(FILES ${makenl_EXAMPLES} DESTINATION share/doc/makenl/examples)
INSTALL(FILES ${makenl_HISTORY} DESTINATION share/doc/makenl/history)
