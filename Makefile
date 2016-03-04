# Variables.
CC=clang++
INCPATH = ./
SRCPATH = ./src
LIBPATH = ./lib
OUTPATH = ./output
ROOTPATH = $(shell root-config --incdir)
INCFLAGS = -I$(INCPATH)
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = -L$(ROOTSYS)/lib $(shell root-config --libs)

LINKFLAGS = -L$(LIBPATH) $(ROOTLIBS)
CXXFLAGS = --std=c++11 $(INCFLAGS) $(ROOTCFLAGS) 

# Rules.
.PHONY: all

libs: Range PlotMacro1D Cut Selection ObjectDefinition Analysis Test

all: Range PlotMacro1D Cut Selection ObjectDefinition Analysis Test
#AnalysisTools

Range:
	@echo "[Compiling: $@]"
	$(CC) src/$@.cxx -o lib/$@.o -c -Wall -Werror -fpic -O2 $(CXXFLAGS)
	$(CC) -shared -o lib/lib$@.so lib/$@.o $(ROOTLIBS) $(LINKFLAGS)

PlotMacro1D:
	@echo "[Compiling: $@]"
	$(CC) src/$@.cxx -o lib/$@.o -c -Wall -Werror -fpic -O2 $(CXXFLAGS)
	$(CC) -shared -o lib/lib$@.so lib/$@.o $(ROOTLIBS) $(LINKFLAGS) -lRange

Cut:
	@echo "[Compiling: $@]"
	$(CC) src/$@.cxx -o lib/$@.o -c -Wall -Werror -fpic -O2 $(CXXFLAGS)
	$(CC) -shared -o lib/lib$@.so lib/$@.o $(ROOTLIBS) $(LINKFLAGS) -lRange -lPlotMacro1D

Selection:
	@echo "[Compiling: $@]"
	$(CC) src/$@.cxx -o lib/$@.o -c -Wall -Werror -fpic -O2 $(CXXFLAGS)
	$(CC) -shared -o lib/lib$@.so lib/$@.o $(ROOTLIBS) $(LINKFLAGS) -lCut -lPlotMacro1D

ObjectDefinition:
	@echo "[Compiling: $@]"
	$(CC) src/$@.cxx -o lib/$@.o -c -Wall -Werror -fpic -O2 $(CXXFLAGS)
	$(CC) -shared -o lib/lib$@.so lib/$@.o $(ROOTLIBS) $(LINKFLAGS) -lSelection -lCut

Analysis:
	@echo "[Compiling: $@]"
	$(CC) src/$@.cxx -o lib/$@.o -c -Wall -Werror -fpic -O2 $(CXXFLAGS)
	$(CC) -shared -o lib/lib$@.so lib/$@.o $(ROOTLIBS) $(LINKFLAGS) -lSelection -lObjectDefinition -lCut -lPlotMacro1D

Test:
	@echo "[Compiling: $@]"
	$(CC) src/$@.cxx -o bin/$@.exe $(CXXFLAGS) $(LINKFLAGS) -lRange -lCut -lPlotMacro1D -lAnalysis -lObjectDefinition -lSelection


clean:
	@rm -f $(LIBPATH)/*.o
	@rm -f $(LIBPATH)/*.so
	@rm -f $(SRCPATH)/*~
	@rm -f $(INCPATH)/*~
	@rm -f $(OUTPATH)/output_*
	@rm -f ./*~