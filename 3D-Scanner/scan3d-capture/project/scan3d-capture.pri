#
# vim:filetype=qmake sw=4 ts=4 expandtab nospell
#

HEADERS += \
        $$SOURCEDIR/io_util.hpp \
        $$SOURCEDIR/im_util.hpp \
        $$SOURCEDIR/Application.hpp \
        $$SOURCEDIR/MainWindow.hpp \
        $$SOURCEDIR/AboutDialog.hpp \
        $$SOURCEDIR/ProcessingDialog.hpp \
        $$SOURCEDIR/CalibrationDialog.hpp \
        $$SOURCEDIR/CaptureDialog.hpp \
        $$SOURCEDIR/VideoInput.hpp \
        $$SOURCEDIR/ImageLabel.hpp \
        $$SOURCEDIR/ProjectorWidget.hpp \
        $$SOURCEDIR/TreeModel.hpp \
        $$SOURCEDIR/CalibrationData.hpp \
        $$SOURCEDIR/structured_light.hpp \
        $$SOURCEDIR/scan3d.hpp \
        $$SOURCEDIR/GLWidget.hpp \
        $$(NULL)

SOURCES += \
        $$SOURCEDIR/main.cpp \
        $$SOURCEDIR/io_util.cpp \
        $$SOURCEDIR/im_util.cpp \
        $$SOURCEDIR/Application.cpp \
        $$SOURCEDIR/MainWindow.cpp \
        $$SOURCEDIR/AboutDialog.cpp \
        $$SOURCEDIR/CaptureDialog.cpp \
        $$SOURCEDIR/VideoInput.cpp \
        $$SOURCEDIR/ProcessingDialog.cpp \
        $$SOURCEDIR/CalibrationDialog.cpp \
        $$SOURCEDIR/ImageLabel.cpp \
        $$SOURCEDIR/ProjectorWidget.cpp \
        $$SOURCEDIR/TreeModel.cpp \
        $$SOURCEDIR/CalibrationData.cpp \
        $$SOURCEDIR/structured_light.cpp \
        $$SOURCEDIR/scan3d.cpp \
        $$SOURCEDIR/GLWidget.cpp \
        $$(NULL)

RESOURCES = $$ASSETSDIR/assets.qrc

macx {
    HEADERS += $$SOURCEDIR/VideoInput_QTkit.hpp
    OBJECTIVE_SOURCES += $$SOURCEDIR/VideoInput_QTkit.mm
}

USE_COGNEX {
    HEADERS += \
               $$SOURCEDIR/cognex_util.hpp \
               $$SOURCEDIR/CognexCamera.hpp \
               $$SOURCEDIR/CognexCameraThread.hpp \
               $$(NULL)
    SOURCES += \
               $$SOURCEDIR/cognex_util.cpp \
               $$SOURCEDIR/CognexCamera.cpp \
               $$SOURCEDIR/CognexCameraThread.cpp \
               $$(NULL)
}

FORMS = \
        $$FORMSDIR/MainWindow.ui \
        $$FORMSDIR/CaptureDialog.ui \
        $$FORMSDIR/AboutDialog.ui \
        $$FORMSDIR/ProcessingDialog.ui \
        $$FORMSDIR/CalibrationDialog.ui \
        $$(NULL)
