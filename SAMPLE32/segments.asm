; declaration and ordering of driver segments
.386P
.SEQ

DDHEADER           SEGMENT DWORD PUBLIC USE16 'DATA'
DDHEADER           ENDS

CONST              SEGMENT DWORD PUBLIC USE32 'DATA'
CONST              ENDS

CONST2             SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2             ENDS

_DATA              SEGMENT DWORD PUBLIC USE32 'DATA'
_DATA              ENDS

_BSS               SEGMENT DWORD PUBLIC USE32 'BSS'
_BSS               ENDS

DGROUP             GROUP DDHEADER,CONST,CONST2,_DATA,_BSS

_TEXTSTART         SEGMENT DWORD PUBLIC USE16 'CODE'
                   PUBLIC _CodeStart
_CodeStart         LABEL BYTE
                   int 3
_TEXTSTART         ENDS

_TEXT16            SEGMENT DWORD PUBLIC USE16 'CODE'
_TEXT16            ENDS

_TEXT              SEGMENT DWORD PUBLIC USE32 'CODE'
_TEXT              ENDS

_TEXTEND           SEGMENT DWORD PUBLIC USE32 'CODE'
                   PUBLIC  _CodeEnd
_CodeEnd           LABEL BYTE
                   int 3
_TEXTEND           ENDS

CGROUP             GROUP _TEXTSTART,_TEXT16,_TEXT,_TEXTEND
                   EXTRN DriverEntry:NEAR
                   ; declare the 32-bit Strategy function as entry point
                   END DriverEntry