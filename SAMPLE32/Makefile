
BILDLVL = @$#$(%BLD_VENDOR):$(%BLD_VERSION)$#SAMPLE PDD

.BEFORE

%INCLUDE=$(%WATCOM)\H;$(%WATCOM)\H\OS2;$(%HOME)\H;
%LIB=$(%WATCOM)\LIB386\OS2;$(%HOME)\LIB;


.ERASE

.SUFFIXES:
.SUFFIXES: .lst .obj .c .asm

AFLAGS  = -q
ASM     = WASM $(AFLAGS)

#CFLAGS  = -q -bt=os2 -bd -ei -fpi87 -zp1 -d0 -fp6 -6r -mf -ox -oi -wx -zl -zm -zls -dDEBUG
CFLAGS  = -q -bt=os2 -bd -ei -fpi87 -zp1 -d0 -fp6 -6s -mf -ox -oi -wx -zl -zm -zls
CC      = WCC386 $(CFLAGS)

LFLAGS  = format os2 lx phys option quiet,nostub,mixed1632,map,align=4
LINK    = WLINK

LIBFLAGS = -fo -io
LIB     = WLIB  $(LIBFLAGS)

DIS     = WDIS /l

.obj.lst: 
        $(DIS) $*

.asm.obj: .AUTODEPEND
        $(ASM) $*.asm

.c.obj: .AUTODEPEND
        $(CC) $*.c


FILES = segments.obj entry.obj main.obj ioctl.obj initcomp.obj 

TARGET = sample

ALL: $(TARGET).sys 

$(TARGET).lnk: makefile
    @echo $(LFLAGS) > $(TARGET).lnk
    @echo option description '$(BILDLVL)'>> $(TARGET).lnk
    @echo order              >> $(TARGET).lnk
    @echo clname DATA        >> $(TARGET).lnk
    @echo segment DDHEADER   >> $(TARGET).lnk
    @echo clname CODE        >> $(TARGET).lnk
    @echo segment _TEXTSTART >> $(TARGET).lnk
    @echo segment _TEXT16    >> $(TARGET).lnk
    @echo segment _TEXT      >> $(TARGET).lnk
    @echo segment _TEXTEND   >> $(TARGET).lnk
    @echo name $(TARGET).sys >> $(TARGET).lnk
    @for %f in ($(FILES)) do @echo file %f >> $(TARGET).lnk
    @echo segment type DATA SHARED PRELOAD >> $(TARGET).lnk
    @echo segment class CODE PRELOAD          >> $(TARGET).lnk
    @echo IMPORT Dos32FlatDS DOSCALLS.370  >> $(TARGET).lnk
    @echo lib kee.lib,vdh.lib >> $(TARGET).lnk


$(TARGET).sys: $(TARGET).lnk $(FILES)
    $(LINK) @$(TARGET).lnk


$(TARGET).tdf: $(TARGET).sys
    wat2map $(TARGET).map $$$.map
    trcust  $(TARGET).tsf /M=$$$.map
    del $$$.map


