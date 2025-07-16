cmake_minimum_required(VERSION 3.19)
project(freelander LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS src/forms)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    NetworkAuth
)

qt_standard_project_setup()

qt_add_executable(freelander
    #WIN32 MACOSX_BUNDLE
    src/calendartabledelegate.cpp
    src/clickabletextedit.cpp
    src/eventdialog.cpp
    src/freelandercalendar.cpp
    src/googleclient.cpp
    src/main.cpp
    src/mainwidget.cpp
    src/settingsdialog.cpp
    src/tokenmanager.cpp
    src/utils.cpp
    src/calendartabledelegate.h
    src/clickabletextedit.h
    src/eventdialog.h
    src/freelandercalendar.h
    src/googleclient.h
    src/mainwidget.h
    src/settingsdialog.h
    src/tokenmanager.h
    src/utils.h
    src/logger.h
	src/logger.cpp
    src/forms/eventdialog.ui
    src/forms/mainwidget.ui
    src/forms/settingsdialog.ui
    resource/calendar.qrc
    resource/calendar.rc
    README.md
    .gitignore LICENSE.txt

)

target_link_libraries(freelander
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::NetworkAuth

)

include(GNUInstallDirs)

install(TARGETS freelander
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET freelander
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})


# libs debug
# message(STATUS "Qt6_DIR: ${Qt6_DIR}")
# message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
# message(STATUS "CMAKE_AUTOMOC: ${CMAKE_AUTOMOC}")
# message(STATUS "CMAKE_AUTORCC: ${CMAKE_AUTORCC}")
# message(STATUS "CMAKE_AUTOUIC: ${CMAKE_AUTOUIC}")

