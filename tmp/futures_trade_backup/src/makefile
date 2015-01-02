ROOTDIR = .
include $(ROOTDIR)/make.begin.mk
include $(ROOTDIR)/make.env.mk

#CXX=/usr/bin/g++

BINDIR = ./bin
TESTDIR = ./test
OBJDIR = ./obj
TMPDIR = ./tmp
LIBDIR = ./lib

COREDIR = ./core
FEATUREDIR = ./feature
APIDIR = ./api
CTPAPIDIR = ./api/CTP
ZDAPIDIR = ./api/ZD
ALPHADIR = ./alpha
UTILSDIR = ./utils
STRATEGYDIR = ./strategy

CPPFLAGS=$(CPPFLAGS_COMMON) -D$(SPFLAG) -D$(DFLAG) -D$(BFLAG) -D$(XFLAG) -D$(SAHFLAG) #-Wsign-conversion 

LDFLAGS=$(LDFLAGS_COMMON) -L$(LIBDIR) -lSimulatorCore -lSimulatorCoreAlpha -lSimulatorCoreCTPApi -lSimulatorCoreStrategy -lSimulatorCoreUtils -I$(COREDIR) -I$(FEATUREDIR) -I$(CTPAPIDIR) -I$(ALPHADIR) -I$(UTILSDIR) -I$(STRATEGYDIR) -lSimulatorCoreFeature -I$(APIDIR) -lSimulatorCoreApi -I$(ZDAPIDIR) -lSimulatorCoreZDMDApi

LDFLAGSPROD=$(LDFLAGSPROD_COMMON) -L. -lthosttraderapi -lthostmduserapi -Wl,-rpath,.


ready:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)
	mkdir -p $(TMPDIR)
	mkdir -p $(LIBDIR)

clean:
	make -C core -f makefile clean
	make -C feature -f makefile clean
	make -C utils -f makefile clean
	make -C alpha -f makefile clean
	make -C api/CTP -f makefile clean
	make -C api/ZD -f makefile clean
	make -C api -f makefile clean
	make -C strategy -f makefile clean
	make -C test -f makefile clean
	echo Clean done

all: ready
	make -C core -f makefile all
	make -C feature -f makefile all
	make -C utils -f makefile all
	make -C alpha -f makefile all
	make -C api/CTP -f makefile all
	make -C api/ZD -f makefile all
	make -C api -f makefile all
	make -C strategy -f makefile all
	echo "all build is done!"

test: all
	make -C test -f makefile test

