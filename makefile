#========================================================================
# Project: none
# ----------------------------------------------------------------------
# Package: PPM
# Authors: Vilas Kumar Chitrakaran (cvilas@ces.clemson.edu)
# Start Date: Sat May 18 06:46:12 edt 2002
# Compiler: GNU GCC 2.96 and above
# Operating System: GNU/Linux 
# ----------------------------------------------------------------------
# File: makefile
#========================================================================

#Name of the package
PKG = PPM

# ----- Directories -----
INSTALLDIR = /usr/local
INSTALLHEADER= $(INSTALLDIR)/include/$(PKG)
INSTALLLIB= $(INSTALLDIR)/lib/
INSTALLBIN =
INSTALLSRC = 

# ----- Doxygen documentation parameters -----
DOCNAME = Math Library
DOCSOURCE = *.hpp
DOCTARGET = 

# Libraries, headers, and binaries that will be installed.
LIBS = libPPM.so
HDRS = PPM.hpp ConvolutionMask.hpp
SRC = *.hpp *.cpp

# ---- compiler options ----
CC = g++
LD = g++
CFLAGS = -Wall -fno-builtin -O2 -fpic -c
LDFLAGS =
INCLUDEHEADERS =
INCLUDELIBS =
OBJ = PPM.o ConvolutionMask.o
TARGET = $(LIBS)
INCLUDEHEADERS =
INCLUDELIBS =
CLEAN = rm -rf *.o lib* *.dat documentation/

# ========== Targets ==========

targets:	$(LIBS)
libPPM.so:	$(OBJ)
	$(LD) -shared -o $(LIBS) $(OBJ)
	

# ----- PPM -----
PPM.o: PPM.hpp PPM.cpp
	$(CC) $(CFLAGS) PPM.cpp $(INCLUDEHEADERS)

# ----- ConvolutionMask -----
ConvolutionMask.o: ConvolutionMask.hpp ConvolutionMask.cpp
	$(CC) $(CFLAGS) ConvolutionMask.cpp $(INCLUDEHEADERS)


# ---- make rules ----
clean:
	@echo
	@echo ----- Package $(PKG), Cleaning -----
	@echo
	$(CLEAN)
	if (test -d examples) ; then (cd examples; make clean);fi

install:
	@echo
	@echo ----- Package $(PKG), Installing to $(INSTALLDIR) -----
	@echo
	if !(test -d $(INSTALLLIB)); then (mkdir $(INSTALLLIB)); fi
	for i in ${LIBS}; do (cp $$i $(INSTALLLIB)); done
	if !(test -d $(INSTALLHEADER)); then (mkdir $(INSTALLHEADER)); fi
	for i in ${HDRS}; do (cp $$i $(INSTALLHEADER)); done

