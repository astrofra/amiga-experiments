;*******************************************************************************
; main.s
; Original demo ASM skeleton by Flabrador.free.fr 
;*******************************************************************************

; Registers definition
	INCLUDE	"System:Sources/includes/register.s"

; System constants
	INCLUDE	"System:Sources/includes/constant.s"

; Macros
	INCLUDE	"System:Sources/includes/macro.s"

; Enable DMA	
DMA_SET 						= DMA_ON+DMA_BITPLANE+DMA_COPPER

; Enable interruptions
INT_SET							= INT_ON+INT_VERTB

; AGA Burst
BURST_SET						= BURST_NONE

;	Plafields
;	Logo playfield definition ****************************************************
PF_WIDTH 						= 384
PF_WIDTH_DISPLAY 		= 320
PF_HEIGHT 					= 80
PF_DEPTH 						= 4
PF_INTER 						= 0
PF_SIZE							= (PF_WIDTH/8)*PF_HEIGHT*PF_DEPTH
PF_MOD1							= (PF_WIDTH-PF_WIDTH_DISPLAY)/8+PF_INTER*(PF_WIDTH*(PF_DEPTH-1)/8)-2
PF_MOD2							= (PF_WIDTH-PF_WIDTH_DISPLAY)/8+PF_INTER*(PF_WIDTH*(PF_DEPTH-1)/8)-2

;	Scrolltext playfield definition **********************************************
PF2_WIDTH 					= 320
PF2_HEIGHT 					= 16
PF2_DEPTH 					= 2
PF2_INTER 					= 0
PF2_SIZE						= (PF2_WIDTH/8)*PF2_HEIGHT*PF2_DEPTH
PF2_MOD1						= (PF2_WIDTH/8)*(PF2_DEPTH-1)*PF2_INTER
PF2_MOD2						= (PF2_WIDTH/8)*(PF2_DEPTH-1)*PF2_INTER

;	Checkboard playfield definition ****************************************************
PF3_WIDTH 					= 304
PF3_WIDTH_DISPLAY 	= 304
PF3_DISPLAY_HEIGHT 	= 140
PF3_STRIP_SIZE			=	10
PF3_HEIGHT 					= PF3_DISPLAY_HEIGHT*PF3_STRIP_SIZE
PF3_DEPTH 					= 1
PF3_INTER 					= 0
PF3_SIZE						= (PF3_WIDTH/8)*PF3_HEIGHT*PF3_DEPTH
PF3_MOD1						= (PF3_WIDTH-PF3_WIDTH_DISPLAY)/8+PF3_INTER*(PF3_WIDTH*(PF3_DEPTH-1)/8)-2
PF3_MOD2						= (PF3_WIDTH-PF3_WIDTH_DISPLAY)/8+PF3_INTER*(PF3_WIDTH*(PF3_DEPTH-1)/8)-2

;	Images
; Logo image definition *********************************************************
BGPIC_WIDTH					= 384
BGPIC_HEIGHT				= 80
BGPIC_DEPTH					= 4
BGPIC_SIZE					= (BGPIC_WIDTH/8)*BGPIC_HEIGHT*BGPIC_DEPTH

; Checkboard image definition *********************************************************
CHKPIC_WIDTH				= 304
CHKPIC_HEIGHT				= 1000
CHKPIC_DEPTH				= 1
CHKPIC_SIZE					= (CHKPIC_WIDTH/8)*CHKPIC_HEIGHT*CHKPIC_DEPTH

;*******************************************************************************
	SECTION PROGRAM,CODE
;*******************************************************************************

Start:
	jsr			SaveSystem										; Sauve les données système
	tst.l		d0
	beq			Restore												; Restaure en cas d'erreur

.Initialize:
	bsr			InitScreen										; Logo screen
	bsr			InitScreen2										; Scrolltext screen
	bsr			InitScreen3										; Checkerboard screen
	bsr			InitCopper										; Initialise la Copper list

;	jsr			DrawLogo
;	jsr			DrawCheckboard

