;*******************************************************************************
;	Macro.s
;	Handful Macros
;	Original file by Flabrador.free.fr
;*******************************************************************************

; Copperlist Macros

; Transfert une donnée dans un registre (CMOVE value,register)
CMOVE	MACRO
	dc.w	(\2)&$0ffe,\1
	ENDM

; Wait for a beam position (CWAIT x,y)
CWAIT	MACRO
	dc.w	((\2)&$ff)<<8!((\1)&$fe)!1,$fffe
	ENDM

; Skip to the next instruction (CSKIP x,y)
CSKIP	MACRO
	dc.w	((\2)&$ff)<<8!((\1)&$fe)!1,$ffff
	ENDM

; End of Copper list (CEND)
CEND MACRO
	dc.l	$fffffffe
	dc.l	$fffffffe
	ENDM

; Wait for a specific raster line (RWAIT x,y)
RWAIT MACRO
.RasterWait\@:
	cmpi.w	#((\2)&$ff)<<8!((\1)&$ff),CUSTOM+VHPOSR
	blo.s		.RasterWait\@
	ENDM

; Allocate a memory buffer (ALLOCMEM size,memtype,adrsave,error)
ALLOCMEM MACRO
	move.l	$4.w,a6
	move.l	#\1,d0
	move.l	#\2,d1
	jsr			_AllocMem(a6)
	move.l	d0,\3
	beq			\4
	ENDM

; Free a memory buffer (FREEMEM size,adrsave)
FREEMEM MACRO
	move.l	$4.w,a6
	move.l	#\1,d0
	move.l	\2,a1
	jsr			_FreeMem(a6)
	ENDM
