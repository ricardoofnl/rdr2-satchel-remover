# GoldHEN plugin build. arm64 macOS: uses Homebrew LLVM at /opt/homebrew (not the
# Intel /usr/local the stock samples assume). override the two SDK paths via env if needed.
OO_PS4_TOOLCHAIN ?= $(HOME)/dproject/ps4-sdk/OpenOrbis-PS4-Toolchain
GOLDHEN_SDK      ?= $(HOME)/dproject/ps4-sdk/GoldHEN_Plugins_SDK
LLVM             ?= /opt/homebrew/opt/llvm/bin

TARGET  = satchel_remover
CC      = $(LLVM)/clang
LD      = $(LLVM)/ld.lld
# create-fself rebuilt native arm64 (go build in OpenOrbis/create-fself); PATH or absolute
CFSELF  ?= create-fself

CFLAGS  = --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -O2 \
          -isysroot $(OO_PS4_TOOLCHAIN) -isystem $(OO_PS4_TOOLCHAIN)/include \
          -I$(GOLDHEN_SDK)/include -Isrc
LDFLAGS = -m elf_x86_64 -pie --script $(OO_PS4_TOOLCHAIN)/link.x -e _init \
          -L$(OO_PS4_TOOLCHAIN)/lib -L$(GOLDHEN_SDK)
LIBS    = $(GOLDHEN_SDK)/build/crtprx.o -lGoldHEN_Hook -lkernel -lSceLibcInternal -lSceSysmodule

SRCS = src/main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET).prx

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(TARGET).prx: $(TARGET).elf
	$(CFSELF) -in=$(TARGET).elf -out=$(TARGET).oelf --lib=$(TARGET).prx --paid 0x3800000000000011

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).oelf $(TARGET).prx

.PHONY: all clean
