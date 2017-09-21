# makefile for pos2kml

BINDIR = /usr/local/bin
SRC    = ../../../src
CFLAGS_CMN = -Wall -ansi -pedantic -I$(SRC) -DTRACE
LDLIBS  = -lm -lrt

OBJS = pos2kml.o convkml.o erb.o convgpx.o solution.o geoid.o rtkcmn.o preceph.o
    
.DEFAULT_GOAL := release
.PHONY: mkdir

RELOPTS    = -O3 -DNDEBUG
PRERELOPTS = -O3
DBGOPTS    = -O0

release: CFLAGS  = $(CFLAGS_CMN) $(RELOPTS)
release: EXEC    = pos2kml
release: OBJ_DIR = ./release
release: build

prerelease: CFLAGS  = $(CFLAGS_CMN) $(PRERELOPTS)
prerelease: EXEC    = pos2kml_prerel
prerelease: OBJ_DIR = ./prerelease
prerelease: build

debug: CFLAGS  = $(CFLAGS_CMN) $(DBGOPTS)
debug: EXEC    = pos2kml_dbg
debug: OBJ_DIR = ./debug
debug: build

build: mkdir $(OBJS)
	$(CC) -o $(EXEC) $(OBJS:%.o=$(OBJ_DIR)/%.o) $(LDLIBS)

mkdir: 
	mkdir -p $(OBJ_DIR)

pos2kml.o  : ../pos2kml.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) ../pos2kml.c
convkml.o  : $(SRC)/convkml.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/convkml.c
erb.o      : $(SRC)/erb.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/erb.c
convgpx.o  : $(SRC)/convgpx.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/convgpx.c
solution.o : $(SRC)/solution.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/solution.c
geoid.o    : $(SRC)/geoid.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/geoid.c
rtkcmn.o   : $(SRC)/rtkcmn.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtkcmn.c
preceph.o  : $(SRC)/preceph.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/preceph.c

pos2kml.o  : $(SRC)/rtklib.h
convkml.o  : $(SRC)/rtklib.h
erb.o      : $(SRC)/rtklib.h
convgpx.o  : $(SRC)/rtklib.h
solution.o : $(SRC)/rtklib.h
geoid.o    : $(SRC)/rtklib.h
rtkcmn.o   : $(SRC)/rtklib.h
preceph.o  : $(SRC)/rtklib.h

install:
	cp pos2kml $(BINDIR)

clean:
	rm -f pos2kml pos2kml_prerel pos2kml_dbg *.o
	rm -rf ./release ./prerelease ./debug
