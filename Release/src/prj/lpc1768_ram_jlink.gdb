#
# This config file was tested with J-Link GDB Server v4.10i
#

# connect to the J-Link gdb server
target remote localhost:2331

# Set gdb server to little endian
monitor endian little

# Set JTAG speed to 5 kHz
monitor speed 5

# Reset the target
monitor reset
monitor sleep 100

# Set JTAG speed in khz
monitor speed auto

# Vector table placed in RAM
monitor writeu32 0xE000ED08 = 0x10000000

load

monitor reg r13 = (0x10000000)
monitor reg pc = (0x10000004)

break ResetISR
break main
continue
