;**************************************************
;*    ----- Protracker V2.3B Playroutine -----	  *
;**************************************************
; Portable version for most assemblers. Doesn't use small data.
;
; Written by Frank Wille in 2013.
; Adapted by stingray for PC-relative mode, added branch size specifiers.
;
;
; Exported functions and variables:
;
; mt_install_cia(a6=CUSTOM, a0=AutoVecBase, d0=PALflag.b)
;   Install a CIA-B interrupt for calling mt_music.
;
; mt_remove_cia(a6=CUSTOM)
;   Remove CIA-B music interrupt and restore the old vector.
;
; mt_init(a6=CUSTOM, a0=TrackerModule, a1=Samples|NULL, d0=InitialSongPos)
;   Initialize a new module.
;   Reset speed to 6, tempo to 125 and start at the given position.
;   When a1 is NULL the samples are assumed to be stored after the patterns.
;   Master volume is at 64 (maximum).
;
; mt_end(a6=CUSTOM)
;   Stop playing current module.
;
; mt_soundfx(a6=CUSTOM, a0=SamplePointer,
;            d0=SampleLength, d1=SamplePeriod, d2=SampleVolume)
;   Request playing of an external sound effect on the most unused channel.
;   This channel will be blocked for music until the effect has finished.
;
; mt_mastervol(d0=MasterVolume)
;   Set a master volume from 0 to 64 for all music channels.
;   Note that the master volume does not affect the volume of external
;   sound effects (which is desired).
;
; mt_music(a6=CUSTOM)
;   The replayer routine. Is automatically called after mt_install_cia().
;
; mt_Enable
;   Set this byte to non-zero to play music, zero to pause playing.
;
; mt_E8Trigger
;   This byte reflects the value of the last E8 command.
;   It is reset to 0 after mt_init().


; Audio channel registers
AUDLC		equ	0
AUDLEN		equ	4
AUDPER		equ	6
AUDVOL		equ	8


; Channel Status
		rsreset
n_note		rs.w	1
n_cmd		rs.b	1
n_cmdlo 	rs.b	1
n_start 	rs.l	1
n_loopstart	rs.l	1
n_length	rs.w	1
n_replen	rs.w	1
n_period	rs.w	1
n_volume	rs.w	1
n_pertab	rs.l	1
n_dmabit	rs.w	1
n_noteoff	rs.w	1
n_toneportspeed rs.w	1
n_wantedperiod	rs.w	1
n_pattpos	rs.w	1
n_funk		rs.w	1
n_wavestart	rs.l	1
n_reallength	rs.w	1
n_intbit	rs.w	1
n_audreg	rs.w	1
n_minusft	rs.b	1
n_vibratoamp	rs.b	1
n_vibratospd	rs.b	1
n_vibratopos	rs.b	1
n_vibratoctrl	rs.b	1
n_tremoloamp	rs.b	1
n_tremolospd	rs.b	1
n_tremolopos	rs.b	1
n_tremoloctrl	rs.b	1
n_gliss		rs.b	1
n_sampleoffset	rs.b	1
n_loopcount	rs.b	1
n_funkoffset	rs.b	1
n_retrigcount	rs.b	1
n_blocked	rs.b	1
n_freecnt	rs.b	1
n_reserved	rs.b	2
n_sizeof	rs.b	0


;---------------------------------------------------------------------------
	xdef	mt_install_cia
mt_install_cia:
; Install a CIA-B interrupt for calling mt_music.
; a6 = CUSTOM
; a0 = AutoVecBase
; d0 = PALflag.b (0 is NTSC)

	move.l	a4,-(sp)
	lea	mt_chan1(pc),a4
	clr.b	mt_Enable-mt_chan1(a4)

	lea	mt_Lev6Int(pc),a1
	lea	$78(a0),a0		; Level 6 interrupt vector
	move.l	a0,(a1)

	; remember level 6 interrupt enable
	move.w	#$2000,d1
	and.w	INTENAR(a6),d1
	or.w	#$8000,d1
	move.w	d1,mt_Lev6Ena-mt_chan1(a4)

	; disable level 6 EXTER interrupts, set player interrupt vector
	move.w	#$2000,INTENA(a6)
	move.l	(a0),mt_oldLev6-mt_chan1(a4)
	lea	mt_TimerAInt(pc),a1
	move.l	a1,(a0)

	; disable CIA-B interrupts, stop and save all timers
	lea	CIAB,a0
	move.b	#$7f,CIAICR(a0)
	move.b	#$10,CIACRA(a0)
	move.b	#$10,CIACRB(a0)
	lea	mt_oldtimers-mt_chan1(a4),a1
	move.b	CIATALO(a0),(a1)+
	move.b	CIATAHI(a0),(a1)+
	move.b	CIATBLO(a0),(a1)+
	move.b	CIATBHI(a0),(a1)

	; determine if 02 clock for timers is based on PAL or NTSC
	tst.b	d0
	bne.b	.1
	move.l	#1789773,d0		; NTSC
	bra.b	.2
.1:	move.l	#1773447,d0		; PAL
.2:	move.l	d0,mt_timerval-mt_chan1(a4)

	; load TimerA in continuous mode for the default tempo of 125
	divu	#125,d0
	move.b	d0,CIATALO(a0)
	lsr.w	#8,d0
	move.b	d0,CIATAHI(a0)
	move.b	#$11,CIACRA(a0)		; load timer, start continuous

	; load TimerB with 496 ticks for setting DMA and repeat
	move.b	#496&255,CIATBLO(a0)
	move.b	#496>>8,CIATBHI(a0)

	; TimerA and TimerB interrupt enable
	move.b	#$83,CIAICR(a0)

	; enable level 6 interrupts
	move.w	#$a000,INTENA(a6)

	move.l	(sp)+,a4
	rts

mt_Lev6Int:
	dc.l	0


;---------------------------------------------------------------------------
	xdef	mt_remove_cia
mt_remove_cia:
; Remove CIA-B music interrupt and restore the old vector.
; a6 = CUSTOM

	move.l	a4,-(sp)
	lea	mt_chan1(pc),a4

	; disable level 6 and CIA-B interrupts
	lea	CIAB,a0
	move.b	#$7f,CIAICR(a0)
	move.w	#$2000,INTENA(a6)

	; restore old timer values
	lea	mt_oldtimers-mt_chan1(a4),a1
	move.b	(a1)+,CIATALO(a0)
	move.b	(a1)+,CIATAHI(a0)
	move.b	(a1)+,CIATBLO(a0)
	move.b	(a1),CIATBHI(a0)
	move.b	#$10,CIACRA(a0)
	move.b	#$10,CIACRB(a0)

	; restore original level 6 interrupt vector
	move.l	mt_Lev6Int(pc),a1
	move.l	mt_oldLev6-mt_chan1(a4),(a1)

	; reeanable CIA-B ALRM interrupt, which was set by AmigaOS
	move.b	#$84,CIAICR(a0)

	; reenable previous level 6 interrupt
	move.w	mt_Lev6Ena-mt_chan1(a4),INTENA(a6)

	move.l	(sp)+,a4
	rts


;---------------------------------------------------------------------------
mt_TimerAInt:
; TimerA interrupt calls mt_music at a selectable tempo (Fxx command),
; which defaults to 50 times per second.

	movem.l	d0-d7/a0-a6,-(sp)
	lea	mt_chan1(pc),a4
	lea	CUSTOM,a6

	; clear EXTER interrupt flag
	move.w	#$2000,INTREQ(a6)

	; check and clear CIAB interrupt flags
	btst	#0,CIAB+CIAICR
	beq.b	.1

	; it was a TA interrupt, do music when enabled
	tst.b	mt_Enable-mt_chan1(a4)
	beq.b	.1

	bsr.w	mt_music

.1:	movem.l	(sp)+,d0-d7/a0-a6
	nop
	rte


;---------------------------------------------------------------------------
mt_TimerBdmaon:
; One-shot TimerB interrupt to enable audio DMA after 496 ticks.

	move.l	a0,-(sp)

	; check and clear CIAB interrupt flags
	btst	#1,CIAB+CIAICR
	beq.b	.1

	; it was a TB interrupt, restart timer to set repeat, enable DMA
	move.b	#$19,CIAB+CIACRB
	move.l	mt_Lev6Int(pc),a0
	move.l	#mt_TimerBsetrep,(a0)

	; clear EXTER interrupt flag
	lea	CUSTOM+INTREQ,a0
	move.w	#$2000,(a0)

	; enable audio DMA
	move.w	mt_dmaon(pc),DMACON-INTREQ(a0)

	move.l	(sp)+,a0
	nop
	rte

	; clear EXTER interrupt flag
.1:	lea	CUSTOM+INTREQ,a0
	move.w	#$2000,(a0)
	move.w	#$2000,(a0)

	move.l	(sp)+,a0
	nop
	rte

mt_dmaon:
	dc.w	$8000


;---------------------------------------------------------------------------
mt_TimerBsetrep:
; One-shot TimerB interrupt to set repeat samples after another 496 ticks.

	movem.l	d0/a4/a6,-(sp)

	lea	CUSTOM,a6
	moveq	#0,d0

	; check and clear CIAB interrupt flags
	btst	#1,CIAB+CIAICR
	beq.b	.1

	; it was a TB interrupt, set repeat sample pointers and lengths
	lea	mt_chan1(pc),a4
	move.l	0*n_sizeof+n_loopstart(a4),AUD0LC(a6)
	move.w	0*n_sizeof+n_replen(a4),AUD0LEN(a6)
	move.l	1*n_sizeof+n_loopstart(a4),AUD1LC(a6)
	move.w	1*n_sizeof+n_replen(a4),AUD1LEN(a6)
	move.l	2*n_sizeof+n_loopstart(a4),AUD2LC(a6)
	move.w	2*n_sizeof+n_replen(a4),AUD2LEN(a6)
	move.l	3*n_sizeof+n_loopstart(a4),AUD3LC(a6)
	move.w	3*n_sizeof+n_replen(a4),AUD3LEN(a6)

	; restore TimerA music interrupt vector
	move.l	mt_Lev6Int(pc),a4
	move.l	#mt_TimerAInt,(a4)

	; audio interrupt flags to clear, for new samples
	move.b	mt_dmaon+1(pc),d0
	lsl.w	#7,d0

	; clear EXTER and possible audio interrupt flags
.1:	or.w	#$2000,d0
	move.w	d0,INTREQ(a6)

	movem.l	(sp)+,d0/a4/a6
	nop
	rte


;---------------------------------------------------------------------------
	xdef	mt_init
mt_init:
; Initialize new module.
; Reset speed to 6, tempo to 125 and start at first position.
; Master volume is at 64 (maximum).
; a6 = CUSTOM
; a0 = module pointer
; a1 = sample pointer (NULL means samples are stored within the module)
; d0 = initial song position

	movem.l	d2/a2/a4,-(sp)
	lea	mt_chan1(pc),a4
	move.l	a0,mt_mod-mt_chan1(a4)

	; set initial song position
	cmp.b	950(a0),d0
	blo	.1
	moveq	#0,d0
.1:	move.b	d0,mt_SongPos-mt_chan1(a4)

	move.l	a1,d0		; sample data location is given?
	bne	.4

	; get number of highest pattern
	lea	952(a0),a1	; song arrangement list
	moveq	#127,d0
	moveq	#0,d2
.2:	move.b	(a1)+,d1
	cmp.b	d2,d1
	bls.b	.3
	move.b	d1,d2
.3:	dbf	d0,.2
	addq.b	#1,d2		; number of patterns

	; now we can calculate the base address of the sample data
	moveq	#10,d0
	asl.l	d0,d2
	add.l	#1084,d2
	lea	(a0,d2.l),a1

	; save start address of each sample
.4:	lea	mt_SampleStarts-mt_chan1(a4),a2
	moveq	#31-1,d0
