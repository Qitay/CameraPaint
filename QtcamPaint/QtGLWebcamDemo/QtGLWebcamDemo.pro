QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtGLWebcamDemo
TEMPLATE = app

INCLUDEPATH+=../cqtopencvviewergl

SOURCES += main.cpp\
           qtglwebcamdemo.cpp \
           ../cqtopencvviewergl/cqtopencvviewergl.cpp

HEADERS  += qtglwebcamdemo.h \
            ../cqtopencvviewergl/cqtopencvviewergl.h

FORMS    += qtglwebcamdemo.ui

win32 {
message("Using win32 configuration")

OPENCV_PATH = D:/opencv # Note: update with the correct OpenCV version


#LIBS_PATH = "$$OPENCV_PATH/build/x86/mingw/lib" #project compiled using MINGW
LIBS_PATH = "$$OPENCV_PATH/build/x86/vc12/lib" #project compiled using Visual C++ 2010 32bit compiler

    CONFIG(debug, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_core2412d \
                -lopencv_highgui2412d \
                -lopencv_calib3d2412 \
                -lopencv_calib3d2412d \
                -lopencv_contrib2412 \
                -lopencv_contrib2412d \
                -lopencv_core2412 \
                -lopencv_core2412d \
                -lopencv_features2d2412 \
                -lopencv_features2d2412d \
                -lopencv_flann2412 \
                -lopencv_flann2412d \
                -lopencv_gpu2412 \
                -lopencv_gpu2412d \
                -lopencv_highgui2412 \
                -lopencv_highgui2412d \
                -lopencv_imgproc2412 \
                -lopencv_imgproc2412d \
                -lopencv_legacy2412 \
                -lopencv_legacy2412d \
                -lopencv_ml2412 \
                -lopencv_ml2412d \
                -lopencv_nonfree2412 \
                -lopencv_nonfree2412d \
                -lopencv_objdetect2412 \
                -lopencv_objdetect2412d \
                -lopencv_ocl2412 \
                -lopencv_ocl2412d \
                -lopencv_photo2412 \
                -lopencv_photo2412d \
                -lopencv_stitching2412 \
                -lopencv_stitching2412d \
                -lopencv_superres2412 \
                -lopencv_superres2412d \
                -lopencv_ts2412 \
                -lopencv_ts2412d \
                -lopencv_video2412 \
                -lopencv_video2412d \
                -lopencv_videostab2412 \
                -lopencv_videostab2412d
    }

    CONFIG(release, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_core2412d \
                -lopencv_highgui2412d \
                -lopencv_calib3d2412 \
                -lopencv_calib3d2412d \
                -lopencv_contrib2412 \
                -lopencv_contrib2412d \
                -lopencv_core2412 \
                -lopencv_core2412d \
                -lopencv_features2d2412 \
                -lopencv_features2d2412d \
                -lopencv_flann2412 \
                -lopencv_flann2412d \
                -lopencv_gpu2412 \
                -lopencv_gpu2412d \
                -lopencv_highgui2412 \
                -lopencv_highgui2412d \
                -lopencv_imgproc2412 \
                -lopencv_imgproc2412d \
                -lopencv_legacy2412 \
                -lopencv_legacy2412d \
                -lopencv_ml2412 \
                -lopencv_ml2412d \
                -lopencv_nonfree2412 \
                -lopencv_nonfree2412d \
                -lopencv_objdetect2412 \
                -lopencv_objdetect2412d \
                -lopencv_ocl2412 \
                -lopencv_ocl2412d \
                -lopencv_photo2412 \
                -lopencv_photo2412d \
                -lopencv_stitching2412 \
                -lopencv_stitching2412d \
                -lopencv_superres2412 \
                -lopencv_superres2412d \
                -lopencv_ts2412 \
                -lopencv_ts2412d \
                -lopencv_video2412 \
                -lopencv_video2412d \
                -lopencv_videostab2412 \
                -lopencv_videostab2412d
    }
}

unix {
message("Using unix configuration")

OPENCV_PATH = /usr/opencv2

LIBS_PATH = /usr/lib

LIBS     += \
    -L$$LIBS_PATH \
    -lopencv_core \
    -lopencv_highgui
}

INCLUDEPATH += \
    $$OPENCV_PATH/build/include/ \ #core module
    $$OPENCV_PATH/modules/highgui/include/ #highgui modul

message("OpenCV path: $$OPENCV_PATH")
message("Includes path: $$INCLUDEPATH")
message("Libraries: $$LIBS")


