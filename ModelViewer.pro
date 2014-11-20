SOURCES = ModelViewer.cc \
          ModelViewerWidget.cc \
          trackball.c 

HEADERS = ModelViewerWidget.h
CONFIG += qt debug
QT+= widgets opengl
QMAKE_CXXFLAGS += -std=c++11

release:DESTDIR = release
release:OBJECTS_DIR = release/.obj
release:MOC_DIR = release/.moc
release:RCC_DIR = release/.rcc
release:UI_DIR = release/.ui

debug:DESTDIR = debug
debug:OBJECTS_DIR = debug/.obj
debug:MOC_DIR = debug/.moc
debug:RCC_DIR = debug/.rcc
debug:UI_DIR = debug/.ui
