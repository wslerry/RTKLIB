# makefile for rnx2rtkp

BINDIR  = /usr/local/bin
SRC     = ../../../src

OPTIONS    = -DTRACE -DENAGLO -DENAQZS -DENAGAL -DENACMP -DENAIRN -DNFREQ=3 -DIERS_MODEL
# for no lapack
CFLAGS_CMN = -Wall -fno-strict-overflow -ansi -pedantic -Wno-unused-but-set-variable -I$(SRC) $(OPTIONS) -g
LDLIBS  = ../../../lib/iers/gcc/iers.a -lgfortran -lm -lrt -lpthread

# for gprof
#CFLAGS  = -Wall -O3 -ansi -pedantic -Wno-unused-but-set-variable -I$(SRC) -DLAPACK $(OPTS) -pg
#LDLIBS  = -lm -lrt -llapack -lblas -pg

# for mkl
##MKLDIR  = /opt/intel/mkl
#MKLDIR  = /proj/madoca/lib/mkl
#CFLAGS  = -O3 -ansi -pedantic -Wno-unused-but-set-variable -I$(SRC) $(OPTS) -DMKL
#LDLIBS  = -L$(MKLDIR)/intel64 -lmkl_intel_lp64 -lmkl_core -lmkl_gnu_thread -lpthread -lgomp -lm -lrt

OBJS = rnx2rtkp.o rtkcmn.o rinex.o rtkpos.o postpos.o erb.o solution.o \
    stream.o rcvraw.o novatel.o ss2.o ublox.o crescent.o skytraq.o gw10.o \
    javad.o nvs.o binex.o rt17.o tersus.o septentrio.o cmr.o \
    lambda.o geoid.o sbas.o preceph.o pntpos.o ephemeris.o options.o \
    ppp.o ppp_ar.o ppp_corr.o rtcm.o rtcm2.o rtcm3.o rtcm3e.o ionex.o tides.o qzslex.o \
    
.DEFAULT_GOAL := release
.PHONY: mkdir

RELOPTS    = -O3 -DNDEBUG
PRERELOPTS = -O3
DBGOPTS    = -O0

all:     release

release: CFLAGS  = $(CFLAGS_CMN) $(RELOPTS)
release: EXEC    = rnx2rtkp
release: OBJ_DIR = ./release
release: build

prerelease: CFLAGS  = $(CFLAGS_CMN) $(PRERELOPTS)
prerelease: EXEC    = rnx2rtkp_prerel
prerelease: OBJ_DIR = ./prerelease
prerelease: build

debug: CFLAGS  = $(CFLAGS_CMN) $(DBGOPTS)
debug: EXEC    = rnx2rtkp_dbg
debug: OBJ_DIR = ./debug
debug: build

build: mkdir $(OBJS)
	$(CC) -o $(EXEC) $(OBJS:%.o=$(OBJ_DIR)/%.o) $(LDLIBS)

mkdir: 
	mkdir -p $(OBJ_DIR)

rnx2rtkp.o : ../rnx2rtkp.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) ../rnx2rtkp.c
rtkcmn.o   : $(SRC)/rtkcmn.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtkcmn.c
rinex.o    : $(SRC)/rinex.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rinex.c
rtkpos.o   : $(SRC)/rtkpos.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtkpos.c
postpos.o  : $(SRC)/postpos.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/postpos.c
erb.o      : $(SRC)/erb.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/erb.c
solution.o : $(SRC)/solution.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/solution.c
stream.o   : $(SRC)/stream.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/stream.c
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
tersus.o     : $(SRC)/rcv/tersus.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/tersus.c
septentrio.o: $(SRC)/rcv/septentrio.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/septentrio.c
cmr.o      : $(SRC)/rcv/cmr.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcv/cmr.c
rcvraw.o   : $(SRC)/rcvraw.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rcvraw.c
lambda.o   : $(SRC)/lambda.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/lambda.c
geoid.o    : $(SRC)/geoid.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/geoid.c
sbas.o     : $(SRC)/sbas.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/sbas.c
preceph.o  : $(SRC)/preceph.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/preceph.c
pntpos.o   : $(SRC)/pntpos.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/pntpos.c
ephemeris.o: $(SRC)/ephemeris.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/ephemeris.c
options.o  : $(SRC)/options.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/options.c
ppp.o      : $(SRC)/ppp.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/ppp.c
ppp_ar.o   : $(SRC)/ppp_ar.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/ppp_ar.c
ppp_corr.o : $(SRC)/ppp_corr.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/ppp_corr.c
rtcm.o     : $(SRC)/rtcm.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm.c
rtcm2.o    : $(SRC)/rtcm2.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm2.c
rtcm3.o    : $(SRC)/rtcm3.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm3.c
rtcm3e.o   : $(SRC)/rtcm3e.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/rtcm3e.c
ionex.o    : $(SRC)/ionex.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/ionex.c
tides.o    : $(SRC)/tides.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/tides.c
qzslex.o   : $(SRC)/qzslex.c
	$(CC) -o $(OBJ_DIR)/$@ -c $(CFLAGS) $(SRC)/qzslex.c

