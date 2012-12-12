TEMPLATE = app
CONFIG += console
CONFIG -= qt
INCLUDEPATH += /usr/lib/openmpi/include /usr/lib/openmpi/include/openmpi

LIBS += -L/usr/lib/openmpi/lib -lmpi_cxx -lmpi -lopen-rte -lopen-pal -ldl -Wl,--export-dynamic -lnsl -lutil -lm -ldl

SOURCES += main.cpp \
    matrice.cpp

HEADERS += \
    matrice.hpp

OTHER_FILES += \
    ex.txt \
    MNT_LB3.txt

