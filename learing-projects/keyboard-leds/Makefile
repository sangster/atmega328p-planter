MKVERSION = 2.0

# Determine operating system environment.
# Possible values are (tested): Linux, FreeBSD (on 8.1), ...
OSNAME =	$(shell uname)

# Name of the program and source .ino (previously .pde) file.
# No extension here (e.g. PROJECT = Blink).
PROJECT ?=	keyboard_leds

# Project version. Only used for packing the source into an archive.
VERSION ?=	1.0

# Arduino model. E.g. atmega328, mega2560, uno.
# Valid model names can be found in $(ARDUINO_DIR)/hardware/archlinux-arduino/avr/boards.txt
# This must be set to a valid model name.
# ARDUINO_MODEL ?= micro
ARDUINO_MODEL = uno
#ARDUINO_MODEL = nano328  # Is set to a 168 CPU
#ARDUINO_MODEL = atmega2560

# Arduino family E.g. mega, diecimila, nano.
# Valid family names can be found in $(ARDUINO_Dir)/hardware/archlinux-arduino/avr/boards.txt
# Set this if your card is a part of a subset
ARDUINO_FAMILY = mega

# Arduino variant (for Arduino 1.0+).
# Directory containing the pins_arduino.h file.
#ARDUINO_VARIANT=$(ARDUINO_DIR)/hardware/archlinux-arduino/avr/variants/micro

# MCU architecture.
# Currently hardcoded to avr (sam, etc. are unsupported.)
ARCH ?= avr

# USB port the Arduino board is connected to.
# Linux: e.g. /dev/ttyUSB0, or /dev/ttyACM0 for the Uno.
# BSD:   e.g. /dev/cuaU0
# USBASP: e.g. /dev/ttyS0
# It is a good idea to use udev rules to create a device name that is constant,
# based on the serial number etc. of the USB device.
# PORT ?=		/dev/serial/by-id/*Arduino*
PORT ?=		/dev/ttyUSB0

# Arduino version (e.g. 23 for 0023, or 105 for 1.0.5).
# Make sure this matches ARDUINO_DIR below!
#ARDUINO = 	23
ARDUINO ?= 	161

# Location of the official Arduino IDE.
# E.g. /usr/local/arduino, or $(HOME)/arduino
# Make sure this matches ARDUINO above!
ARDUINO_DIR ?=	/usr/share/arduino

# Arduino 0.x based on 328P now need the new programmer protocol.
# Arduino 1.6+ uses the avr109 programmer by default
# ICSP programmers can also be used, for example: usbasp
# If unset, a default is chosen based on ARDUINO_MODEL and ARDUINO_FAMILY.
# AVRDUDE_PROGRAMMER = usbasp
# AVRDUDE_PROGRAMMER = avr109

# User libraries (in ~/sketchbook/libraries/).
# Give the name of the directory containing the library source files.
USER_LIBDIR ?=	./libraries
USER_LIBS ?=

# Additional pre-compiled libraries to link with.
# Always leave the math (m) library last!
# The Arduino core library is automatically linked in.
# If the library is in a location the compiler doesn't already know, also
# give the directory with -L.
# Note this is dealing with real libraries (libXXX.a), not Arduino "libraries"!
LDLIBS ?=
LDLIBS +=	-lm

LISTING_ARGS =	-h -S
LISTING_ARGS += -t -l -C -w

SYMBOL_ARGS =	-n
SYMBOL_ARGS +=	-C

# Directory in which files are created.
# Using the current directory ('.') is untested (and probably unwise).
OUTPUT ?=	bin

# Where are tools like avr-gcc located on your system?
# If you set this, it must end with a slash!
#AVR_TOOLS_PATH = $(ARDUINO_DIR)/hardware/tools/avr/bin/
#AVR_TOOLS_PATH = /usr/bin/
AVR_TOOLS_PATH ?=

# Reset command to use.
# Possible values are: "stty", "python", "perl".
#RESETCMD =	stty

