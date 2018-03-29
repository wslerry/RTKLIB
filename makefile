DESTDIR	= /usr/local

APPS = rtkrcv rnx2rtkp str2str convbin pos2kml
# source files

SRC_DIR_1	 = src
SRC_DIR_2	 = src/rcv
SRC_DIR_APPS = $(addprefix app/,$(APPS))
SRC_DIRS	 = $(SRC_DIR_1) $(SRC_DIR_2) $(SRC_DIR_APPS)
IERS_DIR	 = lib/iers
vpath %.c $(SRC_DIRS)

SRC_NAMES_1  = $(notdir $(wildcard $(SRC_DIR_1)/*.c))
SRC_NAMES_2  = $(notdir $(wildcard $(SRC_DIR_2)/*.c))
SRC_NAMES_RTKRCV = rtkrcv.c vt.c
SRC_NAMES_RNX2RTKP = rnx2rtkp.c
SRC_NAMES_POS2KML = pos2kml.c
SRC_NAMES_CONVBIN = convbin.c
SRC_NAMES_STR2STR = str2str.c
SRC_NAMES    = $(SRC_NAMES_1) $(SRC_NAMES_2)

# object files
OBJ_NAMES =$(patsubst %.c,%.o,$(SRC_NAMES))

# common compile options
INCLUDEDIR := -I$(SRC_DIR_1)
OPTIONS	   = -DTRACE -DENAGLO -DENAQZS -DENAGAL -DENACMP -DENAIRN -DNFREQ=3 -DSVR_REUSEADDR
CFLAGS_CMN = -std=c99 -pedantic -Wall -Werror -fpic -fno-strict-overflow -Wno-error=unused-but-set-variable \
					-Wno-error=unused-result $(INCLUDEDIR) $(OPTIONS)
LDLIBS	   = lib/iers/gcc/iers.a -lm -lrt -lpthread
SONAME 	   = 2.4.3
LDFLAGS    = -shared 
TARGET_LIB = librtk.so

# target-specific options
REL_OPTS    = -O3 -DNDEBUG
PREREL_OPTS = -O3
DBG_OPTS    = -O0 -g

####################################################################
##### release / prerelease / debug targets

.DEFAULT_GOAL = all
.PHONY: all release prerelease debug mkdir install clean IERS

all: release

REL_DIR    = build/release
PREREL_DIR = build/prerelease
DBG_DIR    = build/debug

# default dirs
OUTPUT_DIR = $(REL_DIR)
DEPDIR = build/release/.d

ifneq "$(findstring release, $(MAKECMDGOALS))" ""
 DEPDIR = build/release/.d
 OUTPUT_DIR=$(REL_DIR)
endif

ifneq "$(findstring prerelease, $(MAKECMDGOALS))" ""
 DEPDIR = build/prerelease/.d
 OUTPUT_DIR=$(PREREL_DIR)
endif

ifneq "$(findstring debug, $(MAKECMDGOALS))" ""
 DEPDIR = build/debug/.d
 OUTPUT_DIR=$(DBG_DIR)
endif

$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

LIB = $(addprefix $(OUTPUT_DIR)/,$(TARGET_LIB))
OBJS = $(addprefix $(OUTPUT_DIR)/src/,$(OBJ_NAMES))


release: IERS
release: CFLAGS  = $(CFLAGS_CMN) $(REL_OPTS)
release: mkdir
release: $(LIB) | $(APPS)


prerelease: IERS
prerelease: CFLAGS  = $(CFLAGS_CMN) $(PREREL_OPTS)
prerelease: mkdir
prerelease: $(LIB) | $(APPS)

debug: IERS
debug: CFLAGS  = $(CFLAGS_CMN) $(DBG_OPTS)
debug: mkdir
debug: |$(LIB) $(APPS)

####################################################################
IERS:
	@$(MAKE) -C $(IERS_DIR)/gcc
# release lib
$(LIB):  $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(OUTPUT_DIR)/src/%.o: %.c  $(DEPDIR)/%.d
	$(CC) $(DEPFLAGS) -c $(CFLAGS) -fpic $< -o $@
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d
####################################################################

# apps
rtkrcv: $(addprefix $(OUTPUT_DIR)/app/, $(SRC_NAMES_RTKRCV:%.c=%.o)) | $(LIB)
	$(CC) $^ -o $(OUTPUT_DIR)/$@ $(LDLIBS)  -L$(OUTPUT_DIR) -lrtk

rnx2rtkp: $(addprefix $(OUTPUT_DIR)/app/, $(SRC_NAMES_RNX2RTKP:%.c=%.o)) | $(LIB)
	$(CC) $^ -o $(OUTPUT_DIR)/$@ $(LDLIBS) -L$(abspath $(OUTPUT_DIR)) -lrtk

pos2kml: $(addprefix $(OUTPUT_DIR)/app/, $(SRC_NAMES_POS2KML:%.c=%.o))  | $(LIB)
	$(CC) $^ -o $(OUTPUT_DIR)/$@ $(LDLIBS) -L$(OUTPUT_DIR) -lrtk

convbin: $(addprefix $(OUTPUT_DIR)/app/, $(SRC_NAMES_CONVBIN:%.c=%.o)) | $(LIB)
	$(CC) $^ -o $(OUTPUT_DIR)/$@ $(LDLIBS) -L$(OUTPUT_DIR) -lrtk

str2str: $(addprefix $(OUTPUT_DIR)/app/, $(SRC_NAMES_STR2STR:%.c=%.o)) | $(LIB)
	$(CC) $^ -o $(OUTPUT_DIR)/$@ $(LDLIBS) -L$(OUTPUT_DIR) -lrtk

$(OUTPUT_DIR)/app/%.o: %.c $(DEPDIR)/%.d
	$(CC) $(DEPFLAGS) -c $(CFLAGS) $< -o $@
	$(POSTCOMPILE)
####################################################################
mkdir:
	mkdir -p $(addsuffix /src, $(OUTPUT_DIR)) \
			  $(addsuffix /app, $(OUTPUT_DIR))
install:
	cp $(LIB) $(addprefix $(DESTDIR), /lib)
	cp $(addprefix $(REL_DIR)/, $(APPS)) $(addprefix $(DESTDIR), /bin)


clean:
	rm -rf build/
	@$(MAKE) -C $(IERS_DIR)/gcc clean

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRC_NAMES))))