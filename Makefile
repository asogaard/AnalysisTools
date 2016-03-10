# Variables.
CXX = clang++
PACKAGENAME = AnalysisTools

# Root variables
ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS   := $(shell root-config --libs)
ROOTGLIBS  := $(shell root-config --glibs)

# Directories
INCDIR = ./
SRCDIR = ./src
OBJDIR = ./build
LIBDIR = ./lib
EXEDIR = ./bin
EXTDIR = ./$(PACKAGENAME)/External

# Extensions
SRCEXT = cxx

# Collections
SRCS := $(shell find $(SRCDIR) -name '*.$(SRCEXT)')
OBJS := $(patsubst $(SRCDIR)/%.$(SRCEXT),$(OBJDIR)/%.o,$(SRCS))
EXTOBJS := $(shell find $(EXTDIR) -name '*.o')
GARBAGE = $(OBJDIR)/*.o $(EXEDIR)/* $(LIBDIR)/*.so

# Dependencies
CXXFLAGS  = --std=c++11 -I$(INCDIR) -I$(EXTDIR) $(ROOTCFLAGS) 
LINKFLAGS = -O2 -L$(LIBDIR) -L$(ROOTSYS)/lib $(ROOTLIBS) 

# Libraries
LIBS += $(ROOTLIBS)

# Targets
all: $(PACKAGENAME)

$(PACKAGENAME) : $(OBJS) 
	$(CXX) -shared -o $(LIBDIR)/lib$@.so $(OBJS) $(EXTOBJS) $(LIBS)

$(OBJDIR)/%.o : $(SRCDIR)/%.$(SRCEXT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

Test: $(OBJS)
	@echo "[Compiling: $@]"
	$(CXX) src/$@.cxx -o bin/$@.exe $(CXXFLAGS) $(LINKFLAGS) -lAnalysisTools
 

clean : 
	@rm -f $(GARBAGE)

