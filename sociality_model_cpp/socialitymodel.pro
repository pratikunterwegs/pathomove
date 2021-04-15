QT -= gui

CONFIG += c++17
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

R_HOME = $$system(R RHOME)

## include headers and libraries for R
RCPPFLAGS =         $$system($$R_HOME/bin/R CMD config --cppflags)
RLDFLAGS =      $$system($$R_HOME/bin/R CMD config --ldflags)
RBLAS =         $$system($$R_HOME/bin/R CMD config BLAS_LIBS)
RLAPACK =       $$system($$R_HOME/bin/R CMD config LAPACK_LIBS)

## if you need to set an rpath to R itself, also uncomment
RRPATH =        -Wl,-rpath,$$R_HOME/lib

## include headers and libraries for Rcpp interface classes
## note that RCPPLIBS will be empty with Rcpp (>= 0.11.0) and can be omitted
RCPPINCL =      $$system($$R_HOME/bin/Rscript -e \"Rcpp:::CxxFlags\(\)\")
RCPPLIBS =      $$system($$R_HOME/bin/Rscript -e \"Rcpp:::LdFlags\(\)\")

SOURCES += \
        main.cpp \
        ../src/simulations.cpp

HEADERS += \
    ../src/agents.h \
    ../src/ecology.hpp \
    ../src/landscape.h \
    ../src/network.h \
    ../src/network_operations.hpp \
    ../src/parameters.h \
    ../src/data_types.h

QMAKE_CXXFLAGS += $$RCPPWARNING $$RCPPFLAGS $$RCPPINCL
QMAKE_LIBS += $$RLDFLAGS $$RBLAS $$RLAPACK $$RCPPLIBS

LIBS += -L/usr/local/lib -lgsl -lgslcblas -lm \
        -lboost_system


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
