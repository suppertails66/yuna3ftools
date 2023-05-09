IDIR := src
SRCDIR := src
ODIR := obj
LDIR :=

BLACKTDIR = ../blackt
LIBPSXDIR = ../libpsx
DISCASTERDIR = ../discaster

AR=ar
CXX=g++
# Compile only
CDEFINES = -DBLACKT_ENABLE_LIBPNG
#CLIBS = -lpng
CFLAGS = -std=gnu++11 -O2 -Wall -L${LIBPSXDIR} -lpsx -L${BLACKTDIR} -lblackt -lpng
CINCLUDES = -I${BLACKTDIR}/src -I${LIBPSXDIR}/src
CXXFLAGS=$(CFLAGS) $(CDEFINES) $(CINCLUDES) -I$(IDIR)

LIBDEPS := $(LIBPSXDIR)/libpsx.a $(BLACKTDIR)/libblackt.a

TOOLSRCS := $(wildcard $(SRCDIR)/*.cpp)
TOOLSINDIR := $(patsubst %.cpp,%,$(TOOLSRCS))
TOOLS := $(notdir $(TOOLSINDIR))

.SECONDEXPANSION:

all: blackt libpsx discaster $(TOOLS)

blackt:
#	cd ${BLACKTDIR} && $(MAKE) && cd $(CURDIR)
	$(MAKE) -C${BLACKTDIR}

libpsx:
#	cd ${LIBPSXDIR} && $(MAKE) && cd $(CURDIR)
	$(MAKE) -C${LIBPSXDIR}

discaster:
	$(MAKE) -C${DISCASTERDIR}

$(TOOLS): $(SRCDIR)/$$@.cpp $(LIBDEPS)
	make blackt
	make libpsx
	make discaster
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $(notdir $@) $(CXXFLAGS)

.PHONY: blackt libpsx discaster cleanme clean

cleanme:
	rm -f $(TOOLS)

clean: cleanme
#	rm -f $(LIB)
#	rm -rf $(ODIR)
#	cd ${BLACKTDIR} && $(MAKE) clean && cd $(CURDIR)
#	cd ${LIBPSXDIR} && $(MAKE) clean && cd $(CURDIR)
	$(MAKE) -C${BLACKTDIR} clean
	$(MAKE) -C${LIBPSXDIR} clean
	$(MAKE) -C${DISCASTERDIR} clean