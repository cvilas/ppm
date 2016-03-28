#======================================================================== 
# Project: 
# ---------------------------------------------------------------------- 
# Package:  
# Authors:   
# Start Date: Fri Sep 21 06:46:12 edt 2001
# ----------------------------------------------------------------------  
# File: makefile (makefile of examples)
#========================================================================  

LDFLAGS = g++ -Wall -fno-builtin -O2 -o
INCLUDEHEADERS = -I ../
INCLUDELIB = -L ../ -lPPM
TARGETS = targetFinding.t ppmTest.t ocr.t rangeImageSegmentation.t findCentroids.t

targets:	$(TARGETS)
CLEAN = rm -rf $(TARGETS) *.dat *.o *.t.ppm


# ----- targetFinding -----
targetFinding.t:	targetFinding.t.cpp
	$(LDFLAGS) $@ $? $(INCLUDEHEADERS) $(INCLUDELIB)

# ----- ppmTest -----
ppmTest.t:	ppmTest.t.cpp
	$(LDFLAGS) $@ $? $(INCLUDEHEADERS) $(INCLUDELIB)

# ----- ocr -----
ocr.t:	ocr.t.cpp
	$(LDFLAGS) $@ $? $(INCLUDEHEADERS) $(INCLUDELIB)

# ----- rangeImageSegmentation -----
rangeImageSegmentation.t:	rangeImageSegmentation.t.cpp
	$(LDFLAGS) $@ $? $(INCLUDEHEADERS) $(INCLUDELIB)

# ----- findCentroids -----
findCentroids.t:	findCentroids.t.cpp
	$(LDFLAGS) $@ $? $(INCLUDEHEADERS) $(INCLUDELIB)

# --- Rules ---

clean:
	@echo
	@echo ----- Cleaning example directory -----
	@echo
	$(CLEAN)