.5:	move.l	a1,(a2)+
	moveq	#0,d1
	move.w	42(a0),d1
	add.l	d1,d1
	add.l	d1,a1
	lea	30(a0),a0
	dbf	d0,.5

	; disable the filter
	or.b	#2,CIAA+CIAPRA

	; reset speed and counters
	move.b	#6,mt_Speed-mt_chan1(a4)
	clr.b	mt_Counter-mt_chan1(a4)
	clr.w	mt_PatternPos-mt_chan1(a4)

	; set master volume to 64
	lea	MasterVolTab64(pc),a0
	move.l	a0,mt_MasterVolTab-mt_chan1(a4)

	; reset CIA timer A to default (125)
	move.l	mt_timerval-mt_chan1(a4),d0
	divu	#125,d0
	move.b	d0,CIAB+CIATALO
	lsr.w	#8,d0
	move.b	d0,CIAB+CIATAHI

	; initialise channel DMA, interrupt bits and audio register base
	move.w	#$0001,0*n_sizeof+n_dmabit(a4)
	move.w	#$0002,1*n_sizeof+n_dmabit(a4)
	move.w	#$0004,2*n_sizeof+n_dmabit(a4)
	move.w	#$0008,3*n_sizeof+n_dmabit(a4)
	move.w	#$0080,0*n_sizeof+n_intbit(a4)
	move.w	#$0100,1*n_sizeof+n_intbit(a4)
	move.w	#$0200,2*n_sizeof+n_intbit(a4)
	move.w	#$0400,3*n_sizeof+n_intbit(a4)
	move.w	#AUD0LC,0*n_sizeof+n_audreg(a4)
	move.w	#AUD1LC,1*n_sizeof+n_audreg(a4)
	move.w	#AUD2LC,2*n_sizeof+n_audreg(a4)
	move.w	#AUD3LC,3*n_sizeof+n_audreg(a4)

	; make sure n_period doesn't start as 0
	move.w	#320,d0
	move.w	d0,0*n_sizeof+n_period(a4)
	move.w	d0,1*n_sizeof+n_period(a4)
	move.w	d0,2*n_sizeof+n_period(a4)
	move.w	d0,3*n_sizeof+n_period(a4)

	clr.b	mt_E8Trigger-mt_chan1(a4)
	movem.l	(sp)+,d2/a2/a4


;---------------------------------------------------------------------------
	xdef	mt_end
mt_end:
; Stop playing current module.
; a6 = CUSTOM

	lea	mt_Enable(pc),a0
	clr.b	(a0)
	moveq	#0,d0
	move.w	d0,AUD0VOL(a6)
	move.w	d0,AUD1VOL(a6)
	move.w	d0,AUD2VOL(a6)
	move.w	d0,AUD3VOL(a6)
	move.w	#$000f,DMACON(a6)
	rts


;---------------------------------------------------------------------------
	xdef	mt_soundfx
mt_soundfx:
; Request playing of an external sound effect on the most unused channel.
; This channel will be blocked for music until the effect has finished.
; a6 = CUSTOM
; a0 = sample pointer
; d0 = sample length in words
; d1 = sample period
; d2 = sample volume

	lea	mt_chan1(pc),a1
	move.w	#$4000,INTENA(a6)
	move.l	a0,sfx_ptr-mt_chan1(a1)
	movem.w	d0-d2,sfx_len-mt_chan1(a1)
	move.w	#$c000,INTENA(a6)
	rts


;---------------------------------------------------------------------------
	xdef	mt_mastervol
mt_mastervol:
; Set a master volume from 0 to 64 for all music channels.
; Note that the master volume does not affect the volume of external
; sound effects (which is desired).
; d0 = master volume

	; stingray, since each volume table has a size of 65 bytes
	; we simply multiply (optimised of course) by 65 to get the
	; offset to the correct table
	lea	MasterVolTab0(pc),a0
	lea	mt_MasterVolTab(pc),a1
	add.w	d0,a0
	lsl.w	#6,d0
	add.w	d0,a0
	move.l	a0,(a1)
	rts


;---------------------------------------------------------------------------
	xdef	mt_music
mt_music:
; Called from interrupt.
; Play next position when Counter equals Speed.
; Effects are always handled.
; a6 = CUSTOM
; a4 = pointer to mt_chan1

	moveq	#0,d7			; d7 is always zero

	lea	mt_dmaon+1(pc),a0
	move.b	d7,(a0)

	; play external sound effect on a suitable channel, when given
	tst.w	sfx_len-mt_chan1(a4)
	beq.b	.1
	bsr.w	mt_insertsfx
	move.w	d7,sfx_len-mt_chan1(a4)

.1:	addq.b	#1,mt_Counter-mt_chan1(a4)

	move.b	mt_Counter-mt_chan1(a4),d0
	cmp.b	mt_Speed-mt_chan1(a4),d0
	blo.b	no_new_note

	; handle a new note
	move.b	d7,mt_Counter-mt_chan1(a4)
	tst.b	mt_PattDelTime2-mt_chan1(a4)
	beq.w	get_new_note

	; we have a pattern delay, check effects then step
	lea	AUD0LC(a6),a5
	lea	0*n_sizeof(a4),a2
	bsr.w	mt_checkfx
	lea	AUD1LC(a6),a5
	lea	1*n_sizeof(a4),a2
	bsr.w	mt_checkfx
	lea	AUD2LC(a6),a5
	lea	2*n_sizeof(a4),a2
	bsr.w	mt_checkfx
	lea	AUD3LC(a6),a5
	lea	3*n_sizeof(a4),a2
	bsr.w	mt_checkfx

	; set one-shot TimerB interrupt for enabling DMA, when needed
	move.b	mt_dmaon+1(pc),d0
	beq.w	pattern_step

	move.l	mt_Lev6Int(pc),a0
	lea	mt_TimerBdmaon(pc),a1
	move.l	a1,(a0)
	move.b	#$19,CIAB+CIACRB	; load/start timer B, one-shot
	bra.w	pattern_step

no_new_note:
	; just check effects
	lea	AUD0LC(a6),a5
	lea	0*n_sizeof(a4),a2
	bsr.w	mt_checkfx
	lea	AUD1LC(a6),a5
	lea	1*n_sizeof(a4),a2
	bsr.w	mt_checkfx
	lea	AUD2LC(a6),a5
	lea	2*n_sizeof(a4),a2
	bsr.w	mt_checkfx
	lea	AUD3LC(a6),a5
	lea	3*n_sizeof(a4),a2
	bsr.w	mt_checkfx

	; set one-shot TimerB interrupt for enabling DMA, when needed
	move.b	mt_dmaon+1(pc),d0
	beq.w	same_pattern

	move.l	mt_Lev6Int(pc),a0
	lea	mt_TimerBdmaon(pc),a1
	move.l	a1,(a0)
	move.b	#$19,CIAB+CIACRB	; load/start timer B, one-shot
	bra.w	same_pattern

get_new_note:
	; determine pointer to current pattern line
	move.l	mt_mod-mt_chan1(a4),a0
	lea	12(a0),a3		; sample info table
	lea	1084(a0),a1		; pattern data
	lea	952(a0),a0
	moveq	#0,d0
	move.b	mt_SongPos-mt_chan1(a4),d0
	move.b	(a0,d0.w),d0		; current pattern number
	swap	d0
	lsr.l	#6,d0
	add.l	d0,a1			; pattern base
	add.w	mt_PatternPos-mt_chan1(a4),a1	; a1 pattern line

	; play new note for each channel, apply some effects
	lea	AUD0LC(a6),a5
	lea	0*n_sizeof(a4),a2
	bsr.w	mt_playvoice
	lea	AUD1LC(a6),a5
	lea	1*n_sizeof(a4),a2
	bsr.w	mt_playvoice
	lea	AUD2LC(a6),a5
	lea	2*n_sizeof(a4),a2
	bsr.w	mt_playvoice
	lea	AUD3LC(a6),a5
	lea	3*n_sizeof(a4),a2
	bsr.w	mt_playvoice

	; set one-shot TimerB interrupt for enabling DMA, when needed
	move.b	mt_dmaon+1(pc),d0
	beq.b	pattern_step

	move.l	mt_Lev6Int(pc),a0
	lea	mt_TimerBdmaon(pc),a1
	move.l	a1,(a0)
	move.b	#$19,CIAB+CIACRB	; load/start timer B, one-shot

pattern_step:
	; next pattern line, handle delay and break
	moveq	#16,d2			; offset to next pattern line

	move.b	mt_PattDelTime2-mt_chan1(a4),d1
	move.b	mt_PattDelTime-mt_chan1(a4),d0
	beq.b	.1
	move.b	d0,d1
	move.b	d7,mt_PattDelTime-mt_chan1(a4)
.1:	tst.b	d1
	beq.b	.3
	subq.b	#1,d1
	beq.b	.2
	moveq	#0,d2			; do not advance to next line
.2:	move.b	d1,mt_PattDelTime2-mt_chan1(a4)

.3:	add.w	mt_PatternPos-mt_chan1(a4),d2	; d2 PatternPos

	; check for break
	bclr	#0,mt_PBreakFlag-mt_chan1(a4)
	beq.b	.4
	move.w	mt_PBreakPos-mt_chan1(a4),d2
	move.w	d7,mt_PBreakPos-mt_chan1(a4)

	; check whether end of pattern is reached
.4:	move.w	d2,mt_PatternPos-mt_chan1(a4)
	cmp.w	#1024,d2
	blo.b	same_pattern

song_step:
	move.w	mt_PBreakPos-mt_chan1(a4),mt_PatternPos-mt_chan1(a4)
	move.w	d7,mt_PBreakPos-mt_chan1(a4)
	move.b	d7,mt_PosJumpFlag-mt_chan1(a4)

	; next position in song
	moveq	#1,d0
	add.b	mt_SongPos-mt_chan1(a4),d0
	and.w	#$007f,d0
	move.l	mt_mod-mt_chan1(a4),a0
	cmp.b	950(a0),d0		; end of song reached?
	blo.b	.1
	moveq	#0,d0
.1:	move.b	d0,mt_SongPos-mt_chan1(a4)

same_pattern:
	tst.b	mt_PosJumpFlag-mt_chan1(a4)
	bne.b	song_step

	rts


mt_insertsfx:
; Find a suitable channel for an external sound effect.
; Then load hardware registers and block this channel.
; The sound effects system expects that all samples start with two 0 bytes
; to be used as repeat.
; a6 = CUSTOM
; -> d7 = 0 (reset to 0, as it is used here)

	; reset counters for free positions on a channel
	move.b	d7,0*n_sizeof+n_freecnt(a4)
	move.b	d7,1*n_sizeof+n_freecnt(a4)
	move.b	d7,2*n_sizeof+n_freecnt(a4)
	move.b	d7,3*n_sizeof+n_freecnt(a4)

	; We will prefer a channel which had an audio interrupt, because
	; that means the last instrument sample has been played completely
	; and the channel is now in the repeat loop.
	move.w	#$0780,d1
	and.w	INTREQR(a6),d1
	bne.b	.1

	; all channels busy, then it doesn't matter which one we break...
	move.w	#$0780,d1

	; a true in d4..d7 means that we won't use channel 0..3
.1:	btst	#7,d1
	seq	d4
	btst	#8,d1
	seq	d5
	btst	#9,d1
	seq	d6
	btst	#10,d1
	seq	d7

	; also ignore already blocked channels
	or.b	0*n_sizeof+n_blocked(a4),d4
	or.b	1*n_sizeof+n_blocked(a4),d5
	or.b	2*n_sizeof+n_blocked(a4),d6
	or.b	3*n_sizeof+n_blocked(a4),d7

	; Look at the next 8 pattern steps to find the longest sequence
	; of silence (no new note or instrument).
	moveq	#8,d2
	move.l	#$fffff000,d3		; mask to ignore effects

	; get pattern pointer
	move.l	mt_mod-mt_chan1(a4),a3		; a3 mod pointer
	lea	1084(a3),a1
	lea	952(a3),a0		; a0 song arrangement list
	move.w	mt_PatternPos-mt_chan1(a4),d1
	moveq	#0,d0
	move.b	mt_SongPos-mt_chan1(a4),d0
.2:	move.b	(a0,d0.w),d0
	swap	d0
	lsr.l	#6,d0
	add.l	d0,a1
	lea	1024(a1),a2		; a2 end of pattern
	add.w	d1,a1			; a1 current pattern pos

