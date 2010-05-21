#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!    
#
##############################################################################################
# 
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#
# make HEX_FILE="FG" HEX_PATH="C:/Users/Tecnequip/Documents/" TOOLS_PATH="C:/Program Files/yagarto/bin/"
##############################################################################################
# Start of default section
#

# Define project name here
ifdef HEX_FILE
PROJECT = $(HEX_FILE)
else
PROJECT = POP7
endif

ifdef HEX_PATH
TARGET_PATH = $(HEX_PATH)
else
TARGET_PATH = C:/Users/Tecnequip/Documents/
endif

ifdef TOOLS_PATH
TOOLS_PATH = $(TOOLS_PATH)
else
TOOLS_PATH = C:/Program Files/yagarto/bin/
endif

TRGT = $(TOOLS_PATH)arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp

MCU  = cortex-m3

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = 

# List all default directories to look for include files here
DINCDIR = 

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS = 

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

# Define linker script file here
LDSCRIPT = ./prj/lpc1768_ram.ld

# List all user C define here, like -D_DEBUG=1
UDEFS = 

# Define ASM defines here
UADEFS = 

# List C source files here
SRC  = ./src/main.c \
       ./src/system_LPC17xx.c \
       ./src/core_cm3.c \
       ./src/ld.c \
       ./src/timer.c \
       ./src/startup.c

# List ASM source files here
ASRC =

# List all user directories here
UINCDIR = ./inc

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS = 

# Define optimisation level here
OPT = -O0

#
# End of user defines
##############################################################################################


INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
DEFS    = $(DDEFS) $(UDEFS)
ADEFS   = $(DADEFS) $(UADEFS)
OBJS    = $(ASRC:.s=.o) $(SRC:.c=.o)
LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU)

ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS) -O0 -save-temps -ggdb
LDFLAGS = $(MCFLAGS) -mthumb -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch $(LIBDIR)

# Generate dependency information
CPFLAGS += -MD -MP -MF .dep/$(@F).d

#
# makefile rules
#

all: RAM

RAM: $(OBJS) $(PROJECT).elf $(PROJECT).hex $(PROJECT).bin

%o : %c
	"$(CC)" -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

%o : %s
	"$(AS)" -c $(ASFLAGS) $< -o $@

$(PROJECT).elf: $(OBJS)
	"$(CC)" $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	"$(CP)" -O ihex "$<" "$(TARGET_PATH)$@"

%bin: %elf
	"$(CP)" -O binary "$<" "$(TARGET_PATH)$@"

clean:
	-rm -f *.dep
	-rm -f *.elf
	-rm -f *.hex
	-rm -f *.bin
	-rm -f *.s
	-rm -f *.i
	-rm -f *.map
	-rm -f *.log
	-rm -f $(OBJS)
	-rm -f $(PROJECT).elf
	-rm -f $(PROJECT).map
	-rm -f $(PROJECT).hex
	-rm -f $(SRC:.c=.c.bak)
	-rm -f $(SRC:.c=.lst)
	-rm -f $(ASRC:.s=.s.bak)
	-rm -f $(ASRC:.s=.lst)

# 
# Include the dependency files, should be the last of the makefile
#
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
