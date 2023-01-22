QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    imagegenerator.cpp \
    imageprocessor.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    imagegenerator.h \
    imageprocessor.h \
    mainwindow.h

FORMS += \
    mainwindow.ui


INCLUDEPATH += C:\opencv\build\include
LIBS += C:\opencv-builded\bin\libopencv_core460.dll
LIBS += C:\opencv-builded\bin\libopencv_ml460.dll
LIBS += C:\opencv-builded\bin\libopencv_imgproc460.dll
LIBS += C:\opencv-builded\bin\libopencv_features2d460.dll
LIBS += C:\opencv-builded\bin\libopencv_photo460.dll
LIBS += C:\opencv-builded\bin\libopencv_objdetect460.dll
LIBS += C:\opencv-builded\bin\libopencv_videoio460.dll
LIBS += C:\opencv-builded\bin\libopencv_highgui460.dll
LIBS += C:\opencv-builded\bin\libopencv_gapi460.dll
LIBS += C:\opencv-builded\bin\libopencv_imgcodecs460.dll

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