.3:	moveq	#4,d0

	move.l	(a1)+,d1
	tst.b	d4
	bne.b	.4
	addq.b	#1,0*n_sizeof+n_freecnt(a4)
	and.l	d3,d1
	sne	d4
.4:	add.b	d4,d0

	move.l	(a1)+,d1
	tst.b	d5
	bne.b	.5
	addq.b	#1,1*n_sizeof+n_freecnt(a4)
	and.l	d3,d1
	sne	d5
.5:	add.b	d5,d0

	move.l	(a1)+,d1
	tst.b	d6
	bne.b	.6
	addq.b	#1,2*n_sizeof+n_freecnt(a4)
	and.l	d3,d1
	sne	d6
.6:	add.b	d6,d0

	move.l	(a1)+,d1
	tst.b	d7
	bne.b	.7
	addq.b	#1,3*n_sizeof+n_freecnt(a4)
	and.l	d3,d1
	sne	d7
.7:	add.b	d7,d0

	; break, when only a single or no channel is available at this point
	subq.b	#2,d0
	bmi.b	.10

	; otherwise break after 8 pattern steps
	subq.w	#1,d2
	beq.b	.10

	; End of pattern reached? Then load next pattern pointer.
	cmp.l	a2,a1
	blo.b	.3
	moveq	#0,d1
	moveq	#1,d0
	add.b	mt_SongPos-mt_chan1(a4),d0
	and.w	#$007f,d0
	cmp.b	950(a3),d0		; end of song reached?
	blo.b	.2
	moveq	#0,d0
	bra.b	.2

	; We have one or more valid channels. Compare their freecnt.
	; Default to the last channel when all are the same.
.10:	moveq	#0,d0

	lea	0*n_sizeof+n_freecnt(a4),a0
	cmp.b	(a0),d0
	bhi.b	.11
	move.l	a0,a2
	move.b	(a0),d0
.11:	lea	1*n_sizeof+n_freecnt(a4),a0
	cmp.b	(a0),d0
	bhi.b	.12
	move.l	a0,a2
	move.b	(a0),d0
.12:	lea	2*n_sizeof+n_freecnt(a4),a0
	cmp.b	(a0),d0
	bhi.b	.13
	move.l	a0,a2
	move.b	(a0),d0
.13:	lea	3*n_sizeof+n_freecnt(a4),a0
	cmp.b	(a0),d0
	bhi.b	.14
	move.l	a0,a2

	; Set the channel to blocked and start the sound sample.
.14:	st	n_blocked-n_freecnt(a2)

	move.w	n_dmabit-n_freecnt(a2),d0
	move.w	d0,DMACON(a6)

	move.l	a6,a0
	add.w	n_audreg-n_freecnt(a2),a0
	move.l	sfx_ptr-mt_chan1(a4),a1
	move.l	a1,(a0)+
	move.w	sfx_len-mt_chan1(a4),(a0)+
	move.w	sfx_per-mt_chan1(a4),d1
	move.w	d1,(a0)+
	move.w	sfx_vol-mt_chan1(a4),(a0)

	; save repeat and period for later
	move.l	a1,n_loopstart-n_freecnt(a2)
	move.w	#1,n_replen-n_freecnt(a2)
	move.w	d1,n_period-n_freecnt(a2)

	lea	mt_dmaon(pc),a0
	or.w	d0,(a0)

	moveq	#0,d7			; restore d7
	rts


mt_checkfx:
; a2 = channel data
; a5 = audio registers

	tst.b	n_blocked(a2)
	beq.b	.1

	; channel is blocked, only check some E-commands
	move.w	#$0fff,d4
	and.w	n_cmd(a2),d4
	move.w	d4,d0
	clr.b	d0
	cmp.w	#$0e00,d0
	bne.b	mt_nop
	and.w	#$00ff,d4
	bra.w	blocked_e_cmds

.1:	move.w	n_funk(a2),d0
	beq.b	.2
	bsr.w	mt_updatefunk

.2:	move.w	#$0fff,d4
	and.w	n_cmd(a2),d4
	beq.b	mt_pernop
	and.w	#$00ff,d4

	moveq	#$0f,d0
	and.b	n_cmd(a2),d0
	add.w	d0,d0
	move.w	fx_tab(pc,d0.w),d0
	jmp	fx_tab(pc,d0.w)

fx_tab:
	dc.w	mt_arpeggio-fx_tab	; $0
	dc.w	mt_portaup-fx_tab
	dc.w	mt_portadown-fx_tab
	dc.w	mt_toneporta-fx_tab
	dc.w	mt_vibrato-fx_tab	; $4
	dc.w	mt_tonevolslide-fx_tab
	dc.w	mt_vibrvolslide-fx_tab
	dc.w	mt_tremolo-fx_tab
	dc.w	mt_nop-fx_tab		; $8
	dc.w	mt_nop-fx_tab
	dc.w	mt_volumeslide-fx_tab
	dc.w	mt_nop-fx_tab
	dc.w	mt_nop-fx_tab		; $C
	dc.w	mt_nop-fx_tab
	dc.w	mt_e_cmds-fx_tab
	dc.w	mt_nop-fx_tab


mt_pernop:
; just set the current period

	move.w	n_period(a2),AUDPER(a5)
mt_nop:
	rts


mt_playvoice:
; a1 = pattern ptr
; a2 = channel data
; a3 = sample info table
; a5 = audio registers

	move.l	(a1)+,d6		; d6 current note/cmd words

	; channel blocked by external sound effect?
	tst.b	n_blocked(a2)
	beq.b	.1

	; do only some limited commands, while sound effect is in progress
	move.w	n_intbit(a2),d0
	and.w	INTREQR(a6),d0
	beq.w	moreblockedfx
	move.w	n_dmabit(a2),d0
	and.w	mt_dmaon(pc),d0
	bne.w	moreblockedfx

	; sound effect sample has played, so unblock this channel again
	move.b	d7,n_blocked(a2)

.1:	tst.l	(a2)			; any note or cmd set?
	bne.b	.2
	move.w	n_period(a2),AUDPER(a5)
.2:	move.l	d6,(a2)

	moveq	#15,d5
	and.b	n_cmd(a2),d5
	add.w	d5,d5			; d5 cmd*2

	moveq	#0,d4
	move.b	d6,d4			; d4 cmd argument (in MSW)
	swap	d4
	move.w	#$0ff0,d4
	and.w	d6,d4			; d4 for checking E-cmd (in LSW)

	swap	d6
	move.l	d6,d0			; S...S...
	clr.b	d0
	rol.w	#4,d0
	rol.l	#4,d0			; ....00SS

	and.w	#$0fff,d6		; d6 note

	; get sample start address
	move.w	d0,d1			; sample number
	beq.w	set_regs
	lea	mt_SampleStarts-mt_chan1(a4),a0
	subq.w	#1,d0
	add.w	d0,d0
	add.w	d0,d0
	move.l	(a0,d0.w),d2
	move.l	d2,n_start(a2)

	; get length, volume and repeat from sample info table
	add.w	d1,d1
	move.w	mult30tab(pc,d1.w),d0
	lea	(a3,d0.w),a0

	move.w	(a0)+,d0		; length
	move.w	d0,n_reallength(a2)

	; determine period table from fine-tune parameter
	moveq	#0,d3
	move.b	(a0)+,d3
	add.w	d3,d3
	add.w	d3,d3
	move.l	a0,d1
	lea	mt_PerFineTune(pc),a0
	move.l	(a0,d3.w),n_pertab(a2)
	move.l	d1,a0
	cmp.w	#32,d3
	shs	n_minusft(a2)

	moveq	#0,d1
	move.b	(a0)+,d1		; volume
	move.w	d1,n_volume(a2)
	move.w	(a0)+,d3		; repeat offset
	beq.b	no_loop

	; set repeat
	move.w	(a0),d0
	move.w	d0,n_replen(a2)
	add.w	d3,d0
	add.l	d3,d2
	add.l	d3,d2
	bra.b	set_loop

mult30tab:
	dc.w	0*30,1*30,2*30,3*30,4*30,5*30,6*30,7*30
	dc.w	8*30,9*30,10*30,11*30,12*30,13*30,14*30,15*30
	dc.w	16*30,17*30,18*30,19*30,20*30,21*30,22*30,23*30
	dc.w	24*30,25*30,26*30,27*30,28*30,29*30,30*30,31*30

no_loop:
	move.w	(a0),n_replen(a2)
set_loop:
	move.w	d0,n_length(a2)
	move.l	d2,n_loopstart(a2)
	move.l	d2,n_wavestart(a2)

	move.l	mt_MasterVolTab-mt_chan1(a4),a0
	move.b	(a0,d1.w),d1
	move.w	d1,AUDVOL(a5)

set_regs:
; d4 = cmd argument | masked E-cmd
; d5 = cmd*2
; d6 = cmd.w | note.w

	move.w	d4,d3			; d3 masked E-cmd
	swap	d4			; d4 cmd argument into LSW

	tst.w	d6
	beq.w	checkmorefx		; no new note

	cmp.w	#$0e50,d3
	beq.w	set_finetune

	move.w	prefx_tab(pc,d5.w),d0
	jmp	prefx_tab(pc,d0.w)

prefx_tab:
	dc.w	set_period-prefx_tab,set_period-prefx_tab,set_period-prefx_tab
	dc.w	set_toneporta-prefx_tab			; $3
	dc.w	set_period-prefx_tab
	dc.w	set_toneporta-prefx_tab			; $5
	dc.w	set_period-prefx_tab,set_period-prefx_tab,set_period-prefx_tab
	dc.w	set_sampleoffset-prefx_tab		; $9
	dc.w	set_period-prefx_tab,set_period-prefx_tab,set_period-prefx_tab
	dc.w	set_period-prefx_tab,set_period-prefx_tab,set_period-prefx_tab

set_toneporta:
	move.l	n_pertab(a2),a0		; tuned period table

	; find first period which is less or equal the note in d6
	moveq	#36-1,d0
	moveq	#-2,d1
.1:	addq.w	#2,d1
	cmp.w	(a0)+,d6
	dbhs	d0,.1

	tst.b	n_minusft(a2)		; negative fine tune?
	beq.b	.2
	tst.w	d1
	beq.b	.2
	subq.l	#2,a0			; then take previous period
	subq.w	#2,d1

.2:	move.w	d1,n_noteoff(a2)	; note offset in period table
	move.w	n_period(a2),d2
	move.w	-(a0),d1
	cmp.w	d1,d2
	bne.b	.3
	moveq	#0,d1
.3:	move.w	d1,n_wantedperiod(a2)

	move.w	n_funk(a2),d0
	beq.b	.4
	bsr.w	mt_updatefunk

.4:	move.w	d2,AUDPER(a5)
	rts

set_sampleoffset:
; cmd 9 x y (xy = offset in 256 bytes)
; d4 = xy
	moveq	#0,d0
	move.b	d4,d0
	bne.b	.1
	move.b	n_sampleoffset(a2),d0
	bra.b	.2
.1:	move.b	d0,n_sampleoffset(a2)

.2:	lsl.w	#7,d0
	cmp.w	n_length(a2),d0
	bhs.b	.3
	sub.w	d0,n_length(a2)
	add.w	d0,d0
	add.l	d0,n_start(a2)
	bra.b	set_period

.3:	move.w	#1,n_length(a2)
	bra.b	set_period

set_finetune:
	lea	mt_PerFineTune(pc),a0
	moveq	#$0f,d0
	and.b	n_cmdlo(a2),d0
	add.w	d0,d0
	add.w	d0,d0
	move.l	(a0,d0.w),n_pertab(a2)
	cmp.w	#32,d0
	shs	n_minusft(a2)

set_period:
; find nearest period for a note value, then apply finetuning
; d3 = masked E-cmd
; d4 = cmd argument
; d5 = cmd*2
; d6 = note.w

	lea	mt_PeriodTable(pc),a0
	moveq	#36-1,d0
	moveq	#-2,d1