rnx2rtkp.o : $(SRC)/rtklib.h
rtkcmn.o   : $(SRC)/rtklib.h
rinex.o    : $(SRC)/rtklib.h
rtkpos.o   : $(SRC)/rtklib.h
postpos.o  : $(SRC)/rtklib.h
erb.o      : $(SRC)/rtklib.h
solution.o : $(SRC)/rtklib.h
lambda.o   : $(SRC)/rtklib.h
geoid.o    : $(SRC)/rtklib.h
sbas.o     : $(SRC)/rtklib.h
preceph.o  : $(SRC)/rtklib.h
pntpos.o   : $(SRC)/rtklib.h
ephemeris.o: $(SRC)/rtklib.h
options.o  : $(SRC)/rtklib.h
ppp.o      : $(SRC)/rtklib.h
ppp_ar.o   : $(SRC)/rtklib.h
rtcm.o     : $(SRC)/rtklib.h
rtcm2.o    : $(SRC)/rtklib.h
rtcm3.o    : $(SRC)/rtklib.h
rtcm3e.o   : $(SRC)/rtklib.h
ionex.o    : $(SRC)/rtklib.h
tides.o    : $(SRC)/rtklib.h
qzslex.o   : $(SRC)/rtklib.h

CMD1    = $(EXEC)
INPUT11 = ../../../test/data/rinex/07590920.05o ../../../test/data/rinex/30400920.05n
INPUT12 = ../../../test/data/rinex/30400920.05o
OPTS1   = -r -3978241.958 3382840.234 3649900.853

test : test1 test2 test3 test4 test5 test6 test7 test8 test9 test10
test : test11 test12 test13 test14 test15 test16 test17 test18 test19 test20
test : test21 test22 test23 test24

test1 :
	$(CMD1) $(INPUT11) -x 5 -o test1.pos
test2 :
	$(CMD1) -t -e $(OPTS1) $(INPUT11) > test2.pos
test3 :
	$(CMD1) -t -p 1 -e $(OPTS1) $(INPUT11) $(INPUT12) > test3.pos
test4 :
	$(CMD1) -t -p 3 -e $(OPTS1) $(INPUT11) $(INPUT12) > test4.pos
test5 :
	$(CMD1) -t -m 15 -e $(OPTS1) $(INPUT11) $(INPUT12) > test5.pos
test6 :
	$(CMD1) -t -f 1 -e $(OPTS1) $(INPUT11) $(INPUT12) > test6.pos
test7 :
	$(CMD1) -t -v 5 -e $(OPTS1) $(INPUT11) $(INPUT12) > test7.pos
test8 :
	$(CMD1) -t -i -e $(OPTS1) $(INPUT11) $(INPUT12) > test8.pos
test9 :
	$(CMD1) -t -p 0 $(OPTS1) $(INPUT11) > test9.pos
test10 :
	$(CMD1) -t -p 0 $(OPTS1) $(INPUT11) -o test10.pos
test11 :
	$(CMD1) -t -p 0 -n $(OPTS1) $(INPUT11) > test11.pos
test12 :
	$(CMD1) -t -p 0 -g $(OPTS1) $(INPUT11) > test12.pos
test13 :
	$(CMD1) -t -p 0 $(OPTS1) $(INPUT11) > test13.pos
test14 :
	$(CMD1) -t -p 0 -u $(OPTS1) $(INPUT11) > test14.pos
test15 :
	$(CMD1) -t -p 0 -d 9 $(OPTS1) $(INPUT11) > test15.pos
test16 :
	$(CMD1) -t -p 0 -s , $(OPTS1) $(INPUT11) > test16.pos
test17 :
	$(CMD1) -t -b -e $(OPTS1) $(INPUT11) $(INPUT12) > test17.pos
test18 :
	$(CMD1) -t -c -e $(OPTS1) $(INPUT11) $(INPUT12) > test18.pos
test19 :
	$(CMD1) -t -h -e $(OPTS1) $(INPUT11) $(INPUT12) > test19.pos
test20 :
	$(CMD1) -t -p 4 -a $(OPTS1) $(INPUT11) $(INPUT12) > test20.pos
test21 :
	$(CMD1) $(INPUT11) $(INPUT12) > test21.pos
test22 :
	$(CMD1) -k opts1.conf $(INPUT11) $(INPUT12) > test22.pos
test23 :
	$(CMD1) -k opts2.conf $(INPUT11) $(INPUT12) > test23.pos
test24 :
	$(CMD1) -k opts3.conf $(INPUT11) $(INPUT12) -y 2 -o test24.pos
test25 :
	$(CMD1) -k opts4.conf $(INPUT11) $(INPUT12) -y 2 -o test25.pos

clean :
	rm -f rnx2rtkp rnx2rtkp_prerel rnx2rtkp_dbg *.o *.pos *.trace
	rm -rf ./release ./prerelease ./debug

prof :
	gprof rnx2rtkp > prof.txt

install :
	cp rnx2rtkp $(BINDIR)