.SetVBL:
	move.l	VbrBase,a6
	move.l	#VBL,VEC_VBL(a6)

.SetCopper:
	lea			CUSTOM,a6
	move.l	#CopperList,COP1LC(a6)				; Notre Copper list
	clr.w		COPJMP1(a6)										; Que l'on démarre

	tst.w		FlagAGA
	beq.s		.NoBurstMode
	move.w	#BURST_SET,FMODE(a6)					; Mode burst AGA
.NoBurstMode

.SetInterrupts:
	move.w	#INT_STOP,INTENA(a6)					; Stop les interruptions
	move.w	#INT_STOP,INTREQ(a6)					; Stop les requests
	move.w	#DMA_STOP,DMACON(a6)					; Stop le DMA
	move.w	#INT_SET,INTENA(a6)						; Interruptions VBL on
	move.w	#DMA_SET,DMACON(a6)						; Canaux DMA

;*******************************************************************************

MainLoop:
	move.w	#$0,FlagVBL
WaitVBL:
	tst.w		FlagVBL
	beq.s		WaitVBL												; On attend la vbl

	move.w 	#$F0F, CUSTOM+COLOR00

	btst		#MOUSE_BUTTON1,CIAA+CIAPRA		; Mouse button test
	bne.s		MainLoop

;*******************************************************************************

Restore:
	jsr			RestoreSystem
	
	moveq.l	#0,d0
	rts

;*******************************************************************************
;	Routines d'initialisation
;*******************************************************************************

InitScreen:
	move.l	#MandarineLogo,PhysicBase
	move.l	PhysicBase,a0
;	move.l	#$33333333,d1
;	move.w	#(PF_SIZE/4)-1,d0
;.FillScreen:
;	move.l	d1,(a0)+
;	dbf			d0,.FillScreen	
	rts

InitScreen2:
	move.l	#ScreenBuffer2,PhysicBase2
	move.l	PhysicBase2,a0
	move.l	#$CCCCCCCC,d1
	move.w	#(PF2_SIZE/4)-1,d0
.FillScreen:
	move.l	d1,(a0)+
	dbf			d0,.FillScreen	
	rts

InitScreen3:
	move.l	#Checkerboard,PhysicBase3
	move.l	PhysicBase3,a0
;	move.l	#$C3C3C3C3,d1
;	move.w	#(PF3_SIZE/4)-1,d0
;.FillScreen:
;	move.l	d1,(a0)+
;	dbf			d0,.FillScreen	
	rts	

;***************************************

InitCopper:
;	viewport 1
	lea			CLBitplaneAdr,a0							; Les bitplans de la CL
	move.l	PhysicBase,d0									; Notre écran
	move.w	#BPL1PT,d1										; Registre BP
	move.w	#PF_DEPTH-1,d7
.SetBplPointer:
	move.w	d1,(a0)+											; Adresse registre
	swap		d0
	move.w	d0,(a0)+											; Adresse bitplane
	addq.w	#2,d1													; Registre suivant
	move.w	d1,(a0)+											; Adresse registre
	swap		d0
	move.w	d0,(a0)+											; Adresse bitplane
	addq.w	#2,d1													; Registre suivant
	IFEQ		PF_INTER
	addi.l	#PF_WIDTH/8*PF_HEIGHT,d0			; Bitplane suivant (non interleave)
	ELSE
	addi.l	#PF_WIDTH/8,d0								; Bitplane suivant (interleave)
	ENDC
	dbf			d7,.SetBplPointer

	lea			CLSpriteAdr,a0								; Les sprites de la CL
	move.l	#DefaultSprite,d0							; Sprite vide par défaut
	move.w	#SPR_MAXSPRITE-1,d1						; 8 sprites à faire
.SetSpritePtr:
	move.w	d0,6(a0)											; Adresse du sprite
	swap		d0
	move.w	d0,2(a0)											; Adresse du sprite
	swap		d0
	adda.l	#8,a0
	dbf			d1,.SetSpritePtr							; Sprite suivant