.1:	addq.w	#2,d1			; table offset
	cmp.w	(a0)+,d6
	dbhs	d0,.1

	; apply finetuning, set period and note-offset
	move.l	n_pertab(a2),a0
	move.w	(a0,d1.w),d2
	move.w	d2,n_period(a2)
	move.w	d1,n_noteoff(a2)

	; check for notedelay
	cmp.w	#$0ed0,d3		; notedelay
	beq.b	checkmorefx

	; disable DMA
	move.w	n_dmabit(a2),d0
	move.w	d0,DMACON(a6)

	btst	#2,n_vibratoctrl(a2)
	bne.b	.2
	move.b	d7,n_vibratopos(a2)

.2:	btst	#2,n_tremoloctrl(a2)
	bne.b	.3
	move.b	d7,n_tremolopos(a2)

.3:	move.l	n_start(a2),AUDLC(a5)
	move.w	n_length(a2),AUDLEN(a5)
	move.w	d2,AUDPER(a5)
	lea	mt_dmaon(pc),a0
	or.w	d0,(a0)

checkmorefx:
; d4 = cmd argument
; d5 = cmd*2
; d6 = note.w

	move.w	n_funk(a2),d0
	beq.b	.1
	bsr.w	mt_updatefunk

.1:	move.w	morefx_tab(pc,d5.w),d0
	jmp	morefx_tab(pc,d0.w)

morefx_tab:
	dc.w	mt_pernop-morefx_tab,mt_pernop-morefx_tab,mt_pernop-morefx_tab
	dc.w	mt_pernop-morefx_tab,mt_pernop-morefx_tab,mt_pernop-morefx_tab
	dc.w	mt_pernop-morefx_tab,mt_pernop-morefx_tab,mt_pernop-morefx_tab
	dc.w	mt_pernop-morefx_tab			; $9
	dc.w	mt_pernop-morefx_tab
	dc.w	mt_posjump-morefx_tab			; $B
	dc.w	mt_volchange-morefx_tab
	dc.w	mt_patternbrk-morefx_tab		; $D
	dc.w	mt_e_cmds-morefx_tab
	dc.w	mt_setspeed-morefx_tab


moreblockedfx:
; d6 = note.w | cmd.w

	moveq	#0,d4
	move.b	d6,d4			; cmd argument
	and.w	#$0f00,d6
	lsr.w	#7,d6
	move.w	blmorefx_tab(pc,d6.w),d0
	jmp	blmorefx_tab(pc,d0.w)

blmorefx_tab:
	dc.w	mt_nop-blmorefx_tab,mt_nop-blmorefx_tab
	dc.w	mt_nop-blmorefx_tab,mt_nop-blmorefx_tab
	dc.w	mt_nop-blmorefx_tab,mt_nop-blmorefx_tab
	dc.w	mt_nop-blmorefx_tab,mt_nop-blmorefx_tab
	dc.w	mt_nop-blmorefx_tab,mt_nop-blmorefx_tab
	dc.w	mt_nop-blmorefx_tab
	dc.w	mt_posjump-blmorefx_tab			; $B
	dc.w	mt_nop-blmorefx_tab
	dc.w	mt_patternbrk-blmorefx_tab		; $D
	dc.w	blocked_e_cmds-blmorefx_tab
	dc.w	mt_setspeed-blmorefx_tab		; $F


mt_arpeggio:
; cmd 0 x y (x = first arpeggio offset, y = second arpeggio offset)
; d4 = xy

	moveq	#0,d0
	move.b	mt_Counter-mt_chan1(a4),d0
	move.b	arptab(pc,d0.w),d0
	beq.w	mt_pernop		; step 0, just use normal period
	bmi.b	.1

	; step 1, arpeggio by left nibble
	lsr.b	#4,d4
	bra.b	.2

	; step 2, arpeggio by right nibble
.1:	and.w	#$000f,d4

	; offset current note
.2:	add.w	d4,d4
	add.w	n_noteoff(a2),d4
	cmp.w	#2*36,d4
	bhs.b	.4

	; set period with arpeggio offset from note table
	move.l	n_pertab(a2),a0
	move.w	(a0,d4.w),AUDPER(a5)
.4:	rts

arptab:
	dc.b	0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0
	dc.b	1,-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1


mt_fineportaup:
; cmd E 1 x (subtract x from period)
; d0 = x

	tst.b	mt_Counter-mt_chan1(a4)
	beq.b	do_porta_up
	rts


mt_portaup:
; cmd 1 x x (subtract xx from period)
; d4 = xx

	move.w	d4,d0

do_porta_up:
	move.w	n_period(a2),d1
	sub.w	d0,d1
	cmp.w	#113,d1
	bhs.b	.1
	moveq	#113,d1
.1:	move.w	d1,n_period(a2)
	move.w	d1,AUDPER(a5)
	rts


mt_fineportadn:
; cmd E 2 x (add x to period)
; d0 = x

	tst.b	mt_Counter-mt_chan1(a4)
	beq.b	do_porta_down
	rts


mt_portadown:
; cmd 2 x x (add xx to period)
; d4 = xx

	move.w	d4,d0

do_porta_down:
	move.w	n_period(a2),d1
	add.w	d0,d1
	cmp.w	#856,d1
	bls.b	.1
	move.w	#856,d1
.1:	move.w	d1,n_period(a2)
	move.w	d1,AUDPER(a5)
	rts


mt_toneporta:
; cmd 3 x y (xy = tone portamento speed)
; d4 = xy

	tst.b	d4
	beq.b	mt_toneporta_nc
	move.w	d4,n_toneportspeed(a2)
	move.b	d7,n_cmdlo(a2)

mt_toneporta_nc:
	move.w	n_wantedperiod(a2),d1
	beq.b	.6

	move.w	n_toneportspeed(a2),d0
	move.w	n_period(a2),d2
	cmp.w	d1,d2
	blo.b	.2

	; tone porta up
	sub.w	d0,d2
	cmp.w	d1,d2
	bgt.b	.3
	move.w	d1,d2
	move.w	d7,n_wantedperiod(a2)
	bra.b	.3

	; tone porta down
.2:	add.w	d0,d2
	cmp.w	d1,d2
	blt.b	.3
	move.w	d1,d2
	move.w	d7,n_wantedperiod(a2)

.3:	move.w	d2,n_period(a2)

	tst.b	n_gliss(a2)
	beq.b	.5

	; glissando: find nearest note for new period
	move.l	n_pertab(a2),a0
	moveq	#36-1,d0
	moveq	#-2,d1
.4:	addq.w	#2,d1
	cmp.w	(a0)+,d2
	dbhs	d0,.4

	move.w	d1,n_noteoff(a2)	; @@@ needed?
	move.w	-(a0),d2

.5:	move.w	d2,AUDPER(a5)
.6	rts


mt_vibrato:
; cmd 4 x y (x = speed, y = amplitude)
; d4 = xy

	moveq	#$0f,d2
	and.b	d4,d2
	beq.b	.1
	move.b	d2,n_vibratoamp(a2)
	bra.b	.2
.1:	move.b	n_vibratoamp(a2),d2

.2:	lsr.b	#4,d4
	beq.b	.3
	move.b	d4,n_vibratospd(a2)
	bra.b	mt_vibrato_nc
.3:	move.b	n_vibratospd(a2),d4

mt_vibrato_nc:
	; calculate vibrato table offset: 64 * amplitude + (pos & 63)
	lsl.w	#6,d2
	moveq	#63,d0
	and.b	n_vibratopos(a2),d0
	add.w	d0,d2

	; select vibrato waveform
	moveq	#3,d1
	and.b	n_vibratoctrl(a2),d1
	beq.b	.6
	subq.b	#1,d1
	beq.b	.5

	; ctrl 2 & 3 select a rectangle vibrato
	lea	mt_VibratoRectTable(pc),a0
	bra.b	.9

	; ctrl 1 selects a sawtooth vibrato
.5:	lea	mt_VibratoSawTable(pc),a0
	bra.b	.9

	; ctrl 0 selects a sine vibrato
.6:	lea	mt_VibratoSineTable(pc),a0

	; add vibrato-offset to period
.9:	move.b	(a0,d2.w),d0
	ext.w	d0
	add.w	n_period(a2),d0
	move.w	d0,AUDPER(a5)

	; increase vibratopos by speed
	add.b	d4,n_vibratopos(a2)
	rts


mt_tonevolslide:
; cmd 5 x y (x = volume-up, y = volume-down)
; d4 = xy

	pea	mt_volumeslide(pc)
	bra.w	mt_toneporta_nc


mt_vibrvolslide:
; cmd 6 x y (x = volume-up, y = volume-down)
; d4 = xy

	move.w	d4,d3
	move.b	n_vibratoamp(a2),d2
	move.b	n_vibratospd(a2),d4
	bsr.b	mt_vibrato_nc

	move.w	d3,d4
	bra.b	mt_volumeslide


mt_tremolo:
; cmd 7 x y (x = speed, y = amplitude)
; d4 = xy

	moveq	#$0f,d2
	and.b	d4,d2
	beq.b	.1
	move.b	d2,n_tremoloamp(a2)
	bra.b	.2
.1:	move.b	n_tremoloamp(a2),d2

.2:	lsr.b	#4,d4
	beq.b	.3
	move.b	d4,n_tremolospd(a2)
	bra.b	.4
.3:	move.b	n_tremolospd(a2),d4

	; calculate tremolo table offset: 64 * amplitude + (pos & 63)
.4:	lsl.w	#6,d2
	moveq	#63,d0
	and.b	n_tremolopos(a2),d0
	add.w	d0,d2

	; select tremolo waveform
	moveq	#3,d1
	and.b	n_tremoloctrl(a2),d1
	beq.b	.6
	subq.b	#1,d1
	beq.b	.5

	; ctrl 2 & 3 select a rectangle tremolo
	lea	mt_VibratoRectTable(pc),a0
	bra.b	.9

	; ctrl 1 selects a sawtooth tremolo
.5:	lea	mt_VibratoSawTable(pc),a0
	bra.b	.9

	; ctrl 0 selects a sine tremolo
.6:	lea	mt_VibratoSineTable(pc),a0

	; add tremolo-offset to volume
.9:	move.b	(a0,d2.w),d0
	add.w	n_volume(a2),d0
	bpl.b	.10
	moveq	#0,d0
.10:	cmp.w	#64,d0
	bls.b	.11
	moveq	#64,d0
.11:	move.w	n_period(a2),AUDPER(a5)
	move.l	mt_MasterVolTab-mt_chan1(a4),a0
	move.b	(a0,d0.w),d0
	move.w	d0,AUDVOL(a5)

	; increase tremolopos by speed
	add.b	d4,n_tremolopos(a2)
	rts


mt_volumeslide:
; cmd A x y (x = volume-up, y = volume-down)
; d4 = xy

	move.w	n_volume(a2),d0
	moveq	#$0f,d1
	and.b	d4,d1
	lsr.b	#4,d4
	beq.b	vol_slide_down

	; slide up, until 64
	add.b	d4,d0
vol_slide_up:
	cmp.b	#64,d0
	bls.b	set_vol
	moveq	#64,d0
	bra.b	set_vol

	; slide down, until 0
vol_slide_down:
	sub.b	d1,d0
	bpl.b	set_vol
	moveq	#0,d0

set_vol:
	move.w	d0,n_volume(a2)
	move.w	n_period(a2),AUDPER(a5)
	move.l	mt_MasterVolTab-mt_chan1(a4),a0
	move.b	(a0,d0.w),d0
	move.w	d0,AUDVOL(a5)
	rts


mt_posjump:
; cmd B x y (xy = new song position)
; d4 = xy

	move.b	d4,d0
	subq.b	#1,d0
	move.b	d0,mt_SongPos-mt_chan1(a4)

jump_pos0:
	move.w	d7,mt_PBreakPos-mt_chan1(a4)
	st	mt_PosJumpFlag-mt_chan1(a4)
	rts


mt_volchange:
; cmd C x y (xy = new volume)
; d4 = xy

	cmp.w	#64,d4
	bls.b	.1
	moveq	#64,d4
.1:	move.w	d4,n_volume(a2)
	move.l	mt_MasterVolTab-mt_chan1(a4),a0
	move.b	(a0,d4.w),d4
	move.w	d4,AUDVOL(a5)
	rts


