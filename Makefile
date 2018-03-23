# vim: noexpandtab:list:
MKVERSION = 3.0

PROJECT ?= planter
VERSION ?= 1.0

# Determine operating system environment.
# Possible values are (tested): Linux, FreeBSD (on 8.1), ...
OSNAME      = $(shell uname)
ARCH       ?= avr
PROGRAMMER ?= jtag2isp
PORT       ?= /dev/arduino-uno-*

# Arduino's arduino/avr/boards.txt file is useful for these values
UPLOAD_RATE ?= 115200
MCU         ?= atmega328p
F_CPU       ?= 16000000L

BACKUPDIR   = backup
MEMORYTYPES = flash eeprom lfuse hfuse efuse calibration lock signature

# User libraries
USER_LIBDIR ?= ./libraries
USER_LIBS   ?=

# Additional pre-compiled libraries to link with.
# Always leave the math (m) library last!
LDLIBS ?= -lsangster_atmega328p
LDLIBS += -lm

LISTING_ARGS  = -h -S
LISTING_ARGS += -t -l -C -w

SYMBOL_ARGS  = -n
SYMBOL_ARGS += -C

SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

# Where are tools like avr-gcc located on your system?
AVR_TOOLS_PATH ?=


############################################################################
# End of basic config
############################################################################
CDEFS ?=

# Output hex format.
HEXFORMAT = ihex

# Name of the tar file in which to pack the user program up in.
TARFILE = $(PROJECT)-$(VERSION).tar

RESETCMD ?= stty

# Try and guess PORT if it wasn't set previously.
# Note using shell globs most likely won't work, so try first port.
ifeq "$(OSNAME)" "Linux"
    PORT ?= /dev/ttyUSB0
else
    PORT ?= /dev/cuaU0 # Not Linux, so try BSD port name
endif


### Sources

