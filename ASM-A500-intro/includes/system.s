;*******************************************************************************
;	System.s
;	Save and restore the systelm
; 	Memory handler
; 	File handler
;	Original file by Flabrador.free.fr
;*******************************************************************************

;*******************************************************************************
	SECTION PROGRAM,CODE
;*******************************************************************************

;*******************************************************************************
; System save 
;*******************************************************************************
SaveSystem:
	movea.l	$4.w,a6												; Exec library

.StopMultiTask:
	jsr			_Forbid(a6)										;	Stopper le multitache

.CheckCPU:
	moveq.l	#0,d0
	btst.b	#0,EXEC_CPU_TYPE(a6)					; Check le type de CPU présent
	beq.s		.SaveVbr											; Si c'est un 68000 ne récupère pas le VBR

.GetVectorBase:
	lea			GetVBR,a5
	jsr			_Supervisor(a6)
.SaveVbr:
	move.l	d0,VbrBase										;	Récupérer le VBR

.OpenDOSLib:
	lea			DOSLib,a1
	move.l	#33,d0
	jsr			_OpenLibrary(a6)							; Ouvre la dos.library
	tst.l		d0
	beq			.EndInit											; Si on la trouve pas on quitte
	move.l	d0,DOSBase										; On garde le pointeur

.OpenGraphicsLib:
	lea			GraphicsLib,a1
	move.l	#33,d0
	jsr			_OpenLibrary(a6)							;	Ouvrir la graphics.library
	tst.l		d0
	beq			.EndInit											; Si on la trouve pas on quitte
	move.l	d0,GfxBase										; On garde le pointeur

.CheckForPAL:
	cmpi.l	#37,LIB_VERSION(a6)						; OS 2 ou +
	bge.b		.CheckOs20
	cmpi.b	#50,VB_FREQUENCY(a6)					; Fréquence 50Hz
	bne.s		.CheckForAGA
	move.w	#1,FlagPAL										; Oui c'est du PAL
	bra.s		.CheckForAGA

.CheckOs20:
	movea.l	GfxBase,a0										; Graphics base
	btst.b	#GFXB_PAL,GFX_DISPLAYFLAGS(a0)	; PAL bit
	beq.s		.CheckForAGA
	move.w	#1,FlagPAL										; Oui c'est du PAL

.CheckForAGA:
	movea.l	GfxBase,a6
	btst.b	#GFXB_AA_ALICE,GFX_CHIPREV(a6)	;	Test le chipset
	beq.s		.NotAGA												; Pas AGA
	move.w	#1,FlagAGA
.NotAGA:

.ReserveBlitter:
	jsr			_WaitBlit(a6)									;	Attendre la fin d'activité blitter
	jsr			_OwnBlitter(a6)								;	Réserver le blitter

.ResetView:
	move.l	GFX_ACTIVEVIEW(a6),SaveView		; Récupérer l'ancien view
	move.l	GFX_COPPERLIST(a6),SaveCopper ; Récupérer l'ancienne copper list
	movea.l	#0,a1
	jsr			_LoadView(a6)									;	Charger un view vide
	jsr			_WaitTOF(a6)
	jsr			_WaitTOF(a6)									; Deux fois pour les écrans entrelacés

.SaveInterrupts:
	movea.l	VbrBase,a0										;	Sauvegarder les registres des IT
	move.l	VEC_KBD(a0),SaveKeyboard			; Le clavier
	move.l	VEC_VBL(a0),SaveVbl						; La VBL

.StopInterrupts:
	move.w	CUSTOM+INTENAR,SaveIntena
	ori.w		#$C000,SaveIntena							; Sauve l'état des IT
	move.w	CUSTOM+DMACONR,SaveDmacon
	ori.w		#$8100,SaveDmacon							; Sauve l'état des DMA

.NoError:
	move.l	#-1,d0												; Init OK

.EndInit:
	rts

;*******************************************************************************
; Fetch VBR
;*******************************************************************************
GetVBR:
;	movec		vbr,d0												; Pour compilation en 68010 et +
	dc.l		$4E7A0801											; Opcode direct pour compilation en 68000
	rte

;*******************************************************************************
; System restore
;*******************************************************************************
RestoreSystem:
	tst.l		DOSBase
	beq			.CloseGraphicsLib							; Si on a pas réussi à ouvrir la dos lib

	tst.l		GfxBase
	beq			.NoRestore										; Si on a pas réussi à ouvrir la gfx lib

.RestoreInterrupts:
	move.w	#INT_STOP,CUSTOM+INTENA				; Stop les interruptions
	move.w	#INT_STOP,CUSTOM+INTREQ				; Stop les requests
	move.w	#DMA_STOP,CUSTOM+DMACON				; Stop le DMA
	move.w	SaveIntena,CUSTOM+INTENA			; Restaure les IT
	move.w	SaveDmacon,CUSTOM+DMACON			; Restaure le DMA