### Macro definitions. Place -D or -U options here.
CDEFS ?=
ifdef LTO
CDEFS +=	-DLTO
endif
ifdef SD
CDEFS +=	-DUSE_SD
endif
ifdef mega
CDEFS +=	-DARDUINO_MEGA
endif

############################################################################
# Below here nothing should need to be changed.
############################################################################

# Output hex format.
HEXFORMAT =	ihex

# Name of the dependencies file (used for "make depend").
# This doesn't work too well.
# Maybe drop this idea and use auto-generated dependencies (*.d) instead?
DEPFILE =	$(OUTPUT)/Makefile.depend

# Name of the tar file in which to pack the user program up in.
TARFILE =	$(PROJECT)-$(VERSION).tar

# Default reset command if still unset.
RESETCMD ?=	stty

# Get the upload rate, CPU model, CPU frequency, avrdude programmer type
# and other variables from the IDE files.

ifdef ARDUINO_FAMILY
MODEL_PATTERN_MATCHING = $(ARDUINO_MODEL)\|$(ARDUINO_FAMILY)
else
MODEL_PATTERN_MATCHING = $(ARDUINO_MODEL)
endif

getboardvar = $(shell \
	sed "/^\($(MODEL_PATTERN_MATCHING)\)\.$(1)=/ { s/.*=//; q }; d" \
		$(ARDUINO_DIR)/hardware/archlinux-arduino/avr/boards.txt \
	)

UPLOAD_RATE ?=	$(call getboardvar,upload.speed)
MCU ?=		$(call getboardvar,build.mcu)
F_CPU ?=	$(call getboardvar,build.f_cpu)
AVRDUDE_PROGRAMMER ?= $(call getboardvar,upload.protocol)
VID ?=		$(call getboardvar,build.vid)
PID ?=		$(call getboardvar,build.pid)
BOARD ?=	$(call getboardvar,build.board)

# Try and guess PORT if it wasn't set previously.
# Note using shell globs most likely won't work, so try first port.
ifeq "$(OSNAME)" "Linux"
ifeq ("$(ARDUINO_MODEL)", $(filter "$(ARDUINO_MODEL)", "uno" "mega2560"))
    PORT ?= /dev/ttyACM0
else
    PORT ?= /dev/ttyUSB0
endif
else
    # Not Linux, so try BSD port name
    PORT ?= /dev/cuaU0
endif

# Try and guess ARDUINO_VARIANT if it wasn't set previously.
# Possible values for Arduino 1.0 are:
#   eightanaloginputs leonardo mega micro standard
# This makefile part is incomplete. Best set variant explicitly at the top.
# Default is "standard".
ifeq ($(ARDUINO_VARIANT),)
ifeq ("$(ARDUINO_MODEL)", $(filter "$(ARDUINO_MODEL)", "mega" "mega2560"))
ARDUINO_VARIANT ?= $(ARDUINO_DIR)/hardware/archlinux-arduino/avr/variants/mega
else
ifeq "$(ARDUINO_MODEL)" "micro"
ARDUINO_VARIANT ?= $(ARDUINO_DIR)/hardware/archlinux-arduino/avr/variants/micro
else
ARDUINO_VARIANT ?= $(ARDUINO_DIR)/hardware/archlinux-arduino/avr/variants/standard
endif
endif
endif


### Sources

# User library sources.
ULIBDIRS = $(wildcard \
		$(USER_LIBS:%=$(USER_LIBDIR)/%) \
		$(USER_LIBS:%=$(USER_LIBDIR)/%/utility) \
		)
