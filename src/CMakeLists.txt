
set (SOURCES
    bookmark.cpp
    download.cpp
    history.cpp
    mainwindow.cpp
    setting.cpp
    main.cpp
    )

set (HEADERS
    bookmark.h
    download.h
    history.h
    mainwindow.h
    setting.h
    )

QT5_WRAP_CPP(HEADERS_MOC ${HEADERS})
QT5_ADD_RESOURCES(RCC ${RESOURCE})

add_library(source ${SOURCES} ${HEADERS_MOC} ${RCC})

target_link_libraries(source)
