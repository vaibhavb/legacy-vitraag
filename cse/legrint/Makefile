# Vaibhav (vaibhav@cse.ucsc.edu)

PROGRAMS=explorer.lx
# extra dynamic sources
DOBJECTS=repos.o motion.o bump_recovery.o scan_sensors.o annotations.o telemetry.o metro_sound.o

################
LEGOS_ROOT=/projects/bubble/code/lego/legOS-0.2.4/legOS
BOOT_DIR = $(LEGOS_ROOT)/boot #??
KERNEL=$(LEGOS_ROOT)/boot/legOS

LIBS=-lc -lmint -lfloat -lc++
DYNAMIC_LDS =$(KERNEL).lds
DLDFLAGS    =-T $(DYNAMIC_LDS) -L$(LEGOS_ROOT)/lib

TOOLPREFIX=/usr/bin/h8300-hitachi-hms-
COPT  =-O2 -fno-builtin -fomit-frame-pointer
CWARN =-Wall
CINC  =-I$(LEGOS_ROOT)/include -I$(LEGOS_ROOT)/include/lnp -I.
CINC += -I$(BOOT_DIR) #??why is config.h doing in boot??
CFLAGS=$(COPT) $(CWARN) $(CINC) 

# how to disassemble
ODFLAGS = --disassemble-all --no-show-raw-insn -m h8300

# which firmware version to act upon
BASE1=0xb000
BASE2=0xb210
FIRMVERS=firm0309

# The flesh

AS=$(TOOLPREFIX)as
AR=$(TOOLPREFIX)ar
LD=$(TOOLPREFIX)ld
NM=$(TOOLPREFIX)nm
OBJCOPY=$(TOOLPREFIX)objcopy
OBJDUMP=$(TOOLPREFIX)objdump
CC=$(TOOLPREFIX)gcc

MERGEMAP=$(LEGOS_ROOT)/util/merge-map 
GENLDS 	=$(LEGOS_ROOT)/util/genlds
FIXDEPS	=$(LEGOS_ROOT)/util/fixdeps
MAKELX	=$(LEGOS_ROOT)/util/makelx
MAKEDEPEND=$(CC) -M $(CINC)

#include $(LEGOS_ROOT)/Makefile.user
################

all:  $(DOBJECTS) $(PROGRAMS)

# doc/html subdirectory
html:
	doxygen Doxyfile

tag:
	ctags --format=1 *.c include/*.h include/*/*.h *.c

clean:
	rm -rf .depend* *.o *.dis *.dis2 *~ *.bak *.tgz *.s tags *.ds1 *.ds2 *.dmap *.dcoff *.srec *.map *.coff

realclean: clean
	rm -f *.lx

#
# Compilation Rules
#

%.lx: %.ds1 %.ds2
	$(MAKELX) $*.ds1 $*.ds2 $*.lx

# how to make srec files of dynamic executables
%.ds1: %.o 
	$(LD) $(DLDFLAGS) $*.o $(DOBJECTS) $(LIBS) -o $*.ds1 -Ttext $(BASE1)

# how to make srec files of dynamic executables
%.ds2: %.o 
	$(LD) $(DLDFLAGS) $*.o $(DOBJECTS) $(LIBS) -o $*.ds2 -Ttext $(BASE2)

%.o: %.s
	$(AS) $*.s -o $*.o

# how to compile C source
%.o: %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o



# DO NOT DELETE

#
#dependency generation
#

.depend:
	$(MAKEDEPEND) *.c > .depend

ifndef NODEPS
include .depend
endif














