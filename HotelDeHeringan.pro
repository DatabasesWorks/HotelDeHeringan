#-------------------------------------------------
#
# Project created by QtCreator 2018-07-19T14:54:28
#
#-------------------------------------------------

QT       += core gui sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HotelDeHeringan
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "C:\\Wt\\include"
INCLUDEPATH += "C:\\botan2\\include"


LIBS += -L"C:\\Wt\\lib" -lwtdbosqlite3d -lwtdbod -lwtd
LIBS += -L"C:\\botan2\\" -lbotan
LIBS += -L"C:\\boost_lib\\stage\\more\\lib" -llibboost_date_time-vc140-mt-gd-x64-1_68


SOURCES += main.cpp\
        mainwindow.cpp \
        login_dialog.cpp \
        role_definition_dialog.cpp \
        reservation_dialog.cpp \
    splash_screen.cpp \
    admin_request_dialog.cpp \
    utilities.cpp \
    guest_main_window.cpp \
    camera_capture.cpp

HEADERS  += mainwindow.hpp \
    login_dialog.hpp \
    role_definition_dialog.hpp \
    reservation_dialog.hpp \
    splash_screen.hpp \
    admin_request_dialog.hpp \
    utilities.hpp \
    guest_main_window.hpp \
    camera_capture.hpp

RESOURCES += \
    icons.qrc

FORMS += \
    login_dialog.ui \
    role_definition_dialog.ui \
    reservation_dialog.ui \
    admin_request_dialog.ui \
    guest_main_window.ui