ULIBSRC =	$(wildcard $(ULIBDIRS:%=%/*.c))
ULIBASMSRC =	$(wildcard $(ULIBDIRS:%=%/*.S))

# User program sources.
SRC =		$(wildcard *.c)
ASRC =		$(wildcard *.S)

# Paths to check for source files (pre-requisites).
# (Note: The vpath directive clears the path if the argument is empty!)
ifneq "$(ULIBDIRS)" ""
  vpath % $(ULIBDIRS)
endif
vpath % .


### Include directories.
CINCS = \
	$(ULIBDIRS:%=-I%) \
	-I.


### Object and dependencies files.

# User libraries used.
ULIBOBJ =	$(addprefix $(OUTPUT)/,$(notdir \
			$(ULIBSRC:.c=.c.o) \
			$(ULIBASMSRC:.S=.S.o) \
		))

# User program.
OBJ =		$(addprefix $(OUTPUT)/,$(notdir \
			$(SRC:.c=.c.o) \
			$(ASRC:.S=.S.o) \
		))

# All object files.
#ALLOBJ =	$(ULIBOBJ) $(OBJ)
ALLOBJ =	$(OBJ) $(ULIBOBJ)

# All dependencies files.
ALLDEPS =	$(ALLOBJ:%.o=%.d)


### More macro definitions.
# -DF_CPU and -DARDUINO are mandatory.
CDEFS += 	-DF_CPU=$(F_CPU) -DARDUINO=$(ARDUINO)
CDEFS +=	-DARDUINO_$(BOARD)
CDEFS +=	-DARDUINO_ARCH_$(shell echo $(ARCH) | tr '[a-z]' '[A-Z]')


### C Compiler flags.

# C standard level.
# c89   - ISO C90 ("ANSI" C)
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions (default for C)
CSTANDARD =	-std=gnu99

# Optimisations.
OPT_OPTIMS =	-Os
OPT_OPTIMS +=	-ffunction-sections -fdata-sections
OPT_OPTIMS +=	-mrelax
# -mrelax crashes binutils 2.22, 2.19.1 gives 878 byte shorter program.
# The crash with binutils 2.22 needs a patch. See sourceware #12161.
ifdef LTO
OPT_OPTIMS +=	-flto
#OPT_OPTIMS +=	-flto-report
#OPT_OPTIMS +=	-fwhole-program
# -fuse-linker-plugin requires gcc be compiled with --enable-gold, and requires
# the gold linker to be available (GNU ld 2.21+ ?).
#OPT_OPTIMS +=	-fuse-linker-plugin
endif

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
OPT_DEBUG =	-g2 -gstabs

# Warnings.
# A bug in gcc 4.3.x related to progmem might turn a warning into an error
# when using -pedantic. This patch works around the problem:
# http://volker.top.geek.nz/arduino/avr-libc-3.7.1-pgmspace_progmem-fix.diff
# Turning on all warnings shows a large number of less-than-optimal program
# locations in the Arduino sources. Some might turn into errors. Either fix
# your Arduino sources, or turn the warnings off.
ifndef OPT_WARN
OPT_WARN =	-Wall
OPT_WARN +=	-pedantic
OPT_WARN +=	-Wextra
OPT_WARN +=	-Wmissing-declarations
OPT_WARN +=	-Wmissing-field-initializers
OPT_WARN +=	-Wsystem-headers
OPT_WARN +=	-Wno-variadic-macros
endif
ifndef OPT_WARN_C
OPT_WARN_C =	$(OPT_WARN)
OPT_WARN_C +=	-Wmissing-prototypes
endif

# Other.
ifndef OPT_OTHER
OPT_OTHER =
# Save gcc temp files (pre-processor, assembler):
#OPT_OTHER +=	-save-temps

# Automatically enable build.extra_flags if needed
# Used by Micro and other devices to fill in USB_PID and USB_VID
OPT_OTHER +=	-DUSB_VID=$(VID) -DUSB_PID=$(PID)
OPT_CPP_OTHER = -fno-use-cxa-atexit
endif

# Final combined.
CFLAGS =	-mmcu=$(MCU) \
		$(OPT_OPTIMS) $(OPT_DEBUG) $(CSTANDARD) $(CDEFS) \
		$(OPT_WARN) $(OPT_OTHER) $(CEXTRA)

### Assembler flags.

#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs

# Assembler standard level.
ASTANDARD =	-x assembler-with-cpp

# Final combined.
ASFLAGS =	-mmcu=$(MCU) \
		$(CDEFS) \
		$(ASTANDARD) $(ASEXTRA)


### Linker flags.

# Optimisation setting must match compiler's, esp. for -flto.

LDFLAGS =	-mmcu=$(MCU)
LDFLAGS +=	$(OPT_OPTIMS)
LDFLAGS +=	-Wl,--gc-sections
#LDFLAGS +=	-Wl,--print-gc-sections


### Programming / program uploading.

AVRDUDE_FLAGS =

# Do not verify.
#AVRDUDE_FLAGS+= -V

# Override invalid signature check.
#AVRDUDE_FLAGS+= -F

# Disable auto erase for flash memory. (IDE uses this too.)
AVRDUDE_FLAGS+= -D

# Quiet -q -qq / Verbose -v -vv.
AVRDUDE_FLAGS+= -q

AVRDUDE_FLAGS+= -p $(MCU) -c $(AVRDUDE_PROGRAMMER) -b $(UPLOAD_RATE)
AVRDUDE_FLAGS+= -P $(PORT)

# avrdude config file
AVRDUDE_FLAGS+= -C /etc/avrdude.conf
#AVRDUDE_FLAGS+= -C $(ARDUINO_DIR)/hardware/tools/avr/etc/avrdude.conf

AVRDUDE_WRITE_FLASH = -U flash:w:$(OUTPUT)/$(PROJECT).hex:i


### Programs

AVRPREFIX =	avr-
CC =		$(AVR_TOOLS_PATH)$(AVRPREFIX)gcc
OBJCOPY =	$(AVR_TOOLS_PATH)$(AVRPREFIX)objcopy
OBJDUMP =	$(AVR_TOOLS_PATH)$(AVRPREFIX)objdump
AR =		$(AVR_TOOLS_PATH)$(AVRPREFIX)ar
SIZE =		$(AVR_TOOLS_PATH)$(AVRPREFIX)size
NM =		$(AVR_TOOLS_PATH)$(AVRPREFIX)nm
AVRDUDE =	$(AVR_TOOLS_PATH)avrdude
#AVRDUDE =	$(ARDUINO_DIR)/hardware/tools/avrdude
RM =		rm -f
RMDIR = 	rmdir
MV =		mv -f
ifeq "$(OSNAME)" "Linux"
    STTY =	stty -F $(PORT)
else
    # BSD uses small f
    STTY =	stty -f $(PORT)
endif


### Implicit rules

.SUFFIXES: .ino .pde .elf .hex .eep .lss .listing .sym .symbol
.SUFFIXES: .cpp .c .S .o .a

# Compile: create object files from C source files.
%.c.o $(OUTPUT)/%.c.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $< \
	  -MMD -MP -MF"$(@:%.c.o=%.d)" -MT"$@ $(@:%.c.o=%.S) $(@:%.c.o=%.d)" \
	  $(CINCS)
	if [ -f "$(notdir $(@:.c.o=.s))" -a ! -f "$(@:.c.o=.s)" ]; then \
	  mv "$(notdir $(@:.c.o=.s))" "$(dir $@)"; fi
	if [ -f "$(notdir $(@:.c.o=.i))" -a ! -f "$(@:.c.o=.i)" ]; then \
	  mv "$(notdir $(@:.c.o=.i))" "$(dir $@)"; fi

# Compile: create assembler files from C source files.
%.S $(OUTPUT)/%.S: %.c
	$(CC) -o $@ -S $(CFLAGS) $< \
	  -MMD -MP -MF"$(@:%.S=%.d)" -MT"$(@:%.S=%.o) $@ $(@:%.S=%.d)" \
	  $(CINCS)

# Assemble: create object files from assembler source files.
%.S.o $(OUTPUT)/%.S.o: %.S
	$(CC) -o $@ -c $(ASFLAGS) $< \
	  -MMD -MP -MF"$(@:%.S.o=%.d)" -MT"$@ $(@:%.S.o=%.S) $(@:%.S.o=%.d)" \
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
#	$(NM) $(SYMBOL_ARGS) $< > $@
	$(NM) $(SYMBOL_ARGS) $< | uniq > $@

### Explicit rules.

.PHONY: all build elf hex eep lss lst sym listing symbol size tar help extra
.PHONY: coff extcoff
.PHONY: reset reset_stty reset_python reset_perl upload up clean depend mkout
.PHONY: showvars showvars2

# Default target.
all:	elf hex eep listing symbol size

build:	elf hex

elf:	$(OUTPUT) $(OUTPUT)/$(PROJECT).elf
hex:	$(OUTPUT) $(OUTPUT)/$(PROJECT).hex
eep:	$(OUTPUT) $(OUTPUT)/$(PROJECT).eep
lss:	$(OUTPUT) $(OUTPUT)/$(PROJECT).lss
lst:	$(OUTPUT) $(OUTPUT)/$(PROJECT).lss
sym:	$(OUTPUT) $(OUTPUT)/$(PROJECT).sym
listing: $(OUTPUT) $(OUTPUT)/$(PROJECT).listing
symbol: $(OUTPUT) $(OUTPUT)/$(PROJECT).symbol
tar:	$(TARFILE).xz
extra:	$(patsubst %,$(OUTPUT)/$(PROJECT)_2%, .elf .hex .listing .symbol) \
	$(patsubst %,$(OUTPUT)/$(PROJECT)_3%, .elf .hex .listing .symbol) \
	$(patsubst %,$(OUTPUT)/$(PROJECT)_4%, .elf .hex .listing .symbol) \
	$(patsubst %,$(OUTPUT)/$(PROJECT)_6%, .elf .hex .listing .symbol) \
	$(patsubst %,$(OUTPUT)/$(PROJECT)_8%, .elf .hex .listing .symbol) \
	$(patsubst %,$(OUTPUT)/$(PROJECT)_9%, .elf .hex .listing .symbol) \
	$(patsubst %,$(OUTPUT)/$(PROJECT)_A%, .elf .hex .listing .symbol)

help:
	@printf "\
Arduino Makefile version $(MKVERSION) by Volker Kuhlmann\n\
Makefile targets (run \"make <target>\"):\n\
   all           Compile program and create listing, symbol list etc.\n\
   upload        Upload program to Arduino board (or just use 'up')\n\
   size          Show size of all .elf and .hex files in output directory\n\
   reset         Reset Arduino board\n\
   reset_stty    Reset using stty\n\
   reset_python  Reset using Python program\n\
   reset_perl    Reset using perl program\n\
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
	: ARDUINO = "$(ARDUINO)"
	: ARDUINO_MODEL = "$(ARDUINO_MODEL)"
	: ARDUINO_FAMILY = "$(ARDUINO_FAMILY)"
	: F_CPU = "$(F_CPU)"
	: PORT = "$(PORT)"
	: UPLOAD_RATE = "$(UPLOAD_RATE)"
	: MCU = "$(MCU)"
	: AVRDUDE_PROGRAMMER = "$(AVRDUDE_PROGRAMMER)"
	: AVRDUDE = "$(AVRDUDE)"
	: AVRDUDE_FLAGS = "$(AVRDUDE_FLAGS)"
	: AVRDUDE_WRITE_FLASH = "$(AVRDUDE_WRITE_FLASH)"
	: ARDUINO_DIR = "$(ARDUINO_DIR)"
	: ARDUINO_VARIANT = "$(ARDUINO_VARIANT)"
	: USER_LIBS = "$(USER_LIBS)"
	: ULIBDIRS = "$(ULIBDIRS)"
	: CINCS = "$(CINCS)"
	: SRC = "$(SRC)"
	: ASRC = "$(ASRC)"
	: ULIBSRC = "$(ULIBSRC)"
	: CFLAGS = "$(CFLAGS)"
	: ULIBOBJ = "$(ULIBOBJ)"
	: OBJ = "$(OBJ)"
	: ALLOBJ = "$(ALLOBJ)"
	: ALLDEPS = "$(ALLDEPS)"
	: VPATH = "$(VPATH)"
	: VID = "$(VID)"
	: PID = "$(PID)"
	: MODEL_PATTERN_MATCHING = "$(MODEL_PATTERN_MATCHING)"

mkout $(OUTPUT):
	mkdir -p $(OUTPUT)

$(OUTPUT)/libuser.a: $(ULIBOBJ)
	$(AR) rcsv $@ $(ULIBOBJ)

$(OUTPUT)/libapp.a: $(OBJ)
	$(AR) rcsv $@ $(OBJ)

$(OUTPUT)/libapp2.a: $(OBJ)
	$(AR) rcsv $@ $(filter-out $(OUTPUT)/$(PROJECT).o,$(OBJ))

$(OUTPUT)/liball.a: $(ULIBOBJ)
	$(AR) rcsv $@ $(ULIBOBJ)

# Link program from objects and libraries.
$(OUTPUT)/$(PROJECT).elf: $(ALLOBJ)
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		$(OBJ) \
		$(ULIBOBJ) \
		-L$(OUTPUT) $(LDLIBS)

# Alternative linking. Experimental, goes with the additional .a libraries.
# Don't make this dependent on $(OUTPUT), or circular re-makes occur.
# _5.elf fails linking with unresolved setup(), loop().
$(OUTPUT)/$(PROJECT)_2.elf: $(ALLOBJ)
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		$(OBJ) $(ULIBOBJ) \
		$(LDLIBS)
$(OUTPUT)/$(PROJECT)_3.elf: $(ALLOBJ)
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		$(ULIBOBJ) $(OBJ) \
		$(LDLIBS)
$(OUTPUT)/$(PROJECT)_4.elf: $(ALLOBJ) \
				$(OUTPUT)/libduino.a $(OUTPUT)/libuser.a
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		$(OBJ) \
		-L$(OUTPUT) -luser -lduino $(LDLIBS)
$(OUTPUT)/$(PROJECT)_5.elf: $(ALLOBJ) \
		   $(OUTPUT)/libduino.a $(OUTPUT)/libuser.a $(OUTPUT)/libapp.a
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		-L$(OUTPUT) -lapp -luser -lduino $(LDLIBS)
$(OUTPUT)/$(PROJECT)_6.elf: $(ALLOBJ) \
		   $(OUTPUT)/libduino.a $(OUTPUT)/libuser.a $(OUTPUT)/libapp2.a
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		$(OUTPUT)/$(PROJECT).o \
		-L$(OUTPUT) -lapp2 -luser -lduino $(LDLIBS)
# Try compiling in one big step, to ensure LTO works.
# Doesn't link - collect2 says Wire.cpp has undef refs to functions in twi.c.
# Changing order of sources doesn't fix that.
$(OUTPUT)/$(PROJECT)_8.elf: $(ALLOBJ) \
				$(OUTPUT)/libduinoall.a
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		$(OBJ) \
		$(ULIBOBJ) \
		-L$(OUTPUT) -lduinoall $(LDLIBS)
$(OUTPUT)/$(PROJECT)_9.elf: $(ALLOBJ) $(OUTPUT)/liball.a
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		$(OBJ) \
		-L$(OUTPUT) -lall $(LDLIBS)
$(OUTPUT)/$(PROJECT)_A.elf: $(ALLOBJ) \
				$(OUTPUT)/libduino.a
	$(CC) $(LDFLAGS) -Wl,-Map,$*.map,--cref -o $@ \
		$(OBJ) $(ULIBOBJ) \
		-L$(OUTPUT) -lduino $(LDLIBS)

# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
# UNTESTED
COFFCONVERT=$(OBJCOPY) --debugging \
    --change-section-address .data-0x800000 \
    --change-section-address .bss-0x800000 \
    --change-section-address .noinit-0x800000 \
    --change-section-address .eeprom-0x810000
coff: $(OUTPUT)/$(PROJECT).elf
	$(COFFCONVERT) -O coff-avr $(OUTPUT)/$(PROJECT).elf $(PROJECT).cof
extcoff: $(OUTPUT)/$(PROJECT).elf
	$(COFFCONVERT) -O coff-ext-avr $(OUTPUT)/$(PROJECT).elf $(PROJECT).cof

# Display size of file.
# (Actually, sizes of all $(PROJECT) .elf and .hex in $(OUTPUT).)
size:
	@echo; #echo
	-$(SIZE) $(OUTPUT)/$(PROJECT)*.elf
	@echo
	-$(SIZE) --target=$(HEXFORMAT) $(OUTPUT)/$(PROJECT)*.hex
	@#echo

# Reset the Arduino board before uploading a new program.
# The Arduino is reset on a rising edge of DTR; to make it always happen,
# make sure to set the output low before setting it high.
# Alternatively perl and python programs can be used (stty is faster).
reset: reset_$(RESETCMD)
reset_stty:
	$(STTY) -hupcl; sleep 0.1
	$(STTY) hupcl; sleep 0.1
	$(STTY) -hupcl

# Reset the Arduino board: Perl version needs libdevice-serialport-perl.
# zypper -vv in perl-Device-SerialPort
reset_perl:
	perl -MDevice::SerialPort -e \
	  'Device::SerialPort->new("$(PORT)")->pulse_dtr_off(100)'

# Reset the Arduino board: Python version needs python-serial.
# zypper -vv in python-serial
reset_python:
	python -c "\
	import serial; import time; \
	p = serial.Serial('$(PORT)', 57600); \
	p.setDTR(False); \
	time.sleep(0.1); \
	p.setDTR(True)"

# Show the current state of the DTR line.
dtr:
	$(STTY) -a | tr ' ' '\n' | grep hupcl

# Program the Arduino board (upload program).
upload up: hex reset
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

# Create tar file.
# TODO: Dependencies on the header files are missing.
TAREXCL=	$(OUTPUT) back build debug
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

# Single dependencies file for all sources.
# This doesn't really work, so don't use it.
depend: $(OUTPUT)
	$(CC) -M -mmcu=$(MCU) $(CDEFS) \
	    $(CINCS) \
	    $(ULIBSRC) \
	    $(SRC) $(ASRC) \
	    > $(DEPFILE)

# Target: clean project.
CLEANEXT = .elf .hex .eep .cof .lss .sym .listing .symbol .map .log
CLEANPRG = $(foreach p,_2 _3 _4 _5 _6 _7 _8 _9 _A,$(patsubst %,$p%,$(CLEANEXT)))
clean:
	-$(RM) \
	  $(DEPFILE) \
	  $(OUTPUT)/$(PROJECT).cpp \
	  $(CLEANEXT:%=$(OUTPUT)/$(PROJECT)%) \
	  $(CLEANPRG:%=$(OUTPUT)/$(PROJECT)%) \
	  $(patsubst %,$(OUTPUT)/lib%.a,core duino user app app2 duinoall all) \
	  $(ALLOBJ) \
	  $(ALLDEPS) \
	  $(ALLOBJ:%.o=%.S) \
	  $(ALLOBJ:%.o=%.s) \
	  $(ALLOBJ:%.o=%.i) \
	  $(ALLOBJ:%.o=%.ii) \
	  $(notdir $(ALLOBJ:%.o=%.s) $(ALLOBJ:%.o=%.i) $(ALLOBJ:%.o=%.ii))
	-test ! -d $(OUTPUT) || $(RMDIR) $(OUTPUT)


### Dependencies file and source path.

# This must be after the first explicit rule.

-include $(DEPFILE)

-include $(ALLDEPS)