;	viewport 2
	lea			CLBitplaneAdr2,a0							; Les bitplans de la CL
	move.l	PhysicBase2,d0								; Notre écran
	move.w	#BPL1PT,d1										; Registre BP
	move.w	#PF2_DEPTH-1,d7
.SetBplPointer2:
	move.w	d1,(a0)+											; Adresse registre
	swap		d0
	move.w	d0,(a0)+											; Adresse bitplane
	addq.w	#2,d1													; Registre suivant
	move.w	d1,(a0)+											; Adresse registre
	swap		d0
	move.w	d0,(a0)+											; Adresse bitplane
	addq.w	#2,d1													; Registre suivant
	IFEQ		PF2_INTER
	addi.l	#PF2_WIDTH/8*PF2_HEIGHT,d0		; Bitplane suivant (non interleave)
	ELSE
	addi.l	#PF2_WIDTH/8,d0								; Bitplane suivant (interleave)
	ENDC
	dbf			d7,.SetBplPointer2

;	viewport 3
	lea			CLBitplaneAdr3,a0							; Les bitplans de la CL
	move.l	PhysicBase3,d0								; Notre écran
	move.w	#BPL1PT,d1										; Registre BP
	move.w	#PF3_DEPTH-1,d7
.SetBplPointer3:
	move.w	d1,(a0)+											; Adresse registre
	swap		d0
	move.w	d0,(a0)+											; Adresse bitplane
	addq.w	#2,d1													; Registre suivant
	move.w	d1,(a0)+											; Adresse registre
	swap		d0
	move.w	d0,(a0)+											; Adresse bitplane
	addq.w	#2,d1													; Registre suivant
	IFEQ		PF3_INTER
	addi.l	#PF3_WIDTH/8*PF3_HEIGHT,d0		; Bitplane suivant (non interleave)
	ELSE
	addi.l	#PF3_WIDTH/8,d0								; Bitplane suivant (interleave)
	ENDC
	dbf			d7,.SetBplPointer3

	rts

;*******************************************************************************
;	Routines d'animation
;*******************************************************************************

;*******************************************************************************
;	Interruptions
;*******************************************************************************
VBL:
	movem.l	d0-a6,-(sp)

	move.w	#$FFFF,FlagVBL								; Indique la fin de la VBL
	move.w	#$20,CUSTOM+INTREQ						; Libère l'interruption

	movem.l	(sp)+,d0-a6
	rte
;*******************************************************************************

;*******************************************************************************
	SECTION	GENERAL,DATA
;*******************************************************************************

FlagVBL:
	dc.w		0

PhysicBase:
	dc.l		0 

PhysicBase2:
	dc.l		0 

PhysicBase3:
	dc.l		0	

;*******************************************************************************
	SECTION SCREEN,BSS_C
;*******************************************************************************
ScreenBuffer:
	ds.b		PF_SIZE
PaletteBuffer:
	ds.l		16

ScreenBuffer2:
	ds.b		PF2_SIZE
PaletteBuffer2:
	ds.l		4

ScreenBuffer3:
	ds.b		PF3_SIZE
PaletteBuffer3:
	ds.l		2		

PictureBuffer:
	ds.b		CHKPIC_SIZE

;*******************************************************************************
	SECTION SPRITE,DATA_C
;*******************************************************************************
DefaultSprite:
	dc.l		0,0,0,0

;*******************************************************************************
	SECTION COPPER,DATA_C
;*******************************************************************************
CopperList:
;	Viewport 1
	CMOVE		$2C81,DIWSTRT
	CMOVE		$2CC1,DIWSTOP
CLSpriteAdr:
	CMOVE		$0000,SPR0PTH
	CMOVE		$0000,SPR0PTL
	CMOVE		$0000,SPR1PTH
	CMOVE		$0000,SPR1PTL
	CMOVE		$0000,SPR2PTH
	CMOVE		$0000,SPR2PTL
	CMOVE		$0000,SPR3PTH
	CMOVE		$0000,SPR3PTL
	CMOVE		$0000,SPR4PTH
	CMOVE		$0000,SPR4PTL
	CMOVE		$0000,SPR5PTH
	CMOVE		$0000,SPR5PTL
	CMOVE		$0000,SPR6PTH
	CMOVE		$0000,SPR6PTL
	CMOVE		$0000,SPR7PTH
	CMOVE		$0000,SPR7PTL
