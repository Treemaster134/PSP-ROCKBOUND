TARGET = RockBound
OBJS = main.o \
gfx.o




CFLAGS = 
CXXFLAGS = $(CFLAGS) -std=c++14 -fno-rtti -g
ASFLAGS = $(CFLAGS) 



#PSP STUFF
PSP_FW_VERSION = 660
PSP_LARGE_MEMORY = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = RockBound 2
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.png
PSP_EBOOT_SND0 = SND0.at3

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak