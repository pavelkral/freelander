QT += core gui widgets network networkauth
CONFIG += c++17

NCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtOAuth

SOURCES += \
    src/calendartabledelegate.cpp \
    src/clickabletextedit.cpp \
    src/eventdialog.cpp \
    src/freelandercalendar.cpp \
    src/googleclient.cpp \
    src/main.cpp \
    src/mainwidget.cpp \
    src/settingsdialog.cpp \
    src/tokenmanager.cpp \
    src/utils/utils.cpp \
    src/utils/logger.cpp
    

HEADERS += \
    src/calendartabledelegate.h \
    src/clickabletextedit.h \
    src/eventdialog.h \
    src/freelandercalendar.h \
    src/googleclient.h \
    src/mainwidget.h \
    src/settingsdialog.h \
    src/tokenmanager.h \
    src/utils/utils.h \
    src/utils/logger.h

FORMS += \
    src/ui/eventdialog.ui \
    src/ui/mainwidget.ui \
    src/ui/settingsdialog.ui 

RESOURCES += resource/calendar.qrc


RC_FILE = resource/calendar.rc

# deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
      .gitignore \
      README.md