CLScreenDef:
	CWAIT		$0001,$002A
	CMOVE		$0030,DDFSTRT
	CMOVE		$00D0,DDFSTOP
	CMOVE		$4200,BPLCON0
	CMOVE		$0000,BPLCON1
	CMOVE		$0000,BPLCON2
	CMOVE		$0C00,BPLCON3
	CMOVE		$0011,BPLCON4
	CMOVE		PF_MOD1,BPL1MOD
	CMOVE 	PF_MOD2,BPL2MOD
CLBitplaneAdr:
	REPT		PF_DEPTH
	CMOVE		$0000,$0000
	CMOVE		$0000,$0000
	ENDR
CLPaletteLogo:
	CMOVE		$0000,COLOR00
	CMOVE		$0E86,COLOR01
	CMOVE		$0FA4,COLOR02
	CMOVE		$0546,COLOR03
	CMOVE		$0435,COLOR04
	CMOVE		$0202,COLOR05
	CMOVE		$0313,COLOR06
	CMOVE		$0769,COLOR07
	CMOVE		$0FD0,COLOR08
	CMOVE		$0A7A,COLOR09
	CMOVE		$0413,COLOR10
	CMOVE		$0324,COLOR11
	CMOVE		$0514,COLOR12
	CMOVE		$0536,COLOR13
	CMOVE		$0FFF,COLOR14
	CMOVE		$0625,COLOR15

;	Viewport 2
CLScreenDef2:
	CWAIT		$0001,$002A+PF_HEIGHT
	CMOVE		$0000,BPLCON0
	CMOVE		$0000,BPLCON1	
	CWAIT		$0001,$002A+PF_HEIGHT+2
	CMOVE		$0030,DDFSTRT
	CMOVE		$00D0,DDFSTOP
	CMOVE		$2000,BPLCON0
	CMOVE		$0000,BPLCON1
	CMOVE		PF2_MOD1,BPL1MOD
	CMOVE 	PF2_MOD2,BPL2MOD	
CLBitplaneAdr2:
	REPT		PF2_DEPTH
	CMOVE		$0000,$0000
	CMOVE		$0000,$0000
	ENDR

;	Viewport 3
CLScreenDef3:
	CWAIT		$0001,$002A+PF_HEIGHT+PF2_HEIGHT+1
	CMOVE		$0000,BPLCON0
	CMOVE		$0000,BPLCON1
	CWAIT		$0001,$002A+PF_HEIGHT+PF2_HEIGHT+2
	CMOVE		$0030,DDFSTRT
	CMOVE		$00D0,DDFSTOP
	CMOVE		$1000,BPLCON0
	CMOVE		$0000,BPLCON1
	CMOVE		PF2_MOD1,BPL1MOD
	CMOVE 	PF2_MOD2,BPL2MOD	
CLBitplaneAdr3:
	REPT		PF3_DEPTH
	CMOVE		$0000,$0000
	CMOVE		$0000,$0000
	ENDR
CLPaletteCheckboard:
	CMOVE		$0000,COLOR00
	CMOVE		$0FFF,COLOR01	

CLEnd:
	CEND

	EVEN
MandarineLogo:
	INCBIN	"System:Sources/data/mandarine_logo.bin"

	EVEN
Checkerboard:
	INCBIN	"System:Sources/data/checkerboard_strip.bin"

;*******************************************************************************
; Fonctions utiles
;*******************************************************************************

	INCLUDE "System:Sources/includes/system.s"
;	INCLUDE "System:Sources/includes/iff_tool.s"
;	INCLUDE "System:Sources/includes/mod_player.s"

	END
