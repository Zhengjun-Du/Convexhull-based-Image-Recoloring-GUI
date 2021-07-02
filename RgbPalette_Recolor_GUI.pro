#-------------------------------------------------
#
# Project created by QtCreator 2020-04-06T14:49:16
#
#-------------------------------------------------

QT       += core gui widgets datavisualization opengl
LIBS     += -lopengl32 -lglu32 -openmp
QMAKE_CXXFLAGS += -openmp

#note: linux -fopenmp


TARGET = Recolor_GUI
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



CONFIG += c++11

SOURCES += \
    data.cpp \
        main.cpp \
        mainwindow.cpp \
    opengl3dwidget.cpp \
    openglwidget.cpp \
    paletteviewwidget.cpp \
    rgbwidget.cpp \
    utility.cpp \
    videowidget.cpp

HEADERS += \
    data.h \
        mainwindow.h \
    opengl3dwidget.h \
    openglwidget.h \
    paletteviewwidget.h \
    rgbwidget.h \
    utility.h \
    videowidget.h


DEFINES += QUAZIP_BUILD

#INCLUDEPATH += $$PWDQt-Color-Widgets
include(Qt-Color-Widgets/color_widgets.pri)

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
