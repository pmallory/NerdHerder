PROGNAME = NerdHerder

OFILES = myLib.o main.o splash.o nerd_splash.o text.o instructions.o nerd_sprite_sheet.o game.o nerd_bg.o gameover.o pause_img.o pause.o blue_danube.o

HFILES = myLib.h nerd_splash.h text.h nerd_sprite_sheet.h nerd_bg.h nerdLib.h pause_img.h blue_danube.h

# The flags to run the vba program with
# -f0  : Stretch 1x 
# -f1  : Stretch 2x
# -f13 : Stretch 3x
# -f15 : Stretch 4x
VBAOPT	 = -f15

################################################################################
# These are various settings used to make the GBA toolchain work
################################################################################

TOOLDIR  = /usr/local/cs2110-tools
CFLAGS   = -Wall -Werror -std=c99 -pedantic -O2
CFLAGS  += -mthumb-interwork -mlong-calls -nostartfiles
LDFLAGS = -L $(TOOLDIR)/lib \
		  -L $(TOOLDIR)/lib/gcc/arm-thumb-eabi/4.4.1/thumb \
		  -L $(TOOLDIR)/arm-thumb-eabi/lib \
		  --script $(TOOLDIR)/arm-thumb-eabi/lib/arm-gba.ld
CC      = $(TOOLDIR)/bin/arm-thumb-eabi-gcc
AS      = $(TOOLDIR)/bin/arm-thumb-eabi-as
LD      = $(TOOLDIR)/bin/arm-thumb-eabi-ld
OBJCOPY = $(TOOLDIR)/bin/arm-thumb-eabi-objcopy
GDB     = $(TOOLDIR)/bin/arm-thumb-eabi-gdb
GBADEPS = gbadeps
CFILES  = $(OFILES:.o=.c)

################################################################################
# These are the targets for the GBA build system
################################################################################

all: $(GBADEPS) $(PROGNAME).gba

$(PROGNAME).gba: $(PROGNAME).elf
	@echo "Linking files together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf: crt0.o $(OFILES)
	@$(LD) $(LDFLAGS) -o $(PROGNAME).elf $^ -lgcc -lc -lgcc

crt0.o: $(TOOLDIR)/arm-thumb-eabi/lib/crt0.s
	@$(AS) -mthumb-interwork $^ -o crt0.o

%.o: %.c
	@echo "[COMPILE] Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Removing all generated files"
	@rm -f *.o *.elf *.gba $(GBADEPS)

vba: $(PROGNAME).gba
	@echo "[RUN] Running Emulator"
	@$(TOOLDIR)/bin/vbam $(VBAOPT) $(PROGNAME).gba > /dev/null

rebuild: clean $(PROGNAME).gba

rebuildvba: clean vba

$(GBADEPS): $(CFILES) $(HFILES)
	@$(CC) $(CFLAGS) -MM $(CFILES) > $(GBADEPS)

-include $(GBADEPS)
