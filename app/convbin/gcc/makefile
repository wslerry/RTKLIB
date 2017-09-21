#
# makefile for convbin
#

BINDIR = /usr/local/bin

SRC    = ../../../src

INCLUDE = -I$(SRC)
OPTIONS = -DTRACE -DENAGLO -DENAQZS -DENAGAL -DENACMP -DENAIRN -DNFREQ=7 -DNEXOBS=3
CFLAGS_CMN  = -ansi -pedantic -Wall -Wno-unused-but-set-variable $(INCLUDE) $(OPTIONS) -g
LDLIBS = -lm -lrt -lpthread

OBJS = convbin.o stream.o rtkcmn.o rinex.o sbas.o preceph.o rcvraw.o convrnx.o \
    rtcm.o rtcm2.o rtcm3.o rtcm3e.o pntpos.o ephemeris.o ionex.o \
    novatel.o ss2.o ublox.o crescent.o skytraq.o gw10.o javad.o nvs.o \
    binex.o rt17.o tersus.o qzslex.o septentrio.o cmr.o
    
.DEFAULT_GOAL := release
.PHONY: mkdir

RELOPTS    = -O3 -DNDEBUG
PRERELOPTS = -O3
DBGOPTS    = -O0

release: CFLAGS  = $(CFLAGS_CMN) $(RELOPTS)
release: EXEC    = convbin
release: OBJ_DIR = ./release
release: build

prerelease: CFLAGS  = $(CFLAGS_CMN) $(PRERELOPTS)
prerelease: EXEC    = convbin_prerel
prerelease: OBJ_DIR = ./prerelease
prerelease: build

debug: CFLAGS  = $(CFLAGS_CMN) $(DBGOPTS)
debug: EXEC    = convbin_dbg
debug: OBJ_DIR = ./debug
debug: build

build: mkdir $(OBJS)
	$(CC) -o $(EXEC) $(OBJS:%.o=$(OBJ_DIR)/%.o) $(LDLIBS)

mkdir: 
	mkdir -p $(OBJ_DIR)

convbin.o  : ../convbin.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) ../convbin.c
stream.o   : $(SRC)/stream.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/stream.c
rtkcmn.o   : $(SRC)/rtkcmn.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtkcmn.c
rinex.o    : $(SRC)/rinex.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rinex.c
sbas.o     : $(SRC)/sbas.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/sbas.c
preceph.o  : $(SRC)/preceph.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/preceph.c
rcvraw.o   : $(SRC)/rcvraw.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcvraw.c
convrnx.o  : $(SRC)/convrnx.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/convrnx.c
rtcm.o     : $(SRC)/rtcm.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm.c
rtcm2.o    : $(SRC)/rtcm2.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm2.c
rtcm3.o    : $(SRC)/rtcm3.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm3.c
rtcm3e.o   : $(SRC)/rtcm3e.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm3e.c
pntpos.o   : $(SRC)/pntpos.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/pntpos.c
ionex.o    : $(SRC)/ionex.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/ionex.c
ephemeris.o: $(SRC)/ephemeris.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/ephemeris.c
novatel.o  : $(SRC)/rcv/novatel.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/novatel.c
ss2.o      : $(SRC)/rcv/ss2.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/ss2.c
ublox.o    : $(SRC)/rcv/ublox.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/ublox.c
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
qzslex.o   : $(SRC)/qzslex.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/qzslex.c
septentrio.o: $(SRC)/rcv/septentrio.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/septentrio.c
cmr.o      : $(SRC)/rcv/cmr.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/cmr.c