.RestoreVectors:
	move.l	VbrBase,a0										;	Restaurer les vecteurs
	move.l	SaveKeyboard,VEC_KBD(a0)			; Le clavier
	move.l	SaveVbl,VEC_VBL(a0)						; La VBL

.RestoreView:
	movea.l	GfxBase,a6
	move.l	SaveCopper,CUSTOM+COP1LC			;	Restaurer la Copperlist
	movea.l	SaveView,a1
	jsr			_LoadView(a6)									;	Restaurer la view
	jsr			_WaitTOF(a6)
	jsr			_WaitTOF(a6)									; Toujours deux fois pour les écrans entrelacés

.FreeBlitter:
	jsr			_WaitBlit(a6)
	jsr			_DisownBlitter(a6)						;	Libérer le blitter

.CloseGraphicsLib:
	movea.l	$4.w,a6
	movea.l	GfxBase,a1
	jsr			_CloseLibrary(a6)							;	Fermer la librairie	GRAPHICS

.CloseDOSLib:
	movea.l	$4.w,a6
	movea.l	DOSBase,a1
	jsr			_CloseLibrary(a6)							;	Fermer la librairie DOS

.NoRestore
	jsr			_Permit(a6)										;	Relancer le multitache
	rts

;*******************************************************************************
; Allouer de la mémoire
; d0 = taille du buffer à allouer
; d1 = type de mémoire à allouer (MEMF_PUBLIC, MEMF_CHIP, MEMF_FAST)
; => d0 = adresse du buffer alloué ou 0 si erreur
;*******************************************************************************
AllocMemory:
	ori.l 	#MEMF_CLEAR,d1
	movea.l $4.w,a6
	jsr     _AllocMem(a6)
	rts

;*******************************************************************************
; Libérer de la mémoire
; a1 = adresse du buffer à libérer
; d0 = taille du buffer à allouer
;*******************************************************************************
FreeMemory:
	movea.l $4.w,a6
	jsr     _FreeMem(a6)
	rts

;*******************************************************************************
; Charge un fichier dans un buffer
; a1 = nom du fichier à charger
; a2 = adresse du buffer
; => d0 = taille du fichier chargé ou 0 si erreur
;*******************************************************************************
LoadFile:
	movem.l d1-a6,-(sp)
	move.w	#INT_ON+INT_PORTS,CUSTOM+INTENA	; IT pour les IO
.OpenFile:
	move.l  a1,d1                         ; Fichier à ouvrir
	move.l  #MODE_OLDFILE,d2              ; Ouverture en lecture seule
	move.l  DOSBase,a6
	jsr     _Open(a6)
	tst.l   d0                            ; Ouverture OK
	beq     .OpenError                    ; Non
	move.l  d0,d7                         ; Sauve le file handler
.GetFileSize:
	move.l  d7,d1
	move.l  #0,d2
	move.l  #OFFSET_END,d3
	jsr     _Seek(a6)                     ; On se positionne à la fin du fichier
	move.l  d7,d1
	move.l  #0,d2
	move.l  #OFFSET_BEGINNING,d3
	jsr     _Seek(a6)                     ; On se positionne au début du fichier
	tst.l   d0                            ; Taille > à 0
	beq     .CloseFile
	move.l  d0,d6                         ; Sauve la taille du fichier
.ReadFile:
	move.l  d7,d1                         ; File handler
	move.l  a2,d2                         ; Buffer
	move.l  d6,d3                         ; Nombre d'octets à lire
	jsr     _Read(a6)                     ; Lit le fichier
	move.l  d0,d6
	tst.l   d0
	bgt.s   .CloseFile                    ; Pas d'erreur de lecture
	moveq.l #0,d6
.CloseFile:
	move.l  d7,d1
	jsr     _Close(a6)
	move.l  d6,d0
.OpenError:
	movem.l (sp)+,d1-a6
	rts

;*******************************************************************************
	SECTION	SYSTEM,DATA
;*******************************************************************************

GraphicsLib:
	dc.b		"graphics.library",0

DOSLib:
	dc.b		"dos.library",0

	EVEN

SaveIntena:
	dc.w		0
SaveDmacon:
	dc.w		0
FlagAGA:
	dc.w		0
FlagPAL:
	dc.w		0
VbrBase:
	dc.l		0
GfxBase:
	dc.l		0
DOSBase:
	dc.l		0
SaveKeyboard:
	dc.l		0
SaveVbl:
	dc.l		0
SaveView:
	dc.l		0
SaveCopper:
	dc.l		0