# User program sources.
SRC  = $(wildcard $(SRCDIR)/*.c)
ASRC = $(wildcard $(SRCDIR)/*.S)

# Paths to check for source files (pre-requisites).
vpath % .


### Include directories.
CINCS = -I$(SRCDIR)


### Object and dependencies files.

# User program.
# OBJECTS = $(addprefix $(BINDIR)/,$(notdir $(SRC:.c=.c.o) $(ASRC:.S=.S.o)))
OBJECTS = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.c.o) \
          $(ASRC:$(SRCDIR)/%.S=$(OBJDIR)/%.S.o)

# All dependencies files.
ALLDEPS = $(OBJECTS:%.o=%.d)


### More macro definitions.
CDEFS += -DF_CPU=$(F_CPU)  # -DF_CPU is mandatory.


### C Compiler flags.

# C standard level.
# c89   - ISO C90 ("ANSI" C)
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions (default for C)
CSTANDARD = -std=gnu99

# Optimisations.
OPT_OPTIMS  = -Os
OPT_OPTIMS += -ffunction-sections -fdata-sections
OPT_OPTIMS += -mrelax
# -mrelax crashes binutils 2.22, 2.19.1 gives 878 byte shorter program.
# The crash with binutils 2.22 needs a patch. See sourceware #12161.
ifdef LTO
    OPT_OPTIMS += -flto
    #OPT_OPTIMS += -flto-report
    #OPT_OPTIMS += -fwhole-program
    # -fuse-linker-plugin requires gcc be compiled with --enable-gold, and requires
    # the gold linker to be available (GNU ld 2.21+ ?).
    #OPT_OPTIMS += -fuse-linker-plugin
endif

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
OPT_DEBUG = -g2 -gstabs

ifndef OPT_WARN
    OPT_WARN  = -Wall
    OPT_WARN += -pedantic
    OPT_WARN += -Wextra
    OPT_WARN += -Wmissing-declarations
    OPT_WARN += -Wmissing-field-initializers
    OPT_WARN += -Wsystem-headers
    OPT_WARN += -Wno-variadic-macros
endif
ifndef OPT_WARN_C
    OPT_WARN_C  = $(OPT_WARN)
    OPT_WARN_C += -Wmissing-prototypes
endif

# Other.
ifndef OPT_OTHER
    OPT_OTHER =
    # OPT_OTHER += -save-temps  # Save gcc temp files (pre-processor, assembler)
    OPT_CPP_OTHER = -fno-use-cxa-atexit
endif

# Final combined.
CFLAGS = -mmcu=$(MCU) \
         $(OPT_OPTIMS) $(OPT_DEBUG) $(CSTANDARD) $(CDEFS) \
         $(OPT_WARN) $(OPT_OTHER) $(CEXTRA)


### Assembler flags.

#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
ASTANDARD = -x assembler-with-cpp # Assembler standard level.

# Final combined.
ASFLAGS = -mmcu=$(MCU) $(CDEFS) $(ASTANDARD) $(ASEXTRA)


### Linker flags.

# Optimisation setting must match compiler's, esp. for -flto.

LDFLAGS  = -mmcu=$(MCU)
LDFLAGS += $(OPT_OPTIMS)
LDFLAGS += -Wl,--gc-sections
#LDFLAGS += -Wl,--print-gc-sections


### Programming / program uploading.

AVRDUDE_FLAGS  =
AVRDUDE_FLAGS += -q  # Quiet -q -qq / Verbose -v -vv.
# AVRDUDE_FLAGS += -V  # Do not verify.
# AVRDUDE_FLAGS += -F  # Override invalid signature check.

ifeq "$(PROGRAMMER)" "jtag2isp"
    AVRDUDE_FLAGS += -p $(MCU) -c $(PROGRAMMER)
else
    # Disable auto erase for flash memory. (IDE uses this too.)
    AVRDUDE_FLAGS += -D
    AVRDUDE_FLAGS += -p $(MCU) -c $(PROGRAMMER) -b $(UPLOAD_RATE)
    AVRDUDE_FLAGS += -P $(PORT)
    AVRDUDE_FLAGS += -C /etc/avrdude.conf # avrdude config file
endif

AVRDUDE_WRITE_FLASH  = -U flash:w:$(BINDIR)/$(PROJECT).hex:i
AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(BINDIR)/$(PROJECT).eep:i
AVRDUDE_READ_BACKUP  = -U $$memory:r:$(BACKUPDIR)/$(MCU).$$memory.hex:i


### Programs
AVRPREFIX = avr-
CC      = $(AVR_TOOLS_PATH)$(AVRPREFIX)gcc
OBJCOPY = $(AVR_TOOLS_PATH)$(AVRPREFIX)objcopy
OBJDUMP = $(AVR_TOOLS_PATH)$(AVRPREFIX)objdump
AR      = $(AVR_TOOLS_PATH)$(AVRPREFIX)ar
SIZE    = $(AVR_TOOLS_PATH)$(AVRPREFIX)size
NM      = $(AVR_TOOLS_PATH)$(AVRPREFIX)nm
AVRDUDE = $(AVR_TOOLS_PATH)avrdude
RM      = rm -f
RMDIR   = rmdir
MV      = mv -f
ifeq "$(OSNAME)" "Linux"
    STTY = stty -F $(PORT)
else
    # BSD uses small f
    STTY = stty -f $(PORT)
endif


### Implicit rules
.SUFFIXES: .ino .pde .elf .hex .eep .lss .listing .sym .symbol
.SUFFIXES: .cpp .c .S .o .a

# Compile: create object files from C source files.
%.c.o $(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	$(CC) -o $@ -c $(CFLAGS) $< \
	    -MMD -MP -MF"$(@:%.c.o=%.c.d)" -MT"$@ $(@:%.c.o=%.S) $(@:%.c.o=%.c.d)" \
	    $(CINCS)
	if [ -f "$(notdir $(@:.c.o=.s))" -a ! -f "$(@:.c.o=.s)" ]; then \
	    mv "$(notdir $(@:.c.o=.s))" "$(dir $@)"; fi
	if [ -f "$(notdir $(@:.c.o=.i))" -a ! -f "$(@:.c.o=.i)" ]; then \
	    mv "$(notdir $(@:.c.o=.i))" "$(dir $@)"; fi

# Compile: create assembler files from C source files.
%.S $(OBJDIR)/%.S: %.c
	$(CC) -o $@ -S $(CFLAGS) $< \
	    -MMD -MP -MF"$(@:%.S=%.S.d)" -MT"$(@:%.S=%.o) $@ $(@:%.S=%.S.d)" \
	    $(CINCS)

# Assemble: create object files from assembler source files.
%.S.o $(OBJDIR)/%.S.o: %.S
	$(CC) -o $@ -c $(ASFLAGS) $< \
	    -MMD -MP -MF"$(@:%.S.o=%.S.d)" -MT"$@ $(@:%.S.o=%.S) $(@:%.S.o=%.S.d)" \
	    $(CINCS)

# Create extended listing file from object file.
%.lss %.listing: %.o
	$(OBJDUMP) $(LISTING_ARGS) $< > $@

%.hex: %.elf
	$(OBJCOPY) -O $(HEXFORMAT) -R .eeprom $< $@

%.eep: %.elf
	-$(OBJCOPY) -j .eeprom \
	    --set-section-flags=.eeprom="alloc,load" \
	    --change-section-lma .eeprom=0 \
	    -O $(HEXFORMAT) $< $@

# Create extended listing file from ELF output file.
%.lss %.listing: %.elf
	$(OBJDUMP) $(LISTING_ARGS) $< > $@

# Create a symbol table from ELF output file.
%.sym %.symbol: %.elf
	$(NM) $(SYMBOL_ARGS) $< | uniq > $@


### Explicit rules.
.PHONY: all build elf hex eep lss lst sym listing symbol size tar help
.PHONY: coff extcoff
.PHONY: reset reset_stty reset_python reset_perl upload up clean depend mkout
.PHONY: showvars showvars2

# Default target.
all: elf hex eep listing symbol size

build: elf hex

elf: $(BINDIR) $(BINDIR)/$(PROJECT).elf
hex: $(BINDIR) $(BINDIR)/$(PROJECT).hex
eep: $(BINDIR) $(BINDIR)/$(PROJECT).eep
lss: $(BINDIR) $(BINDIR)/$(PROJECT).lss
lst: $(BINDIR) $(BINDIR)/$(PROJECT).lss
sym: $(BINDIR) $(BINDIR)/$(PROJECT).sym
listing: $(BINDIR) $(BINDIR)/$(PROJECT).listing
symbol: $(BINDIR) $(BINDIR)/$(PROJECT).symbol
tar: $(TARFILE).xz


help:
	@printf "\
Makefile version $(MKVERSION) by Volker Kuhlmann\n\
Makefile targets (run \"make <target>\"):\n\
   all           Compile program and create listing, symbol list etc.\n\
   upload        Upload program to the board (or just use 'up')\n\
   size          Show size of all .elf and .hex files in output directory\n\
   reset         Reset board\n\
   tar           Create tar file of program\n\
   dtr           Show current state of serial port's DTR line\n\
   showvars      Show almost all makefile variables\n\
   mkout         Create output directory\n\
   depend        Put all dependencies into one file. Doesn't work, don't use.\n\
   clean         Delete all generated files\n\
"

# Show variables. Essential when developing this makefile.
showvars:
	@make --no-print-directory $(MAKEVARS) showvars2 | $${PAGER:-less}


showvars2:
	: PROJECT = "$(PROJECT)", VERSION = "$(VERSION)"
	: F_CPU = "$(F_CPU)"
	: PORT = "$(PORT)"
	: UPLOAD_RATE = "$(UPLOAD_RATE)"
	: MCU = "$(MCU)"
	: PROGRAMMER = "$(PROGRAMMER)"
	: AVRDUDE = "$(AVRDUDE)"
	: AVRDUDE_FLAGS = "$(AVRDUDE_FLAGS)"
	: AVRDUDE_WRITE_FLASH = "$(AVRDUDE_WRITE_FLASH)"
	: AVRDUDE_WRITE_EEPROM = "$(AVRDUDE_WRITE_EEPROM)"
	: AVRDUDE_READ_BACKUP = "$(AVRDUDE_READ_BACKUP)"
	: BACKUPDIR = "$(BACKUPDIR)"
	: USER_LIBS = "$(USER_LIBS)"
	: CINCS = "$(CINCS)"
	: SRC = "$(SRC)"
	: ASRC = "$(ASRC)"
	: CFLAGS = "$(CFLAGS)"
	: OBJECTS = "$(OBJECTS)"
	: ALLDEPS = "$(ALLDEPS)"
	: VPATH = "$(VPATH)"


mkout $(OBJDIR) $(BINDIR):
	mkdir -p $(OBJDIR) $(BINDIR)


$(BINDIR)/libapp.a: $(OBJECTS)
	$(AR) rcsv $@ $(OBJECTS)


$(BINDIR)/libapp2.a: $(OBJECTS)
	$(AR) rcsv $@ $(filter-out $(BINDIR)/$(PROJECT).o,$(OBJECTS))


# Link program from objects and libraries.
$(BINDIR)/$(PROJECT).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
	 $(OBJECTS) \
	 -L$(BINDIR) $(LDLIBS)

# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
# UNTESTED
COFFCONVERT=$(OBJCOPY) --debugging \
    --change-section-address .data-0x800000 \
    --change-section-address .bss-0x800000 \
    --change-section-address .noinit-0x800000 \
    --change-section-address .eeprom-0x810000


coff: $(BINDIR)/$(PROJECT).elf
	$(COFFCONVERT) -O coff-avr $(BINDIR)/$(PROJECT).elf $(PROJECT).cof


extcoff: $(BINDIR)/$(PROJECT).elf
	$(COFFCONVERT) -O coff-ext-avr $(BINDIR)/$(PROJECT).elf $(PROJECT).cof


# Display size of file.
# (Actually, sizes of all $(PROJECT) .elf and .hex in $(BINDIR).)
size:
	@echo; #echo
	-$(SIZE) $(BINDIR)/$(PROJECT)*.elf
	@echo
	-$(SIZE) --target=$(HEXFORMAT) $(BINDIR)/$(PROJECT)*.hex
	@#echo


# Reset the board before uploading a new program.
# The board is reset on a rising edge of DTR; to make it always happen,
# make sure to set the output low before setting it high.
# Alternatively perl and python programs can be used (stty is faster).
reset: reset_$(RESETCMD)
reset_stty:
	$(STTY) -hupcl; sleep 0.1
	$(STTY) hupcl; sleep 0.1
	$(STTY) -hupcl


# Show the current state of the DTR line.
dtr:
	$(STTY) -a | tr ' ' '\n' | grep hupcl


ifeq "$(PROGRAMMER)" "jtag2isp"
upload up: hex eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)
else
upload up: hex eep reset
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)
endif


backup:
	$(shell mkdir -p $(BACKUPDIR) 2>/dev/null)
	@for memory in $(MEMORYTYPES); do \
	 $(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_READ_BACKUP); \
	done


# Create tar file.
# TODO: Dependencies on the header files are missing.
TAREXCL= $(BINDIR) back build debug
$(TARFILE).bz2: $(SRC)
	PRJBASE=$$(basename "$$PWD"); \
	cd ..; \
	tar -cvf "$(TARFILE)$(suffix $@)" --bzip2 \
	    $(patsubst %,--exclude "%", $(TAREXCL)) \
	    $(patsubst %,--exclude "$(TARFILE)%", $(suffix $@) .??? .??) \
	    --owner=root --group=root "$$PRJBASE" \
	&& mv "$(TARFILE)$(suffix $@)" "$$OLDPWD" \
	&& echo "" && echo "Created $(TARFILE)$(suffix $@)"
$(TARFILE).xz: $(SRC)
	PRJBASE=$$(basename "$$PWD"); \
	cd ..; \
	tar -cvf "$(TARFILE)$(suffix $@)" --xz \
	    $(patsubst %,--exclude "%", $(TAREXCL)) \
	    $(patsubst %,--exclude "$(TARFILE)%", $(suffix $@) .??? .??) \
	    --owner=root --group=root "$$PRJBASE" \
	&& mv "$(TARFILE)$(suffix $@)" "$$OLDPWD" \
	&& echo "" && echo "Created $(TARFILE)$(suffix $@)"


# Target: clean project.
CLEANEXT = .elf .hex .eep .cof .lss .sym .listing .symbol .map .log
clean:
	-$(RM) \
	    $(CLEANEXT:%=$(BINDIR)/$(PROJECT)%) \
	    $(patsubst %,$(BINDIR)/lib%.a,core duino user app app2 duinoall all) \
	    $(OBJECTS) \
	    $(ALLDEPS) \
	    $(OBJECTS:%.o=%.S) \
	    $(OBJECTS:%.o=%.s) \
	    $(OBJECTS:%.o=%.i) \
	    $(OBJECTS:%.o=%.ii) \
	    $(notdir $(OBJECTS:%.o=%.s) $(OBJECTS:%.o=%.i) $(OBJECTS:%.o=%.ii))
	-test ! -d $(BINDIR) || $(RMDIR) $(BINDIR)
	-test ! -d $(OBJDIR) || $(RMDIR) $(OBJDIR)


### Dependencies file and source path.

# This must be after the first explicit rule.
# -include $(ALLDEPS)
