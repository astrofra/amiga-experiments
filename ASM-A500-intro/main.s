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
;	Définition du Playfield
;*******************************************************************************
PF_WIDTH 						= 320
PF_HEIGHT 					= 256
PF_DEPTH 						= 1
PF_INTER 						= 0
PF_SIZE							= (PF_WIDTH/8)*PF_HEIGHT*PF_DEPTH
PF_MOD1							= (PF_WIDTH/8)*(PF_DEPTH-1)*PF_INTER
PF_MOD2							= (PF_WIDTH/8)*(PF_DEPTH-1)*PF_INTER

;*******************************************************************************
	SECTION PROGRAM,CODE
;*******************************************************************************

Start:
	jsr			SaveSystem										; Sauve les données système
	tst.l		d0
	beq			Restore												; Restaure en cas d'erreur

.Initialize:
	bsr			InitScreen										; Initialise l'écran
	bsr			InitCopper										; Initialise la Copper list

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

	btst		#MOUSE_BUTTON1,CIAA+CIAPRA		; Test la souris
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

;*******************************************************************************
	SECTION SCREEN,BSS_C
;*******************************************************************************
ScreenBuffer:
	ds.b		PF_SIZE

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
	CMOVE		$1200,BPLCON0									; Ecran 2 couleurs lowres
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
CLPalette:
	CMOVE		$0000,COLOR00
	CMOVE		$0FFF,COLOR01
CLEnd:
	CEND

;*******************************************************************************
; Fonctions utiles
;*******************************************************************************

	INCLUDE "System:Sources/includes/system.s"
	INCLUDE "System:Sources/includes/iff_tool.s"
;	INCLUDE "System:Sources/includes/mod_player.s"

	END