mt_patternbrk:
; cmd D x y (xy = break pos in decimal)
; d4 = xy

	moveq	#$0f,d0
	and.w	d4,d0
	move.w	d4,d1
	lsr.w	#4,d1
	add.b	mult10tab(pc,d1.w),d0
	cmp.b	#63,d0
	bhi.b	jump_pos0

	lsl.w	#4,d0
	move.w	d0,mt_PBreakPos-mt_chan1(a4)
	st	mt_PosJumpFlag-mt_chan1(a4)
	rts

mult10tab:
	dc.b	0,10,20,30,40,50,60,70,80,90,0,0,0,0,0,0


mt_setspeed:
; cmd F x y (xy<$20 new speed, xy>=$20 new tempo)
; d4 = xy

	cmp.b	#$20,d4
	bhs.b	.1
	move.b	d4,mt_Speed-mt_chan1(a4)
	beq.w	mt_end
	rts

	; set tempo (CIA only)
.1:	and.w	#$00ff,d4
	move.l	mt_timerval-mt_chan1(a4),d0
	divu	d4,d0
	move.b	d0,CIAB+CIATALO
	lsr.w	#8,d0
	move.b	d0,CIAB+CIATAHI
	rts


mt_e_cmds:
; cmd E x y (x=command, y=argument)
; d4 = xy

	moveq	#$0f,d0
	and.w	d4,d0			; pass E-cmd argument in d0

	move.w	d4,d1
	lsr.w	#4,d1
	add.w	d1,d1
	move.w	ecmd_tab(pc,d1.w),d1
	jmp	ecmd_tab(pc,d1.w)

ecmd_tab:
	dc.w	mt_filter-ecmd_tab
	dc.w	mt_fineportaup-ecmd_tab
	dc.w	mt_fineportadn-ecmd_tab
	dc.w	mt_glissctrl-ecmd_tab
	dc.w	mt_vibratoctrl-ecmd_tab
	dc.w	mt_finetune-ecmd_tab
	dc.w	mt_jumploop-ecmd_tab
	dc.w	mt_tremoctrl-ecmd_tab
	dc.w	mt_e8-ecmd_tab
	dc.w	mt_retrignote-ecmd_tab
	dc.w	mt_volfineup-ecmd_tab
	dc.w	mt_volfinedn-ecmd_tab
	dc.w	mt_notecut-ecmd_tab
	dc.w	mt_notedelay-ecmd_tab
	dc.w	mt_patterndelay-ecmd_tab
	dc.w	mt_funk-ecmd_tab


blocked_e_cmds:
; cmd E x y (x=command, y=argument)
; d4 = xy

	moveq	#$0f,d0
	and.w	d4,d0			; pass E-cmd argument in d0

	move.w	d4,d1
	lsr.w	#4,d1
	add.w	d1,d1
	move.w	blecmd_tab(pc,d1.w),d1
	jmp	blecmd_tab(pc,d1.w)

blecmd_tab:
	dc.w	mt_filter-blecmd_tab
	dc.w	mt_rts-blecmd_tab
	dc.w	mt_rts-blecmd_tab
	dc.w	mt_glissctrl-blecmd_tab
	dc.w	mt_vibratoctrl-blecmd_tab
	dc.w	mt_finetune-blecmd_tab
	dc.w	mt_jumploop-blecmd_tab
	dc.w	mt_tremoctrl-blecmd_tab
	dc.w	mt_e8-blecmd_tab
	dc.w	mt_rts-blecmd_tab
	dc.w	mt_rts-blecmd_tab
	dc.w	mt_rts-blecmd_tab
	dc.w	mt_rts-blecmd_tab
	dc.w	mt_rts-blecmd_tab
	dc.w	mt_patterndelay-blecmd_tab
	dc.w	mt_rts-blecmd_tab


mt_filter:
; cmd E 0 x (x=1 disable, x=0 enable)
; d0 = x

	lsr.b	#1,d0
	bcs.b	.1
	bclr	#2,CIAA+CIAPRA
	rts
.1:	bset	#2,CIAA+CIAPRA
mt_rts:
	rts


mt_glissctrl:
; cmd E 3 x (x gliss)
; d0 = x

	and.b	#$04,n_gliss(a2)
	or.b	d0,n_gliss(a2)
	rts


mt_vibratoctrl:
; cmd E 4 x (x = vibrato)
; d0 = x

	move.b	d0,n_vibratoctrl(a2)
	rts


mt_finetune:
; cmd E 5 x (x = finetune)
; d0 = x

	lea	mt_PerFineTune(pc),a0
	add.w	d0,d0
	add.w	d0,d0
	move.l	(a0,d0.w),n_pertab(a2)
	cmp.w	#32,d0
	shs	n_minusft(a2)
	rts


mt_jumploop:
; cmd E 6 x (x=0 loop start, else loop count)
; d0 = x

	tst.b	mt_Counter-mt_chan1(a4)
	bne.b	.4

.1:	tst.b	d0
	beq.b	.3			; set start

	; otherwise we are at the end of the loop
	subq.b	#1,n_loopcount(a2)
	beq.b	.4			; loop finished
	bpl.b	.2

	; initialize loop counter
	move.b	d0,n_loopcount(a2)

	; jump back to start of loop
.2:	move.w	n_pattpos(a2),mt_PBreakPos-mt_chan1(a4)
	st	mt_PBreakFlag-mt_chan1(a4)
	rts

	; remember start of loop position
.3:	move.w	mt_PatternPos-mt_chan1(a4),d0
	move.w	d0,n_pattpos(a2)
.4:	rts


mt_tremoctrl:
; cmd E 7 x (x = tremolo)
; d0 = x

	move.b	d0,n_tremoloctrl(a2)
	rts


mt_e8:
; cmd E 8 x (x = trigger value)
; d0 = x

	move.b	d0,mt_E8Trigger-mt_chan1(a4)
	rts


mt_retrignote:
; cmd E 9 x (x = retrigger count)
; d0 = x

	tst.b	d0
	beq.b	.2

	; set new retrigger count when Counter=0
.1:	tst.b	mt_Counter-mt_chan1(a4)
	bne.b	.3
	move.b	d0,n_retrigcount(a2)

	; avoid double retrigger, when Counter=0 and a note was set
	move.w	#$0fff,d2
	and.w	(a2),d2
	beq.b	do_retrigger
.2:	rts

	; check if retrigger count is reached
.3:	subq.b	#1,n_retrigcount(a2)
	bne.b	.2
	move.b	d0,n_retrigcount(a2)	; reset

do_retrigger:
	; DMA off, set sample pointer and length
	move.w	n_dmabit(a2),d0
	move.w	d0,DMACON(a6)
	move.l	n_start(a2),AUDLC(a5)
	move.w	n_length(a2),AUDLEN(a5)
	lea	mt_dmaon(pc),a0
	or.w	d0,(a0)
	rts


mt_volfineup:
; cmd E A x (x = volume add)
; d0 = x

	tst.b	mt_Counter-mt_chan1(a4)
	beq.b	.1
	rts

.1:	add.w	n_volume(a2),d0
	bra.w	vol_slide_up


mt_volfinedn:
; cmd E B x (x = volume sub)
; d0 = x

	tst.b	mt_Counter-mt_chan1(a4)
	beq.b	.1
	rts

.1:	move.b	d0,d1
	move.w	n_volume(a2),d0
	bra.w	vol_slide_down


mt_notecut:
; cmd E C x (x = counter to cut at)
; d0 = x

	cmp.b	mt_Counter-mt_chan1(a4),d0
	bne.b	.1
	move.w	d7,n_volume(a2)
	move.w	d7,AUDVOL(a5)
.1:	rts


mt_notedelay:
; cmd E D x (x = counter to retrigger at)
; d0 = x

	cmp.b	mt_Counter-mt_chan1(a4),d0
	bne.b	.1
	tst.w	(a2)			; trigger note when given
	bne.b	do_retrigger
.1:	rts


mt_patterndelay:
; cmd E E x (x = delay count)
; d0 = x

	tst.b	mt_Counter-mt_chan1(a4)
	bne.b	.1
	tst.b	mt_PattDelTime2-mt_chan1(a4)
	bne.b	.1
	addq.b	#1,d0
	move.b	d0,mt_PattDelTime-mt_chan1(a4)
.1:	rts


mt_funk:
; cmd E F x (x = funk speed)
; d0 = x

	tst.b	mt_Counter-mt_chan1(a4)
	bne.b	.1
	move.w	d0,n_funk(a2)
	bne.b	mt_updatefunk
.1:	rts

mt_updatefunk:
; d0 = funk speed

	move.b	mt_FunkTable(pc,d0.w),d0
	add.b	d0,n_funkoffset(a2)
	bpl.b	.2
	move.b	d7,n_funkoffset(a2)

	move.l	n_loopstart(a2),d0
	moveq	#0,d1
	move.w	n_replen(a2),d1
	add.l	d1,d1
	add.l	d0,d1
	move.l	n_wavestart(a2),a0
	addq.l	#1,a0
	cmp.l	d1,a0
	blo.b	.1
	move.l	d0,a0
.1:	move.l	a0,n_wavestart(a2)
	not.b	(a0)

.2:	rts


mt_FunkTable:
	dc.b	0,5,6,7,8,10,11,13,16,19,22,26,32,43,64,128

