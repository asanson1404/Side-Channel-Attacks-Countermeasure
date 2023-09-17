# Compile tools
PREFIX=arm-none-eabi-
AS    = $(PREFIX)as
CC    = $(PREFIX)gcc
LD    = $(PREFIX)gcc
GDB   = gdb-multiarch

# Object file to execute
ELFFILE = skinny_STM32.elf

# Include files
INCLUDES += -I./cmsis
INCLUDES += -I./Drivers/Inc/

# Flags
ASFLAGS = -g -mcpu=cortex-m4 -mthumb
LDFLAGS = -g -Tld_ram.lds -nostartfiles -nostdlib
CFLAGS  = -g -O1 -Wall -Wextra -mcpu=cortex-m4 -mthumb -ffreestanding -mhard-float $(INCLUDES)

# Path towards other file to compile
VPATH := Drivers/Src Drivers/Clocks Drivers/Cache Implementation/ref Implementation/affine_mask

# Object files
# we don't want to remove the clock.o file!
OBJFILES = crt0.o firmware.o memfuncs.o trigger.o \
		   led.o uart.o trng.o \
		   skinny_reference.o \
		   mask.o skinny_aff.o \
		   clocks.o \
		   cache.o \
		   init.o

####################################################
# Targets
####################################################
.PHONY: all clean build startgdbserver debug 


all: clean build

clean:
	rm -rf $(ELFFILE) $(OBJFILES)

build: $(ELFFILE)

startgdbserver:
	JLinkGDBServer -device STM32L475VG -endian little -if SWD -speed auto -ir -LocalhostOnly

debug:
	$(GDB) $(ELFFILE)
	

####################################################
# Rules
####################################################
$(ELFFILE): $(OBJFILES)
	$(LD) $(LDFLAGS)  $^ -o $@
