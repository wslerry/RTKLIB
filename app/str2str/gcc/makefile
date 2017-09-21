# makefile for str2str

BINDIR = /usr/local/bin
SRC    = ../../../src

OPTIONS = -DENAGLO -DENAGAL -DENAQZS -DENACMP -DENAIRN -DTRACE -DNFREQ=3 -DNEXOBS=3 -DSVR_REUSEADDR
CFLAGS_CMN  = -Wall -ansi -pedantic -Wno-unused-but-set-variable -I$(SRC) $(OPTIONS) -g
LDLIBS  = -lm -lrt -lpthread

# for beagleboard
#CTARGET= -mfpu=neon -mfloat-abi=softfp -ffast-math

OBJS = str2str.o stream.o rtkcmn.o erb.o solution.o sbas.o geoid.o \
    rcvraw.o novatel.o ublox.o ss2.o crescent.o skytraq.o gw10.o javad.o \
    nvs.o binex.o rt17.o tersus.o rtcm.o rtcm2.o rtcm3.o rtcm3e.o preceph.o \
    streamsvr.o septentrio.o cmr.o

.DEFAULT_GOAL := release
.PHONY: mkdir

RELOPTS    = -O3 -DNDEBUG
PRERELOPTS = -O3
DBGOPTS    = -O0

release: CFLAGS  = $(CFLAGS_CMN) $(RELOPTS)
release: EXEC    = str2str
release: OBJ_DIR = ./release
release: build

prerelease: CFLAGS  = $(CFLAGS_CMN) $(PRERELOPTS)
prerelease: EXEC    = str2str_prerel
prerelease: OBJ_DIR = ./prerelease
prerelease: build

debug: CFLAGS  = $(CFLAGS_CMN) $(DBGOPTS)
debug: EXEC    = str2str_dbg
debug: OBJ_DIR = ./debug
debug: build

build: mkdir $(OBJS)
	$(CC) -o $(EXEC) $(OBJS:%.o=$(OBJ_DIR)/%.o) $(LDLIBS)

mkdir: 
	mkdir -p $(OBJ_DIR)

str2str.o  : ../str2str.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) ../str2str.c
stream.o   : $(SRC)/stream.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/stream.c
streamsvr.o: $(SRC)/streamsvr.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/streamsvr.c
rtkcmn.o   : $(SRC)/rtkcmn.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtkcmn.c
erb.o      : $(SRC)/erb.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/erb.c
solution.o : $(SRC)/solution.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/solution.c
sbas.o     : $(SRC)/sbas.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/sbas.c
geoid.o    : $(SRC)/geoid.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/geoid.c
rcvraw.o   : $(SRC)/rcvraw.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcvraw.c
novatel.o  : $(SRC)/rcv/novatel.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/novatel.c
ublox.o    : $(SRC)/rcv/ublox.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/ublox.c
ss2.o      : $(SRC)/rcv/ss2.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/ss2.c
crescent.o : $(SRC)/rcv/crescent.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/crescent.c
skytraq.o  : $(SRC)/rcv/skytraq.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/skytraq.c
gw10.o     : $(SRC)/rcv/gw10.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/gw10.c
javad.o    : $(SRC)/rcv/javad.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/javad.c
nvs.o      : $(SRC)/rcv/nvs.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/nvs.c
binex.o    : $(SRC)/rcv/binex.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/binex.c
rt17.o     : $(SRC)/rcv/rt17.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/rt17.c
tersus.o   : $(SRC)/rcv/tersus.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/tersus.c
rtcm.o     : $(SRC)/rtcm.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm.c
rtcm2.o    : $(SRC)/rtcm2.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm2.c
rtcm3.o    : $(SRC)/rtcm3.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm3.c
rtcm3e.o   : $(SRC)/rtcm3e.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm3e.c
preceph.o  : $(SRC)/preceph.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/preceph.c
septentrio.o: $(SRC)/rcv/septentrio.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/septentrio.c
cmr.o      : $(SRC)/rcv/cmr.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/cmr.c

str2str.o  : $(SRC)/rtklib.h
stream.o   : $(SRC)/rtklib.h
streamsvr.o: $(SRC)/rtklib.h
rtkcmn.o   : $(SRC)/rtklib.h
erb.o      : $(SRC)/rtklib.h
solution.o : $(SRC)/rtklib.h
sbas.o     : $(SRC)/rtklib.h
geoid.o    : $(SRC)/rtklib.h
rcvraw.o   : $(SRC)/rtklib.h
novatel.o  : $(SRC)/rtklib.h
ublox.o    : $(SRC)/rtklib.h
ss2.o      : $(SRC)/rtklib.h
crescent.o : $(SRC)/rtklib.h
skytraq.o  : $(SRC)/rtklib.h
gw10.o     : $(SRC)/rtklib.h
javad.o    : $(SRC)/rtklib.h
nvs.o      : $(SRC)/rtklib.h
binex.o    : $(SRC)/rtklib.h
rt17.o     : $(SRC)/rtklib.h
tersus.o   : $(SRC)/rtklib.h
rtcm.o     : $(SRC)/rtklib.h
rtcm2.o    : $(SRC)/rtklib.h
rtcm3.o    : $(SRC)/rtklib.h
rtcm3e.o   : $(SRC)/rtklib.h
preceph.o  : $(SRC)/rtklib.h
septentrio.o: $(SRC)/rtklib.h
cmr.o      : $(SRC)/rtklib.h

install:
	cp str2str $(BINDIR)

#DEVICE=ttyACM0
DEVICE=ttyS1:115200:8:n:1:off
#DEVICE=ttyUSB0:115200:8:n:1:off

test1:
	./$(EXEC) -in serial://$(DEVICE) -out file://test1.out -t 2

test2:
	./$(EXEC) -in serial://$(DEVICE) -out tcpsvr://:2102 -c ../../../data/oem4_raw_10hz.cmd -t 5

test3:
	./$(EXEC) -in serial://$(DEVICE) -out ntrips://:sesam@localhost:8000/BUCU0 -c ../../../data/oem4_raw_10hz.cmd -t 5

test4:
	./$(EXEC) -in ../oem6_20121107.gps#nov -out oem6_20121107.rtcm3#rtcm3 -sta 123 -msg "1077(10),1087(10),1097(10),1107(10),1117(10),1019,1020"

test5:
	./$(EXEC) -in ../oem6_20121107.gps#nov -out oem6_20121107_gal.rtcm3#rtcm3 -msg "1097(10)"

test6:
	./$(EXEC) -in serial://ttyS54:115200#nov -out tcpsvr://:2103#rtcm3 -c ../../../data/oem4_raw_10hz.cmd

clean:
	rm -f str2str str2str_prerel str2str_dbg *.o *.out *.trace
	rm -rf ./release ./prerelease ./debug

