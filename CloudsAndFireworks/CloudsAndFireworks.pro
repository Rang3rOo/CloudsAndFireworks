QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CloudsAndFireworks
TEMPLATE = app


SOURCES += main.cpp\
    resourcemanager.cpp \
    window.cpp \
    glmatrixstack.cpp \
    cloud.cpp \
    firework.cpp

HEADERS  += \
    resourcemanager.h \
    window.h \
    glmatrixstack.h \
    cloud.h \
    firework.h

FORMS    +=

RESOURCES += \
    resources.qrc

QMAKE_CXXFLAGS += -std=c++11

QMAKE_LFLAGS += -static -static-libgcc

RC_ICONS = firework.ico
