; driver header and standard entry points

.386P
.SEQ



                   EXTRN Dos32FlatDS:ABS
                   EXTRN KernThunkStackTo16:near
                   EXTRN KernThunkStackTo32:near
                   EXTRN KernSelToFlat:near
                   EXTRN DriverEntry:near
                   EXTRN _CodeEnd:BYTE
                   EXTRN _end:BYTE


                   PUBLIC _DDHeader
                   PUBLIC _Device_Help


DDHEADER           SEGMENT DWORD PUBLIC USE16 'DATA'

_DDHeader          LABEL BYTE

   NextHdr         DD    -1
   DrvFlags        DW    8180h
   MainEntry       DW    _TEXT16:Strat16_init
   IDCEntry        DW    _TEXT16:IDC16
   DriverName      DB    "SAMPLE$ "
   ProtCS          DW    0
   ProtDS          DW    0
   RealCS          DW    0
   RealDS          DW    0
   Capabilities    DD    01Bh
   Rsvd            DW    0

   _Device_Help    DD    0

   CpuCount        DW    1

DDHEADER           ENDS


_TEXT16            SEGMENT DWORD PUBLIC USE16 'CODE'
                   ASSUME ds:DDHEADER
                   ; 16-bit Strategy entry point for Init call
   Strat16_init:
                   cmp   byte ptr es:[bx+2],00h
                   jne   Strat16
                   ; save DevHlp entry point
                   mov   eax, dword ptr es:[bx+14]
                   mov   _Device_Help, eax
                   ; patch the entry point
                   mov   ax, offset _TEXT16:Strat16
                   mov   MainEntry, ax
                   ; query the number of CPU in the system
                   push  bx
                   mov   al, 20        ; DHGETDOSV_TOTALCPUS
                   mov   dl, 24h       ; DevHlp_GetDosVar
                   call  dword ptr [_Device_Help]
                   jc    @F
                   push  ds
                   mov   ds, ax
                   mov   ax, [bx]
                   pop   ds
                   mov   CpuCount, ax
              @@:
                   pop   bx
                   ; return size of segments
                   mov   ax, offset _TEXT16:_CodeEnd
                   mov   word ptr es:[bx+14], ax
                   mov   ax, offset DDHEADER:_end
                   mov   word ptr es:[bx+16], ax
                   mov   ax, 0100h
                   mov   word ptr es:[bx+3], ax
                   retf

                   ; 16-bit strategy entry point for other calls
   Strat16:
                   jmp   far ptr FLAT:Strategy32

                   ; 16-bit IDC entry point, IDC calls not supported
   IDC16:
                   retf
                   ; 16-bit part of DevHelp thunk
   DHCall16:
                   push  ds
                   push  ax
                   mov   ax, SEG DDHEADER
                   mov   ds, ax
                   pop   ax
                   call  dword ptr [_Device_Help]
                   pop   ds
                   jmp   far ptr FLAT:DHCall32_ret


_TEXT16            ENDS

_TEXT              SEGMENT DWORD PUBLIC USE32 'CODE'

                   ASSUME ds:DDHEADER,es:NOTHING

                   ; 32-bit part of Strategy entry thunk
Strategy32         PROC
                   ; save registers
                   push  ds
                   push  es
                   push  ebx
                   push  ebp
                   ; store request packet pointer into ECX
                   mov   cx, es
                   shl   ecx, 16
                   mov   cx, bx
                   ; put it into stack (this is parameter for KernSelToFlat)
                   push  ecx
                   ; load FLAT selector into ES and DS
                   mov   ax, offset Dos32FlatDS
                   mov   es, ax
                   mov   ds, ax

                   ASSUME ds:FLAT,es:FLAT

                   ; thunk the stack
                   call KernThunkStackTo32

                   ASSUME ss:FLAT

                   ; convert request packet pointer to 0:32
                   call  KernSelToFlat
                   add   esp,4
                   ; push the parameter - 0:32 pointer to request packet
                   push  eax
                   ; call driver strategy function
                   call  DriverEntry
                   ; remove parameter from the stack
                   add   esp, 4
                   ; save return value (eax is changed by Stack thunking)
                   mov   ecx, eax
                   ; thunk the stack back to 16:16
                   call KernThunkStackTo16
                   ; restore registers
                   pop   ebp
                   pop   ebx
                   pop   es
                   pop   ds
                   ; put return value into request packet
                   mov   word ptr es:[bx+3], cx
                   ; 16-bit far return
                   DB    66h
                   retf
Strategy32         ENDP


                   PUBLIC DHCall32
                   ; 32-bit part of DevHelp thunk
DHCall32           PROC NEAR
                   ASSUME ds:FLAT
                   jmp   far ptr _TEXT16:DHCall16

   DHCall32_ret:
                   retn
DHCall32           ENDP

_TEXT              ENDS

                   END 