convbin.o  : $(SRC)/rtklib.h
stream.o   : $(SRC)/rtklib.h
rtkcmn.o   : $(SRC)/rtklib.h
rinex.o    : $(SRC)/rtklib.h
sbas.o     : $(SRC)/rtklib.h
preceph.o  : $(SRC)/rtklib.h
rcvraw.o   : $(SRC)/rtklib.h
convrnx.o  : $(SRC)/rtklib.h
rtcm.o     : $(SRC)/rtklib.h
rtcm2.o    : $(SRC)/rtklib.h
rtcm3.o    : $(SRC)/rtklib.h
rtcm3e.o   : $(SRC)/rtklib.h
pntpos.o   : $(SRC)/rtklib.h
ephemeris.o: $(SRC)/rtklib.h
ionex.o    : $(SRC)/rtklib.h
novatel.o  : $(SRC)/rtklib.h
ss2.o      : $(SRC)/rtklib.h
ublox.o    : $(SRC)/rtklib.h
crescent.o : $(SRC)/rtklib.h
skytraq.o  : $(SRC)/rtklib.h
gw10.o     : $(SRC)/rtklib.h
javad.o    : $(SRC)/rtklib.h
nvs.o      : $(SRC)/rtklib.h
binex.o    : $(SRC)/rtklib.h
rt17.o     : $(SRC)/rtklib.h
tersus.o   : $(SRC)/rtklib.h
qzslex.o   : $(SRC)/rtklib.h
septentrio.o: $(SRC)/rtklib.h
cmr.o      : $(SRC)/rtklib.h

DATDIR = ../../../test/data/rcvraw

install:
	cp convbin $(BINDIR)

clean:
	rm -f convbin convbin_prerel convbin_dbg *.o *.obs *.nav *.gnav *.hnav *.qnav *.sbs *.stackdump
	rm -rf ./release ./prerelease ./debug

test : test1 test2 test3 test4 test5 test7 test8 test9 test10 test11 test12 test13
test : test14 test15

test1:
	./$(EXEC) -r nov $(DATDIR)/oemv_200911218.gps -ti 10 -d . -os
test2:
	./$(EXEC) -r hemis $(DATDIR)/cres_20080526.bin -ti 10 -d . -f 1 -od -os
test3:
	./$(EXEC) $(DATDIR)/ubx_20080526.ubx -o ubx_test.obs -d . -f 1 -ts 2008/5/26 6:00 -te 2008/5/26 6:10
test4:
	./$(EXEC) $(DATDIR)/ubx_20080526.ubx -n ubx_test.nav -d .
test5:
	./$(EXEC) $(DATDIR)/ubx_20080526.ubx -h ubx_test.hnav -s ubx_test.sbs -d . -x 129
test7:
	./$(EXEC) $(DATDIR)/testglo.rtcm2 -tr 2009/12/18 23:20 -d .
test8:
	./$(EXEC) $(DATDIR)/testglo.rtcm3 -os -tr 2009/12/18 23:20 -d .
test9:
	./$(EXEC) -v 3 -f 6 -r nov $(DATDIR)/oemv_200911218.gps -od -os -o rnx3_test.obs -n rnx3_test.nav -d .
test10:
	./$(EXEC) $(DATDIR)/testglo.rtcm3 -os -tr 2009/12/18 23:20 -d .
test11:
	./$(EXEC) $(DATDIR)/javad_20110115.jps -d out -c JAV1
test12:
	./$(EXEC) $(DATDIR)/javad_20110115.jps -d out -v 3.00 -f 3 -od -os
test13:
	./$(EXEC) $(DATDIR)/javad_20110115.jps -d out -o test13.obs -v 3 -hc test1 -hc test2 -hm MARKER -hn MARKERNO -ht MARKKERTYPE -ho OBSERVER/AGENCY -hr 1234/RECEIVER/V.0.1.2 -ha ANTNO/ANTENNA -hp 1234.567/8901.234/5678.901 -hd 0.123/0.234/0.567
test14:
	./$(EXEC) $(DATDIR)/javad_20110115.jps -d out -o test14.obs -v 3 -y S -y J -x 2 -x R19 -x R21
test15:
	./$(EXEC) $(DATDIR)/javad_20110115.jps -d out -o test15.obs -v 3 -ro "-GL1P -GL2C"
test16:
	./$(EXEC) $(DATDIR)/javad_20110115.jps -d out -o test15.obs -v 3 -ro "-GL1P -GL2C"
test17:
	./$(EXEC) $(DATDIR)/GMSD7_20121014.rtcm3 -tr 2012/10/14 0:00:00
test18:
	./$(EXEC) $(DATDIR)/GMSD7_20121014.rtcm3 -scan -v 3.01 -f 6 -od -os -tr 2012/10/14 0:00:00
test21:
	stty raw < /dev/ttyACM0
	./$(EXEC) -r ubx -o ubx.obs -n ubx.nav -s ubx.sbs -h ubx.hnav /dev/ttyACM0


