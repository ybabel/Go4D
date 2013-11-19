#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I../Go4D -I../Go4D/Tools -I../Go4D/Special -I../Go4D/Platform -I../Go4D/Graph2D -I../Go4D/Graph3D -I/home/yba/go4d/Beast/
CFLAGS = -Dusefixed -DSDL
RESINC = 
LIBDIR = 
LIB = ../Go4D/libGo4D.a -lSDL
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g -O0
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = Debug
DEP_DEBUG = 
OUT_DEBUG = Debug/Beast.exe

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS)
OBJDIR_RELEASE = Release
DEP_RELEASE = 
OUT_RELEASE = Release/Beast.exe

OBJ_DEBUG = $(OBJDIR_DEBUG)/BeastLand.o $(OBJDIR_DEBUG)/BeastMain.o $(OBJDIR_DEBUG)/DNA.o $(OBJDIR_DEBUG)/Drawable.o $(OBJDIR_DEBUG)/RandomBeast.o $(OBJDIR_DEBUG)/WBeasts.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/BeastLand.o $(OBJDIR_RELEASE)/BeastMain.o $(OBJDIR_RELEASE)/DNA.o $(OBJDIR_RELEASE)/Drawable.o $(OBJDIR_RELEASE)/RandomBeast.o $(OBJDIR_RELEASE)/WBeasts.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d Debug || mkdir -p Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/BeastLand.o: BeastLand.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c BeastLand.cpp -o $(OBJDIR_DEBUG)/BeastLand.o

$(OBJDIR_DEBUG)/BeastMain.o: BeastMain.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c BeastMain.cpp -o $(OBJDIR_DEBUG)/BeastMain.o

$(OBJDIR_DEBUG)/DNA.o: DNA.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c DNA.cpp -o $(OBJDIR_DEBUG)/DNA.o

$(OBJDIR_DEBUG)/Drawable.o: Drawable.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c Drawable.cpp -o $(OBJDIR_DEBUG)/Drawable.o

$(OBJDIR_DEBUG)/RandomBeast.o: RandomBeast.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c RandomBeast.cpp -o $(OBJDIR_DEBUG)/RandomBeast.o

$(OBJDIR_DEBUG)/WBeasts.o: WBeasts.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c WBeasts.cpp -o $(OBJDIR_DEBUG)/WBeasts.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf Debug
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d Release || mkdir -p Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/BeastLand.o: BeastLand.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c BeastLand.cpp -o $(OBJDIR_RELEASE)/BeastLand.o

$(OBJDIR_RELEASE)/BeastMain.o: BeastMain.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c BeastMain.cpp -o $(OBJDIR_RELEASE)/BeastMain.o

$(OBJDIR_RELEASE)/DNA.o: DNA.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c DNA.cpp -o $(OBJDIR_RELEASE)/DNA.o

$(OBJDIR_RELEASE)/Drawable.o: Drawable.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Drawable.cpp -o $(OBJDIR_RELEASE)/Drawable.o

$(OBJDIR_RELEASE)/RandomBeast.o: RandomBeast.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c RandomBeast.cpp -o $(OBJDIR_RELEASE)/RandomBeast.o

$(OBJDIR_RELEASE)/WBeasts.o: WBeasts.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c WBeasts.cpp -o $(OBJDIR_RELEASE)/WBeasts.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf Release
	rm -rf $(OBJDIR_RELEASE)

virtual_all: debug release

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

