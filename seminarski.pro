QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$quote("C:\Open CV\opencv\release\install\include")

LIBS += $$quote("C:\Open CV\opencv\release\bin\libopencv_core454.dll")
LIBS += $$quote("C:\Open CV\opencv\release\bin\libopencv_highgui454.dll")
LIBS += $$quote("C:\Open CV\opencv\release\bin\libopencv_imgcodecs454.dll")
LIBS += $$quote("C:\Open CV\opencv\release\bin\libopencv_imgproc454.dll")
LIBS += $$quote("C:\Open CV\opencv\release\bin\libopencv_calib3d454.dll")
LIBS += $$quote("C:\Open CV\opencv\release\bin\libopencv_video454.dll")
LIBS += $$quote("C:\Open CV\opencv\release\bin\libopencv_videoio454.dll")


SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