mt_VibratoSineTable:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1
	dc.b	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0
	dc.b	0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	dc.b	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0
	dc.b	0,0,0,1,1,1,2,2,2,3,3,3,3,3,3,3
	dc.b	3,3,3,3,3,3,3,3,2,2,2,1,1,1,0,0
	dc.b	0,0,0,-1,-1,-1,-2,-2,-2,-3,-3,-3,-3,-3,-3,-3
	dc.b	-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-1,-1,-1,0,0
	dc.b	0,0,1,1,2,2,3,3,4,4,4,5,5,5,5,5
	dc.b	5,5,5,5,5,5,4,4,4,3,3,2,2,1,1,0
	dc.b	0,0,-1,-1,-2,-2,-3,-3,-4,-4,-4,-5,-5,-5,-5,-5
	dc.b	-5,-5,-5,-5,-5,-5,-4,-4,-4,-3,-3,-2,-2,-1,-1,0
	dc.b	0,0,1,2,3,3,4,5,5,6,6,7,7,7,7,7
	dc.b	7,7,7,7,7,7,6,6,5,5,4,3,3,2,1,0
	dc.b	0,0,-1,-2,-3,-3,-4,-5,-5,-6,-6,-7,-7,-7,-7,-7
	dc.b	-7,-7,-7,-7,-7,-7,-6,-6,-5,-5,-4,-3,-3,-2,-1,0
	dc.b	0,0,1,2,3,4,5,6,7,7,8,8,9,9,9,9
	dc.b	9,9,9,9,9,8,8,7,7,6,5,4,3,2,1,0
	dc.b	0,0,-1,-2,-3,-4,-5,-6,-7,-7,-8,-8,-9,-9,-9,-9
	dc.b	-9,-9,-9,-9,-9,-8,-8,-7,-7,-6,-5,-4,-3,-2,-1,0
	dc.b	0,1,2,3,4,5,6,7,8,9,9,10,11,11,11,11
	dc.b	11,11,11,11,11,10,9,9,8,7,6,5,4,3,2,1
	dc.b	0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-9,-10,-11,-11,-11,-11
	dc.b	-11,-11,-11,-11,-11,-10,-9,-9,-8,-7,-6,-5,-4,-3,-2,-1
	dc.b	0,1,2,4,5,6,7,8,9,10,11,12,12,13,13,13
	dc.b	13,13,13,13,12,12,11,10,9,8,7,6,5,4,2,1
	dc.b	0,-1,-2,-4,-5,-6,-7,-8,-9,-10,-11,-12,-12,-13,-13,-13
	dc.b	-13,-13,-13,-13,-12,-12,-11,-10,-9,-8,-7,-6,-5,-4,-2,-1
	dc.b	0,1,3,4,6,7,8,10,11,12,13,14,14,15,15,15
	dc.b	15,15,15,15,14,14,13,12,11,10,8,7,6,4,3,1
	dc.b	0,-1,-3,-4,-6,-7,-8,-10,-11,-12,-13,-14,-14,-15,-15,-15
	dc.b	-15,-15,-15,-15,-14,-14,-13,-12,-11,-10,-8,-7,-6,-4,-3,-1
	dc.b	0,1,3,5,6,8,9,11,12,13,14,15,16,17,17,17
	dc.b	17,17,17,17,16,15,14,13,12,11,9,8,6,5,3,1
	dc.b	0,-1,-3,-5,-6,-8,-9,-11,-12,-13,-14,-15,-16,-17,-17,-17
	dc.b	-17,-17,-17,-17,-16,-15,-14,-13,-12,-11,-9,-8,-6,-5,-3,-1
	dc.b	0,1,3,5,7,9,11,12,14,15,16,17,18,19,19,19
	dc.b	19,19,19,19,18,17,16,15,14,12,11,9,7,5,3,1
	dc.b	0,-1,-3,-5,-7,-9,-11,-12,-14,-15,-16,-17,-18,-19,-19,-19
	dc.b	-19,-19,-19,-19,-18,-17,-16,-15,-14,-12,-11,-9,-7,-5,-3,-1
	dc.b	0,2,4,6,8,10,12,13,15,16,18,19,20,20,21,21
	dc.b	21,21,21,20,20,19,18,16,15,13,12,10,8,6,4,2
	dc.b	0,-2,-4,-6,-8,-10,-12,-13,-15,-16,-18,-19,-20,-20,-21,-21
	dc.b	-21,-21,-21,-20,-20,-19,-18,-16,-15,-13,-12,-10,-8,-6,-4,-2
	dc.b	0,2,4,6,9,11,13,15,16,18,19,21,22,22,23,23
	dc.b	23,23,23,22,22,21,19,18,16,15,13,11,9,6,4,2
	dc.b	0,-2,-4,-6,-9,-11,-13,-15,-16,-18,-19,-21,-22,-22,-23,-23
	dc.b	-23,-23,-23,-22,-22,-21,-19,-18,-16,-15,-13,-11,-9,-6,-4,-2
	dc.b	0,2,4,7,9,12,14,16,18,20,21,22,23,24,25,25
	dc.b	25,25,25,24,23,22,21,20,18,16,14,12,9,7,4,2
	dc.b	0,-2,-4,-7,-9,-12,-14,-16,-18,-20,-21,-22,-23,-24,-25,-25
	dc.b	-25,-25,-25,-24,-23,-22,-21,-20,-18,-16,-14,-12,-9,-7,-4,-2
	dc.b	0,2,5,8,10,13,15,17,19,21,23,24,25,26,27,27
	dc.b	27,27,27,26,25,24,23,21,19,17,15,13,10,8,5,2
	dc.b	0,-2,-5,-8,-10,-13,-15,-17,-19,-21,-23,-24,-25,-26,-27,-27
	dc.b	-27,-27,-27,-26,-25,-24,-23,-21,-19,-17,-15,-13,-10,-8,-5,-2
	dc.b	0,2,5,8,11,14,16,18,21,23,24,26,27,28,29,29
	dc.b	29,29,29,28,27,26,24,23,21,18,16,14,11,8,5,2
	dc.b	0,-2,-5,-8,-11,-14,-16,-18,-21,-23,-24,-26,-27,-28,-29,-29
	dc.b	-29,-29,-29,-28,-27,-26,-24,-23,-21,-18,-16,-14,-11,-8,-5,-2

mt_VibratoSawTable:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	dc.b	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1
	dc.b	2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3
	dc.b	-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2
	dc.b	-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2
	dc.b	3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5
	dc.b	-5,-5,-5,-5,-5,-5,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3
	dc.b	-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,0
	dc.b	0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3
	dc.b	4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7
	dc.b	-7,-7,-7,-7,-6,-6,-6,-6,-5,-5,-5,-5,-4,-4,-4,-4
	dc.b	-3,-3,-3,-3,-2,-2,-2,-2,-1,-1,-1,-1,0,0,0,0
	dc.b	0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,4
	dc.b	5,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9
	dc.b	-9,-9,-9,-9,-8,-8,-8,-7,-7,-7,-6,-6,-6,-5,-5,-5
	dc.b	-4,-4,-4,-4,-3,-3,-3,-2,-2,-2,-1,-1,-1,0,0,0
	dc.b	0,0,0,1,1,1,2,2,3,3,3,4,4,4,5,5
	dc.b	6,6,6,7,7,7,8,8,9,9,9,10,10,10,11,11
	dc.b	-11,-11,-11,-10,-10,-10,-9,-9,-8,-8,-8,-7,-7,-7,-6,-6
	dc.b	-5,-5,-5,-4,-4,-4,-3,-3,-2,-2,-2,-1,-1,-1,0,0
	dc.b	0,0,0,1,1,2,2,3,3,3,4,4,5,5,6,6
	dc.b	7,7,7,8,8,9,9,10,10,10,11,11,12,12,13,13
	dc.b	-13,-13,-13,-12,-12,-11,-11,-10,-10,-10,-9,-9,-8,-8,-7,-7
	dc.b	-6,-6,-6,-5,-5,-4,-4,-3,-3,-3,-2,-2,-1,-1,0,0
	dc.b	0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7
	dc.b	8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15
	dc.b	-15,-15,-14,-14,-13,-13,-12,-12,-11,-11,-10,-10,-9,-9,-8,-8
	dc.b	-7,-7,-6,-6,-5,-5,-4,-4,-3,-3,-2,-2,-1,-1,0,0
	dc.b	0,0,1,1,2,2,3,3,4,5,5,6,6,7,7,8
	dc.b	9,9,10,10,11,11,12,12,13,14,14,15,15,16,16,17
	dc.b	-17,-17,-16,-16,-15,-15,-14,-13,-13,-12,-12,-11,-11,-10,-10,-9
	dc.b	-8,-8,-7,-7,-6,-6,-5,-4,-4,-3,-3,-2,-2,-1,-1,0
	dc.b	0,0,1,1,2,3,3,4,5,5,6,6,7,8,8,9
	dc.b	10,10,11,11,12,13,13,14,15,15,16,16,17,18,18,19
	dc.b	-19,-19,-18,-18,-17,-16,-16,-15,-14,-14,-13,-13,-12,-11,-11,-10
	dc.b	-9,-9,-8,-8,-7,-6,-6,-5,-4,-4,-3,-3,-2,-1,-1,0
	dc.b	0,0,1,2,2,3,4,4,5,6,6,7,8,8,9,10
	dc.b	11,11,12,13,13,14,15,15,16,17,17,18,19,19,20,21
	dc.b	-21,-21,-20,-19,-19,-18,-17,-17,-16,-15,-15,-14,-13,-12,-12,-11
	dc.b	-10,-10,-9,-8,-8,-7,-6,-6,-5,-4,-4,-3,-2,-1,-1,0
	dc.b	0,0,1,2,3,3,4,5,6,6,7,8,9,9,10,11
	dc.b	12,12,13,14,15,15,16,17,18,18,19,20,21,21,22,23
	dc.b	-23,-23,-22,-21,-20,-20,-19,-18,-17,-17,-16,-15,-14,-14,-13,-12
	dc.b	-11,-11,-10,-9,-8,-8,-7,-6,-5,-5,-4,-3,-2,-2,-1,0
	dc.b	0,0,1,2,3,4,4,5,6,7,8,8,9,10,11,12
	dc.b	13,13,14,15,16,17,17,18,19,20,21,21,22,23,24,25
	dc.b	-25,-25,-24,-23,-22,-21,-21,-20,-19,-18,-17,-16,-16,-15,-14,-13
	dc.b	-12,-12,-11,-10,-9,-8,-8,-7,-6,-5,-4,-3,-3,-2,-1,0
	dc.b	0,0,1,2,3,4,5,6,7,7,8,9,10,11,12,13
	dc.b	14,14,15,16,17,18,19,20,21,21,22,23,24,25,26,27
	dc.b	-27,-27,-26,-25,-24,-23,-22,-21,-20,-20,-19,-18,-17,-16,-15,-14
	dc.b	-13,-13,-12,-11,-10,-9,-8,-7,-6,-6,-5,-4,-3,-2,-1,0
	dc.b	0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
	dc.b	15,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29
	dc.b	-29,-28,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15
	dc.b	-14,-13,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0

mt_VibratoRectTable:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	dc.b	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	dc.b	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	dc.b	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	dc.b	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
	dc.b	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
	dc.b	-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3
	dc.b	-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3
	dc.b	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
	dc.b	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
	dc.b	-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5
	dc.b	-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5
	dc.b	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
	dc.b	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
	dc.b	-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7
	dc.b	-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7
	dc.b	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9
	dc.b	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9
	dc.b	-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9
	dc.b	-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9,-9
	dc.b	11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11
	dc.b	11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11
	dc.b	-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11
	dc.b	-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11
	dc.b	13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13
	dc.b	13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13
	dc.b	-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13
	dc.b	-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,-13
	dc.b	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15
	dc.b	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15
	dc.b	-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15
	dc.b	-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15,-15
	dc.b	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17
	dc.b	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17
	dc.b	-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17
	dc.b	-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17,-17
	dc.b	19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19
	dc.b	19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19
	dc.b	-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19
	dc.b	-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19,-19
	dc.b	21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21
	dc.b	21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21
	dc.b	-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21
	dc.b	-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21,-21
	dc.b	23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	dc.b	23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	dc.b	-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23
	dc.b	-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23,-23
	dc.b	25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25
	dc.b	25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25
	dc.b	-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25
	dc.b	-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25,-25
	dc.b	27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27
	dc.b	27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27
	dc.b	-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27
	dc.b	-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27
	dc.b	29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29
	dc.b	29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29
	dc.b	-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29
	dc.b	-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29


mt_PeriodTable:
mt_Tuning0:	; Tuning 0, Normal c-1 - b3
	dc.w	856,808,762,720,678,640,604,570,538,508,480,453
	dc.w	428,404,381,360,339,320,302,285,269,254,240,226
	dc.w	214,202,190,180,170,160,151,143,135,127,120,113
mt_Tuning1:
	dc.w	850,802,757,715,674,637,601,567,535,505,477,450
	dc.w	425,401,379,357,337,318,300,284,268,253,239,225
	dc.w	213,201,189,179,169,159,150,142,134,126,119,113
mt_Tuning2:
	dc.w	844,796,752,709,670,632,597,563,532,502,474,447
	dc.w	422,398,376,355,335,316,298,282,266,251,237,224
	dc.w	211,199,188,177,167,158,149,141,133,125,118,112
mt_Tuning3:
	dc.w	838,791,746,704,665,628,592,559,528,498,470,444
	dc.w	419,395,373,352,332,314,296,280,264,249,235,222
	dc.w	209,198,187,176,166,157,148,140,132,125,118,111
mt_Tuning4:
	dc.w	832,785,741,699,660,623,588,555,524,495,467,441
	dc.w	416,392,370,350,330,312,294,278,262,247,233,220
	dc.w	208,196,185,175,165,156,147,139,131,124,117,110
mt_Tuning5:
	dc.w	826,779,736,694,655,619,584,551,520,491,463,437
	dc.w	413,390,368,347,328,309,292,276,260,245,232,219
	dc.w	206,195,184,174,164,155,146,138,130,123,116,109
mt_Tuning6:
	dc.w	820,774,730,689,651,614,580,547,516,487,460,434
	dc.w	410,387,365,345,325,307,290,274,258,244,230,217
	dc.w	205,193,183,172,163,154,145,137,129,122,115,109
mt_Tuning7:
	dc.w	814,768,725,684,646,610,575,543,513,484,457,431
	dc.w	407,384,363,342,323,305,288,272,256,242,228,216
	dc.w	204,192,181,171,161,152,144,136,128,121,114,108
mt_TuningM8:
	dc.w	907,856,808,762,720,678,640,604,570,538,508,480
	dc.w	453,428,404,381,360,339,320,302,285,269,254,240
	dc.w	226,214,202,190,180,170,160,151,143,135,127,120
