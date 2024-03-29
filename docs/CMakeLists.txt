CMAKE_MINIMUM_REQUIRED(VERSION 2.8...3.19)
PROJECT(makenl-docs NONE)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
INCLUDE(pandocology)

add_document(
    TARGET makenl_html
    SOURCES makenl.md
    OUTPUT_FILE makenl.html
    PANDOC_DIRECTIVES --self-contained --metadata pagetitle='MakeNL NODELIST Generator Program by Ben Baker'
    NO_EXPORT_PRODUCT
)

add_document(
    TARGET whatsnew_html
    SOURCES whatsnew.md
    OUTPUT_FILE whatsnew.html
    PANDOC_DIRECTIVES --self-contained --metadata pagetitle='MakeNL Revision History'
    NO_EXPORT_PRODUCT
)

add_document(
    TARGET readme_html
    SOURCES readme.md
    OUTPUT_FILE readme.html
    PANDOC_DIRECTIVES --self-contained --metadata pagetitle='MakeNL read me first'
    NO_EXPORT_PRODUCT
)

SET(makenl_DOCS
    makenl.md
    readme.md
    whatsnew.md
    fts-5000.txt
)

INSTALL(FILES ${makenl_DOCS} DESTINATION share/doc/makenl)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/makenl.html DESTINATION share/doc/makenl)
INSTALL(FILES ${makenl_EXAMPLES} DESTINATION share/doc/makenl/examples)
