#------------------------------------------------------------------------------
# Definições
#------------------------------------------------------------------------------

# Define project name here
ifdef HEX_FILE
PROJECT = $(HEX_FILE)
else
PROJECT = POP7
endif

ifdef HEX_PATH
TARGET_PATH = $(HEX_PATH)
else
TARGET_PATH = ./
endif

ifdef TOOLS_PATH
TOOLS_PATH = $(TOOLS_PATH)
else
TOOLS_PATH = C:/Program Files/yagarto
endif

GNU_CC=arm-none-eabi
GCC_VERSION = 4.4.1

#------------------------------------------------------------------------------
# Diretorios de trabalho
#------------------------------------------------------------------------------

GCC_DIR = $(TOOLS_PATH)
GCC_INC = $(GCC_DIR)/include

HOME_DIR=.

SRC_DIR=$(HOME_DIR)
APP_DIR=$(HOME_DIR)
LIB_DIR=$(HOME_DIR)

#------------------------------------------------------------------------------
# Ferramentas ARM ELF
#------------------------------------------------------------------------------

CC=$(GCC_DIR)/bin/$(GNU_CC)-gcc
AS=$(GCC_DIR)/bin/$(GNU_CC)-as
LD=$(GCC_DIR)/bin/$(GNU_CC)-ld
AR=$(GCC_DIR)/bin/$(GNU_CC)-ar
OBJSIZE=$(GCC_DIR)/bin/$(GNU_CC)-size
OBJCOPY=$(GCC_DIR)/bin/$(GNU_CC)-objcopy

#---------------------------------------------------------------------------
# Bibliotecas e includes
#------------------------------------------------------------------------------

#LIB_GCC = $(GCC_DIR)/lib/gcc/$(GNU_CC)/$(GCC_VERSION)/libgcc.a
#LIB_C   = $(GCC_DIR)/$(GNU_CC)/lib/libc.a
#LIB_M   = $(GCC_DIR)/$(GNU_CC)/lib/libm.a

LIB_GCC = ./lib32bitc.a
LIB_C   = ./lib32bitc.a
LIB_M   = ./lib32bitc.a

#------------------------------------------------------------------------------
# Flags de compilação
#------------------------------------------------------------------------------
#  -Wall
#  -fpack-struct=1  Alinha as estruturas em 1 byte. 
#  -nodefaultlibs -nostdlib -nostartfiles 
#  -fomit-frame-pointer 
#  -Wstrict-prototypes 
# -Wno-trigraphs 
# -fno-strict-aliasing 
# -fno-common 
# -fstack-protector "no memory region specified for loadable section `__libc_freeres_fn'"
# -fomit-frame-pointer -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Wdeclaration-after-statement -Wno-pointer-sign
# -static
GCC_FLAGS_OPT=-mcpu=cortex-m3 -mlittle-endian -mthumb -Wall 
#  $(GCC_MODE_FLAG) -I $(GCC_INC) -I $(LIB_INC)
CFLAGS0=$(GCC_FLAGS_OPT)
CFLAGS1=$(CFLAGS0)

# -D
# --gstabs
# -mfpu=softfpa
# -alh
AAFLAGS=-D -mcpu=cortex-m3 -mthumb --gstabs 

# -N
# -v
# -T
LDFLAGS=-N -v -T

#------------------------------------------------------------------------------
# Diretorio de destino
#------------------------------------------------------------------------------

DIR_OBJ		= $(APP_DIR)
DIR_LST		= $(APP_DIR)
PROG_NAME	= POP7

#------------------------------------------------------------------------------
# Linker script files
#------------------------------------------------------------------------------

LDSCRIPT_RAM   = $(SRC_DIR)/lpc1768.ld
LDSCRIPT_FLASH = $(SRC_DIR)/lpc1768.ld

#------------------------------------------------------------------------------
# Objects to link
#------------------------------------------------------------------------------

OBJ_START	= $(LIB_DIR)/startup_LPC17xx.o
OBJ_FILES   = $(patsubst %.c,%.o,$(wildcard *.c))
OBJ_APP     = $(addprefix $(DIR_OBJ)/, $(OBJ_FILES))

#------------------------------------------------------------------------------
# Libraries to link to. LIB_GCC must be the last
#------------------------------------------------------------------------------

#LIB_LIST		= $(LIB_C) $(LIB_M) $(LIB_GCC)
LIB_LIST		= $(LIB_C)

#------------------------------------------------------------------------------
# Implicit rule to compile all C files in the directory
#------------------------------------------------------------------------------

$(DIR_OBJ)/%.o : %.c 
	$(CC) $(CFLAGS1) -c $< -o $@
	
#------------------------------------------------------------------------------
# Implicit rule to assemble all ASM files in the directory
#------------------------------------------------------------------------------

$(DIR_OBJ)/%.o : %.s
	$(AS) $(AAFLAGS) $< -o $@

./%.o : ./%.s
	$(AS) $(AAFLAGS1) -ahlms=$<.lst $< -o $@

#------------------------------------------------------------------------------
# default target: Generates application WITHOUT optimization
#------------------------------------------------------------------------------
#$(CC) $(CFLAGS1) -Wa,-a,-ad -g -c main.c > main.lst
$(DIR_OBJ)/$(PROG_NAME).elf:  makefile $(OBJ_APP)
	$(AS) $(AAFLAGS1) ./startup_LPC17xx.s -o startup_LPC17xx.o
	$(LD) -Map $(DIR_LST)/$(PROG_NAME).map $(LDFLAGS) $(LDSCRIPT_FLASH)\
                                         $(OBJ_APP) $(OBJ_START) $(LIB_LIST)\
                                         -o $(DIR_OBJ)/$(PROG_NAME).elf 
	$(OBJSIZE) $(DIR_LST)/$(PROG_NAME).elf
	$(OBJCOPY) -O binary $(DIR_LST)/$(PROG_NAME).elf $(TARGET_PATH)/$(PROJECT).bin
	$(OBJCOPY) -O ihex $(DIR_LST)/$(PROG_NAME).elf $(TARGET_PATH)/$(PROJECT).hex

#------------------------------------------------------------------------------
# release target: Generates application with optimization
#------------------------------------------------------------------------------

release: CFLAGS1 = -O2 $(CFLAGS0)
release: $(DIR_OBJ)/$(PROG_NAME).elf

#------------------------------------------------------------------------------
# debug target: Generates application WITHOUT optimization
#------------------------------------------------------------------------------

debug: CFLAGS1 = -O0 -save-temps -ggdb $(CFLAGS0)
debug: $(DIR_OBJ)/$(PROG_NAME).elf
		
#------------------------------------------------------------------------------
# clean target: Removes all generated files
#------------------------------------------------------------------------------

clean:
	rm -rf $(DIR_OBJ)/*.o $(DIR_OBJ)/*.elf \
         $(DIR_OBJ)/*.bin $(DIR_OBJ)/*.map $(DIR_OBJ)/*.hex $(DIR_OBJ)/*.i