mt_TuningM7:
	dc.w	900,850,802,757,715,675,636,601,567,535,505,477
	dc.w	450,425,401,379,357,337,318,300,284,268,253,238
	dc.w	225,212,200,189,179,169,159,150,142,134,126,119
mt_TuningM6:
	dc.w	894,844,796,752,709,670,632,597,563,532,502,474
	dc.w	447,422,398,376,355,335,316,298,282,266,251,237
	dc.w	223,211,199,188,177,167,158,149,141,133,125,118
mt_TuningM5:
	dc.w	887,838,791,746,704,665,628,592,559,528,498,470
	dc.w	444,419,395,373,352,332,314,296,280,264,249,235
	dc.w	222,209,198,187,176,166,157,148,140,132,125,118
mt_TuningM4:
	dc.w	881,832,785,741,699,660,623,588,555,524,494,467
	dc.w	441,416,392,370,350,330,312,294,278,262,247,233
	dc.w	220,208,196,185,175,165,156,147,139,131,123,117
mt_TuningM3:
	dc.w	875,826,779,736,694,655,619,584,551,520,491,463
	dc.w	437,413,390,368,347,328,309,292,276,260,245,232
	dc.w	219,206,195,184,174,164,155,146,138,130,123,116
mt_TuningM2:
	dc.w	868,820,774,730,689,651,614,580,547,516,487,460
	dc.w	434,410,387,365,345,325,307,290,274,258,244,230
	dc.w	217,205,193,183,172,163,154,145,137,129,122,115
mt_TuningM1:
	dc.w	862,814,768,725,684,646,610,575,543,513,484,457
	dc.w	431,407,384,363,342,323,305,288,272,256,242,228
	dc.w	216,203,192,181,171,161,152,144,136,128,121,114

mt_PerFineTune:
	dc.l	mt_Tuning0,mt_Tuning1,mt_Tuning2,mt_Tuning3
	dc.l	mt_Tuning4,mt_Tuning5,mt_Tuning6,mt_Tuning7
	dc.l	mt_TuningM8,mt_TuningM7,mt_TuningM6,mt_TuningM5
	dc.l	mt_TuningM4,mt_TuningM3,mt_TuningM2,mt_TuningM1

MasterVolTab0:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0
MasterVolTab1:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	1
MasterVolTab2:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	dc.b	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	dc.b	2
MasterVolTab3:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1
	dc.b	1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2
	dc.b	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	dc.b	3
MasterVolTab4:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dc.b	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	dc.b	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
	dc.b	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
	dc.b	4
MasterVolTab5:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1
	dc.b	1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2
	dc.b	2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3
	dc.b	3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4
	dc.b	5
MasterVolTab6:
	dc.b	0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1
	dc.b	1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2
	dc.b	3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4
	dc.b	4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5
	dc.b	6
MasterVolTab7:
	dc.b	0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1
	dc.b	1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3
	dc.b	3,3,3,3,3,4,4,4,4,4,4,4,4,4,5,5
	dc.b	5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6
	dc.b	7
MasterVolTab8:
	dc.b	0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1
	dc.b	2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3
	dc.b	4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5
	dc.b	6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7
	dc.b	8
MasterVolTab9:
	dc.b	0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,2
	dc.b	2,2,2,2,2,2,3,3,3,3,3,3,3,4,4,4
	dc.b	4,4,4,4,5,5,5,5,5,5,5,6,6,6,6,6
	dc.b	6,6,7,7,7,7,7,7,7,8,8,8,8,8,8,8
	dc.b	9
MasterVolTab10:
	dc.b	0,0,0,0,0,0,0,1,1,1,1,1,1,2,2,2
	dc.b	2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,4
	dc.b	5,5,5,5,5,5,5,6,6,6,6,6,6,7,7,7
	dc.b	7,7,7,7,8,8,8,8,8,8,9,9,9,9,9,9
	dc.b	10
MasterVolTab11:
	dc.b	0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2
	dc.b	2,2,3,3,3,3,3,3,4,4,4,4,4,4,5,5
	dc.b	5,5,5,6,6,6,6,6,6,7,7,7,7,7,7,8
	dc.b	8,8,8,8,8,9,9,9,9,9,9,10,10,10,10,10
	dc.b	11
MasterVolTab12:
	dc.b	0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2
	dc.b	3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5
	dc.b	6,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8
	dc.b	9,9,9,9,9,9,10,10,10,10,10,11,11,11,11,11
	dc.b	12
MasterVolTab13:
	dc.b	0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3
	dc.b	3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6
	dc.b	6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9
	dc.b	9,9,10,10,10,10,10,11,11,11,11,11,12,12,12,12
	dc.b	13
MasterVolTab14:
	dc.b	0,0,0,0,0,1,1,1,1,1,2,2,2,2,3,3
	dc.b	3,3,3,4,4,4,4,5,5,5,5,5,6,6,6,6
	dc.b	7,7,7,7,7,8,8,8,8,8,9,9,9,9,10,10
	dc.b	10,10,10,11,11,11,11,12,12,12,12,12,13,13,13,13
	dc.b	14
MasterVolTab15:
	dc.b	0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3
	dc.b	3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7
	dc.b	7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,11
	dc.b	11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14
	dc.b	15
MasterVolTab16:
	dc.b	0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3
	dc.b	4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7
	dc.b	8,8,8,8,9,9,9,9,10,10,10,10,11,11,11,11
	dc.b	12,12,12,12,13,13,13,13,14,14,14,14,15,15,15,15
	dc.b	16
MasterVolTab17:
	dc.b	0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3
	dc.b	4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,8
	dc.b	8,8,9,9,9,9,10,10,10,10,11,11,11,11,12,12
	dc.b	12,13,13,13,13,14,14,14,14,15,15,15,15,16,16,16
	dc.b	17
MasterVolTab18:
	dc.b	0,0,0,0,1,1,1,1,2,2,2,3,3,3,3,4
	dc.b	4,4,5,5,5,5,6,6,6,7,7,7,7,8,8,8
	dc.b	9,9,9,9,10,10,10,10,11,11,11,12,12,12,12,13
	dc.b	13,13,14,14,14,14,15,15,15,16,16,16,16,17,17,17
	dc.b	18
MasterVolTab19:
	dc.b	0,0,0,0,1,1,1,2,2,2,2,3,3,3,4,4
	dc.b	4,5,5,5,5,6,6,6,7,7,7,8,8,8,8,9
	dc.b	9,9,10,10,10,10,11,11,11,12,12,12,13,13,13,13
	dc.b	14,14,14,15,15,15,16,16,16,16,17,17,17,18,18,18
	dc.b	19
MasterVolTab20:
	dc.b	0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,4
	dc.b	5,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9
	dc.b	10,10,10,10,11,11,11,12,12,12,13,13,13,14,14,14
	dc.b	15,15,15,15,16,16,16,17,17,17,18,18,18,19,19,19
	dc.b	20
MasterVolTab21:
	dc.b	0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,4
	dc.b	5,5,5,6,6,6,7,7,7,8,8,8,9,9,9,10
	dc.b	10,10,11,11,11,12,12,12,13,13,13,14,14,14,15,15
	dc.b	15,16,16,16,17,17,17,18,18,18,19,19,19,20,20,20
	dc.b	21
MasterVolTab22:
	dc.b	0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5
	dc.b	5,5,6,6,6,7,7,7,8,8,8,9,9,9,10,10
	dc.b	11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,16
	dc.b	16,16,17,17,17,18,18,18,19,19,19,20,20,20,21,21
	dc.b	22
MasterVolTab23:
	dc.b	0,0,0,1,1,1,2,2,2,3,3,3,4,4,5,5
	dc.b	5,6,6,6,7,7,7,8,8,8,9,9,10,10,10,11
	dc.b	11,11,12,12,12,13,13,14,14,14,15,15,15,16,16,16
	dc.b	17,17,17,18,18,19,19,19,20,20,20,21,21,21,22,22
	dc.b	23
MasterVolTab24:
	dc.b	0,0,0,1,1,1,2,2,3,3,3,4,4,4,5,5
	dc.b	6,6,6,7,7,7,8,8,9,9,9,10,10,10,11,11
	dc.b	12,12,12,13,13,13,14,14,15,15,15,16,16,16,17,17
	dc.b	18,18,18,19,19,19,20,20,21,21,21,22,22,22,23,23
	dc.b	24
MasterVolTab25:
	dc.b	0,0,0,1,1,1,2,2,3,3,3,4,4,5,5,5
	dc.b	6,6,7,7,7,8,8,8,9,9,10,10,10,11,11,12
	dc.b	12,12,13,13,14,14,14,15,15,16,16,16,17,17,17,18
	dc.b	18,19,19,19,20,20,21,21,21,22,22,23,23,23,24,24
	dc.b	25
MasterVolTab26:
	dc.b	0,0,0,1,1,2,2,2,3,3,4,4,4,5,5,6
	dc.b	6,6,7,7,8,8,8,9,9,10,10,10,11,11,12,12
	dc.b	13,13,13,14,14,15,15,15,16,16,17,17,17,18,18,19
	dc.b	19,19,20,20,21,21,21,22,22,23,23,23,24,24,25,25
	dc.b	26
MasterVolTab27:
	dc.b	0,0,0,1,1,2,2,2,3,3,4,4,5,5,5,6
	dc.b	6,7,7,8,8,8,9,9,10,10,10,11,11,12,12,13
	dc.b	13,13,14,14,15,15,16,16,16,17,17,18,18,18,19,19
	dc.b	20,20,21,21,21,22,22,23,23,24,24,24,25,25,26,26
	dc.b	27
MasterVolTab28:
	dc.b	0,0,0,1,1,2,2,3,3,3,4,4,5,5,6,6
	dc.b	7,7,7,8,8,9,9,10,10,10,11,11,12,12,13,13
	dc.b	14,14,14,15,15,16,16,17,17,17,18,18,19,19,20,20
	dc.b	21,21,21,22,22,23,23,24,24,24,25,25,26,26,27,27
	dc.b	28
MasterVolTab29:
	dc.b	0,0,0,1,1,2,2,3,3,4,4,4,5,5,6,6
	dc.b	7,7,8,8,9,9,9,10,10,11,11,12,12,13,13,14
	dc.b	14,14,15,15,16,16,17,17,18,18,19,19,19,20,20,21
	dc.b	21,22,22,23,23,24,24,24,25,25,26,26,27,27,28,28
	dc.b	29
MasterVolTab30:
	dc.b	0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7
	dc.b	7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14
	dc.b	15,15,15,16,16,17,17,18,18,19,19,20,20,21,21,22
	dc.b	22,22,23,23,24,24,25,25,26,26,27,27,28,28,29,29
	dc.b	30
MasterVolTab31:
	dc.b	0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7
	dc.b	7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15
	dc.b	15,15,16,16,17,17,18,18,19,19,20,20,21,21,22,22
	dc.b	23,23,24,24,25,25,26,26,27,27,28,28,29,29,30,30
	dc.b	31
MasterVolTab32:
	dc.b	0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7
	dc.b	8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15
	dc.b	16,16,17,17,18,18,19,19,20,20,21,21,22,22,23,23
	dc.b	24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31
	dc.b	32
MasterVolTab33:
	dc.b	0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7
	dc.b	8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15
	dc.b	16,17,17,18,18,19,19,20,20,21,21,22,22,23,23,24
	dc.b	24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,32
	dc.b	33
MasterVolTab34:
	dc.b	0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7
	dc.b	8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16
	dc.b	17,17,18,18,19,19,20,20,21,21,22,22,23,23,24,24
	dc.b	25,26,26,27,27,28,28,29,29,30,30,31,31,32,32,33
	dc.b	34
MasterVolTab35:
	dc.b	0,0,1,1,2,2,3,3,4,4,5,6,6,7,7,8
	dc.b	8,9,9,10,10,11,12,12,13,13,14,14,15,15,16,16
	dc.b	17,18,18,19,19,20,20,21,21,22,22,23,24,24,25,25
	dc.b	26,26,27,27,28,28,29,30,30,31,31,32,32,33,33,34
	dc.b	35
