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

;*******************************************************************************
;	Logo playfield definition
;*******************************************************************************
PF_WIDTH 						= 320
PF_HEIGHT 					= 80
PF_DEPTH 						= 4
PF_INTER 						= 1
PF_SIZE							= (PF_WIDTH/8)*PF_HEIGHT*PF_DEPTH
PF_MOD1							= (PF_WIDTH/8)*(PF_DEPTH-1)*PF_INTER
PF_MOD2							= (PF_WIDTH/8)*(PF_DEPTH-1)*PF_INTER

; Logo image defintion
BGPIC_WIDTH					= 320
BGPIC_HEIGHT				= 80
BGPIC_DEPTH					= 4
BGPIC_SIZE					= (BGPIC_WIDTH/8)*BGPIC_HEIGHT*BGPIC_DEPTH

;*******************************************************************************
	SECTION PROGRAM,CODE
;*******************************************************************************

Start:
	jsr			SaveSystem										; Sauve les donn�es syst�me
	tst.l		d0
	beq			Restore												; Restaure en cas d'erreur

.Initialize:
	bsr			InitScreen										; Initialise l'�cran
	bsr			InitCopper										; Initialise la Copper list

	jsr			InitBackground	

.SetVBL:
	move.l	VbrBase,a6
	move.l	#VBL,VEC_VBL(a6)

.SetCopper:
	lea			CUSTOM,a6
	move.l	#CopperList,COP1LC(a6)				; Notre Copper list
	clr.w		COPJMP1(a6)										; Que l'on d�marre

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
	move.l	#ScreenBuffer,PhysicBase
	move.l	PhysicBase,a0
	move.l	#$33333333,d1
	move.w	#(PF_SIZE/4)-1,d0
.FillScreen:
	move.l	d1,(a0)+
	dbf			d0,.FillScreen	
	rts

;***************************************

InitCopper:
	lea			CLBitplaneAdr,a0							; Les bitplans de la CL
	move.l	PhysicBase,d0									; Notre �cran
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
	move.l	#DefaultSprite,d0							; Sprite vide par d�faut
	move.w	#SPR_MAXSPRITE-1,d1						; 8 sprites � faire
.SetSpritePtr:
	move.w	d0,6(a0)											; Adresse du sprite
	swap		d0
	move.w	d0,2(a0)											; Adresse du sprite
	swap		d0
	adda.l	#8,a0
	dbf			d1,.SetSpritePtr							; Sprite suivant
	rts

; Logo image initialisation
InitBackground:
	lea			Background,a0
;	lea			PaletteBuffer,a1
	lea			PictureBuffer,a2
	jsr			DecodePicture
	tst.l		d0
	beq			.DecodeError
	move.l	PhysicBase,a0
	move.w	#BGPIC_HEIGHT-1,d0						; On transfert notre image
.NextLine:
	move.w	#PF_DEPTH-1,d1								; Sur un �cran 3 plans double largeur
.NextPlan:
	move.w	#(BGPIC_WIDTH/32)-1,d2
.NextBlock:
	move.l	(a2)+,(a0)+
	dbf			d2,.NextBlock
;	move.l	a1,a0
	dbf			d1,.NextPlan
	dbf			d0,.NextLine
.DecodeError:
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
	move.w	#$20,CUSTOM+INTREQ						; Lib�re l'interruption

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

;*******************************************************************************
	SECTION SCREEN,BSS_C
;*******************************************************************************
ScreenBuffer:
	ds.b		PF_SIZE

PaletteBuffer:
	ds.l		16

PictureBuffer:
	ds.b		BGPIC_SIZE	

;*******************************************************************************
	SECTION SPRITE,DATA_C
;*******************************************************************************
DefaultSprite:
	dc.l		0,0,0,0

;*******************************************************************************
	SECTION COPPER,DATA_C
;*******************************************************************************
CopperList:
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
	CMOVE		$0038,DDFSTRT
	CMOVE		$00D0,DDFSTOP
	CMOVE		$4200,BPLCON0									; 100001000000000
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
	CMOVE		$0111,COLOR01
	CMOVE		$0222,COLOR02
	CMOVE		$0333,COLOR03
	CMOVE		$0444,COLOR04
	CMOVE		$0555,COLOR05
	CMOVE		$0666,COLOR06
	CMOVE		$0777,COLOR07
	CMOVE		$0888,COLOR08
	CMOVE		$0999,COLOR09
	CMOVE		$0AAA,COLOR10
	CMOVE		$0BBB,COLOR11
	CMOVE		$0CCC,COLOR12
	CMOVE		$0DDD,COLOR13
	CMOVE		$0EEE,COLOR14
	CMOVE		$0FFF,COLOR15

CLEnd:
	CEND

Background:
	INCBIN	"System:Sources/data/mandarine_logo.iff"	

;*******************************************************************************
; Fonctions utiles
;*******************************************************************************

	INCLUDE "System:Sources/includes/system.s"
	INCLUDE "System:Sources/includes/iff_tool.s"
;	INCLUDE "System:Sources/includes/mod_player.s"

	END
