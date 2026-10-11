# goldhen plugin build. ci builds this on linux with the openorbis toolchain and the goldhen
# sdk installed into it. OO_PS4_TOOLCHAIN must point at the toolchain, and libGoldHEN_Hook.a +
# crtprx.o + the GoldHEN headers must live under it (ci does `make install` then copies crtprx.o)
TARGET    := satchel_remover.prx
TOOLCHAIN := $(OO_PS4_TOOLCHAIN)
PROJDIR   := src
INTDIR    := build

LIBS   := -lSceLibcInternal -lkernel -lSceSysmodule -lGoldHEN_Hook
CFILES := $(wildcard $(PROJDIR)/*.c)
OBJS   := $(patsubst $(PROJDIR)/%.c,$(INTDIR)/%.o,$(CFILES))

CFLAGS  := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c -O2 \
           -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include -I$(PROJDIR)
LDFLAGS := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x -e _init --eh-frame-hdr \
           -L$(TOOLCHAIN)/lib $(LIBS)
CRTPRX  := $(TOOLCHAIN)/lib/crtprx.o

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  CC   := clang
  LD   := ld.lld
  CDIR := linux
endif
ifeq ($(UNAME_S),Darwin)
  CC   := /opt/homebrew/opt/llvm/bin/clang
  LD   := /opt/homebrew/opt/llvm/bin/ld.lld
  CDIR := macos
endif

all: $(TARGET)

$(INTDIR):
	@mkdir -p $(INTDIR)

$(INTDIR)/%.o: $(PROJDIR)/%.c | $(INTDIR)
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(CRTPRX) -o $(INTDIR)/satchel_remover.elf $(LDFLAGS)
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself -in=$(INTDIR)/satchel_remover.elf -out=$(INTDIR)/satchel_remover.oelf --lib=$(TARGET) --paid 0x3800000000000011

clean:
	rm -rf $(INTDIR) $(TARGET)

.PHONY: all clean