MasterVolTab36:
	dc.b	0,0,1,1,2,2,3,3,4,5,5,6,6,7,7,8
	dc.b	9,9,10,10,11,11,12,12,13,14,14,15,15,16,16,17
	dc.b	18,18,19,19,20,20,21,21,22,23,23,24,24,25,25,26
	dc.b	27,27,28,28,29,29,30,30,31,32,32,33,33,34,34,35
	dc.b	36
MasterVolTab37:
	dc.b	0,0,1,1,2,2,3,4,4,5,5,6,6,7,8,8
	dc.b	9,9,10,10,11,12,12,13,13,14,15,15,16,16,17,17
	dc.b	18,19,19,20,20,21,21,22,23,23,24,24,25,26,26,27
	dc.b	27,28,28,29,30,30,31,31,32,32,33,34,34,35,35,36
	dc.b	37
MasterVolTab38:
	dc.b	0,0,1,1,2,2,3,4,4,5,5,6,7,7,8,8
	dc.b	9,10,10,11,11,12,13,13,14,14,15,16,16,17,17,18
	dc.b	19,19,20,20,21,21,22,23,23,24,24,25,26,26,27,27
	dc.b	28,29,29,30,30,31,32,32,33,33,34,35,35,36,36,37
	dc.b	38
MasterVolTab39:
	dc.b	0,0,1,1,2,3,3,4,4,5,6,6,7,7,8,9
	dc.b	9,10,10,11,12,12,13,14,14,15,15,16,17,17,18,18
	dc.b	19,20,20,21,21,22,23,23,24,24,25,26,26,27,28,28
	dc.b	29,29,30,31,31,32,32,33,34,34,35,35,36,37,37,38
	dc.b	39
MasterVolTab40:
	dc.b	0,0,1,1,2,3,3,4,5,5,6,6,7,8,8,9
	dc.b	10,10,11,11,12,13,13,14,15,15,16,16,17,18,18,19
	dc.b	20,20,21,21,22,23,23,24,25,25,26,26,27,28,28,29
	dc.b	30,30,31,31,32,33,33,34,35,35,36,36,37,38,38,39
	dc.b	40
MasterVolTab41:
	dc.b	0,0,1,1,2,3,3,4,5,5,6,7,7,8,8,9
	dc.b	10,10,11,12,12,13,14,14,15,16,16,17,17,18,19,19
	dc.b	20,21,21,22,23,23,24,24,25,26,26,27,28,28,29,30
	dc.b	30,31,32,32,33,33,34,35,35,36,37,37,38,39,39,40
	dc.b	41
MasterVolTab42:
	dc.b	0,0,1,1,2,3,3,4,5,5,6,7,7,8,9,9
	dc.b	10,11,11,12,13,13,14,15,15,16,17,17,18,19,19,20
	dc.b	21,21,22,22,23,24,24,25,26,26,27,28,28,29,30,30
	dc.b	31,32,32,33,34,34,35,36,36,37,38,38,39,40,40,41
	dc.b	42
MasterVolTab43:
	dc.b	0,0,1,2,2,3,4,4,5,6,6,7,8,8,9,10
	dc.b	10,11,12,12,13,14,14,15,16,16,17,18,18,19,20,20
	dc.b	21,22,22,23,24,24,25,26,26,27,28,28,29,30,30,31
	dc.b	32,32,33,34,34,35,36,36,37,38,38,39,40,40,41,42
	dc.b	43
MasterVolTab44:
	dc.b	0,0,1,2,2,3,4,4,5,6,6,7,8,8,9,10
	dc.b	11,11,12,13,13,14,15,15,16,17,17,18,19,19,20,21
	dc.b	22,22,23,24,24,25,26,26,27,28,28,29,30,30,31,32
	dc.b	33,33,34,35,35,36,37,37,38,39,39,40,41,41,42,43
	dc.b	44
MasterVolTab45:
	dc.b	0,0,1,2,2,3,4,4,5,6,7,7,8,9,9,10
	dc.b	11,11,12,13,14,14,15,16,16,17,18,18,19,20,21,21
	dc.b	22,23,23,24,25,26,26,27,28,28,29,30,30,31,32,33
	dc.b	33,34,35,35,36,37,37,38,39,40,40,41,42,42,43,44
	dc.b	45
MasterVolTab46:
	dc.b	0,0,1,2,2,3,4,5,5,6,7,7,8,9,10,10
	dc.b	11,12,12,13,14,15,15,16,17,17,18,19,20,20,21,22
	dc.b	23,23,24,25,25,26,27,28,28,29,30,30,31,32,33,33
	dc.b	34,35,35,36,37,38,38,39,40,40,41,42,43,43,44,45
	dc.b	46
MasterVolTab47:
	dc.b	0,0,1,2,2,3,4,5,5,6,7,8,8,9,10,11
	dc.b	11,12,13,13,14,15,16,16,17,18,19,19,20,21,22,22
	dc.b	23,24,24,25,26,27,27,28,29,30,30,31,32,33,33,34
	dc.b	35,35,36,37,38,38,39,40,41,41,42,43,44,44,45,46
	dc.b	47
MasterVolTab48:
	dc.b	0,0,1,2,3,3,4,5,6,6,7,8,9,9,10,11
	dc.b	12,12,13,14,15,15,16,17,18,18,19,20,21,21,22,23
	dc.b	24,24,25,26,27,27,28,29,30,30,31,32,33,33,34,35
	dc.b	36,36,37,38,39,39,40,41,42,42,43,44,45,45,46,47
	dc.b	48
MasterVolTab49:
	dc.b	0,0,1,2,3,3,4,5,6,6,7,8,9,9,10,11
	dc.b	12,13,13,14,15,16,16,17,18,19,19,20,21,22,22,23
	dc.b	24,25,26,26,27,28,29,29,30,31,32,32,33,34,35,35
	dc.b	36,37,38,39,39,40,41,42,42,43,44,45,45,46,47,48
	dc.b	49
MasterVolTab50:
	dc.b	0,0,1,2,3,3,4,5,6,7,7,8,9,10,10,11
	dc.b	12,13,14,14,15,16,17,17,18,19,20,21,21,22,23,24
	dc.b	25,25,26,27,28,28,29,30,31,32,32,33,34,35,35,36
	dc.b	37,38,39,39,40,41,42,42,43,44,45,46,46,47,48,49
	dc.b	50
MasterVolTab51:
	dc.b	0,0,1,2,3,3,4,5,6,7,7,8,9,10,11,11
	dc.b	12,13,14,15,15,16,17,18,19,19,20,21,22,23,23,24
	dc.b	25,26,27,27,28,29,30,31,31,32,33,34,35,35,36,37
	dc.b	38,39,39,40,41,42,43,43,44,45,46,47,47,48,49,50
	dc.b	51
MasterVolTab52:
	dc.b	0,0,1,2,3,4,4,5,6,7,8,8,9,10,11,12
	dc.b	13,13,14,15,16,17,17,18,19,20,21,21,22,23,24,25
	dc.b	26,26,27,28,29,30,30,31,32,33,34,34,35,36,37,38
	dc.b	39,39,40,41,42,43,43,44,45,46,47,47,48,49,50,51
	dc.b	52
MasterVolTab53:
	dc.b	0,0,1,2,3,4,4,5,6,7,8,9,9,10,11,12
	dc.b	13,14,14,15,16,17,18,19,19,20,21,22,23,24,24,25
	dc.b	26,27,28,28,29,30,31,32,33,33,34,35,36,37,38,38
	dc.b	39,40,41,42,43,43,44,45,46,47,48,48,49,50,51,52
	dc.b	53
MasterVolTab54:
	dc.b	0,0,1,2,3,4,5,5,6,7,8,9,10,10,11,12
	dc.b	13,14,15,16,16,17,18,19,20,21,21,22,23,24,25,26
	dc.b	27,27,28,29,30,31,32,32,33,34,35,36,37,37,38,39
	dc.b	40,41,42,43,43,44,45,46,47,48,48,49,50,51,52,53
	dc.b	54
MasterVolTab55:
	dc.b	0,0,1,2,3,4,5,6,6,7,8,9,10,11,12,12
	dc.b	13,14,15,16,17,18,18,19,20,21,22,23,24,24,25,26
	dc.b	27,28,29,30,30,31,32,33,34,35,36,36,37,38,39,40
	dc.b	41,42,42,43,44,45,46,47,48,48,49,50,51,52,53,54
	dc.b	55
MasterVolTab56:
	dc.b	0,0,1,2,3,4,5,6,7,7,8,9,10,11,12,13
	dc.b	14,14,15,16,17,18,19,20,21,21,22,23,24,25,26,27
	dc.b	28,28,29,30,31,32,33,34,35,35,36,37,38,39,40,41
	dc.b	42,42,43,44,45,46,47,48,49,49,50,51,52,53,54,55
	dc.b	56
MasterVolTab57:
	dc.b	0,0,1,2,3,4,5,6,7,8,8,9,10,11,12,13
	dc.b	14,15,16,16,17,18,19,20,21,22,23,24,24,25,26,27
	dc.b	28,29,30,31,32,32,33,34,35,36,37,38,39,40,40,41
	dc.b	42,43,44,45,46,47,48,48,49,50,51,52,53,54,55,56
	dc.b	57
MasterVolTab58:
	dc.b	0,0,1,2,3,4,5,6,7,8,9,9,10,11,12,13
	dc.b	14,15,16,17,18,19,19,20,21,22,23,24,25,26,27,28
	dc.b	29,29,30,31,32,33,34,35,36,37,38,38,39,40,41,42
	dc.b	43,44,45,46,47,48,48,49,50,51,52,53,54,55,56,57
	dc.b	58
MasterVolTab59:
	dc.b	0,0,1,2,3,4,5,6,7,8,9,10,11,11,12,13
	dc.b	14,15,16,17,18,19,20,21,22,23,23,24,25,26,27,28
	dc.b	29,30,31,32,33,34,35,35,36,37,38,39,40,41,42,43
	dc.b	44,45,46,47,47,48,49,50,51,52,53,54,55,56,57,58
	dc.b	59
MasterVolTab60:
	dc.b	0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
	dc.b	15,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29
	dc.b	30,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44
	dc.b	45,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59
	dc.b	60
MasterVolTab61:
	dc.b	0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
	dc.b	15,16,17,18,19,20,20,21,22,23,24,25,26,27,28,29
	dc.b	30,31,32,33,34,35,36,37,38,39,40,40,41,42,43,44
	dc.b	45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60
	dc.b	61
MasterVolTab62:
	dc.b	0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
	dc.b	15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
	dc.b	31,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45
	dc.b	46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61
	dc.b	62
MasterVolTab63:
	dc.b	0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
	dc.b	15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
	dc.b	31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46
	dc.b	47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62
	dc.b	63
MasterVolTab64:
	dc.b	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	dc.b	16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	dc.b	32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
	dc.b	48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
	dc.b	64


	cnop	0,4
mt_chan1:
	ds.b	n_sizeof
mt_chan2:
	ds.b	n_sizeof
mt_chan3:
	ds.b	n_sizeof
mt_chan4:
	ds.b	n_sizeof

mt_SampleStarts:
	ds.l	31

mt_mod:
	ds.l	1
mt_oldLev6:
	ds.l	1
mt_timerval:
	ds.l	1
mt_oldtimers:
	ds.b	4
mt_MasterVolTab:
	ds.l	1
mt_Lev6Ena:
	ds.w	1
mt_PatternPos:
	ds.w	1
mt_PBreakPos:
	ds.w	1
mt_PosJumpFlag:
	ds.b	1
mt_PBreakFlag:
	ds.b	1
mt_Speed:
	ds.b	1
mt_Counter:
	ds.b	1
mt_SongPos:
	ds.b	1
mt_PattDelTime:
	ds.b	1
mt_PattDelTime2:
	ds.b	1

	xdef	mt_Enable
mt_Enable:
	ds.b	1

	xdef	mt_E8Trigger
mt_E8Trigger:
	ds.b	1

	cnop	0,4
sfx_ptr:
	ds.l	1
sfx_len:
	ds.w	1
sfx_per:
	ds.w	1
sfx_vol:
	ds.w	1
