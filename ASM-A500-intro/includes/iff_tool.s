;*******************************************************************************
;	IFFTool.s
;	Librairie de fonctions pour lire les fichiers IFF (ILBM et 8SVX)
; V1.5 Octobre 2014
;*******************************************************************************

;*******************************************************************************
	SECTION	PROGRAM,CODE
;*******************************************************************************

;*******************************************************************************
; Décode une image IFF (Récupère la palette et l'image)
;	a0.L = adresse de l'image
;	a1.L = adresse du buffer pour la palette
;	a2.L = adresse du buffer pour l'image
; d0.L = retour 0 si erreur, taille image sinon
;*******************************************************************************
DecodePicture:
	movem.l	d1-a6,-(sp)
	lea			IFFBmhd,a6										; Petite séance de nettoyage
	clr.l		(a6)+
	clr.l		(a6)+
	clr.l		(a6)+
	clr.l		(a6)+
	cmp.l		#'FORM',(a0)+									; Est ce un fichier iff
	bne			.BadFileFormat								; On dirait que non
	move.l	(a0)+,d0											; On récupère la taille
	add.l		a0,d0													; L'adresse de fin du fichier
	cmp.l		#'ILBM',(a0)+									; Est ce une image
	bne			.BadFileFormat								; Et non
.NextChunk:
	move.l	(a0)+,d1											; On récupère le chunk
	move.l	(a0)+,d2											; Et sa taille
.BmhdChunk:
	cmp.l		#'BMHD',d1										; Bitmapheader
	bne.s		.CmapChunk										; On passe au suivant
	lea			IFFBmhd,a6										; Pour garder les infos
	move.w	(a0),(a6)+										; Largeur de l'image
	move.w	2(a0),(a6)+										; Hauteur de l'image
	move.b	8(a0),d1
	andi.w	#$000F,d1
	move.w	d1,(a6)+											; Nombre de plans
	move.b	10(a0),d1
	andi.w	#$000F,d1
	move.w	d1,(a6)+											; Compression
	bra.s		.SkipChunk
.CmapChunk:
	cmp.l		#'CMAP',d1										; Colormap
	bne.s		.BodyChunk										; On passe au suivant
	lea			IFFCmap,a6										; Pour garder l'adresse
	move.l	a0,(a6)
	bra.s		.SkipChunk
.BodyChunk:
	cmp.l		#'BODY',d1										; Body
	bne.s		.SkipChunk										; On a pas trouvé de chunk qui nous intéresse
	lea			IFFBody,a6										; Pour garder l'adresse
	move.l	a0,(a6)
.SkipChunk:
	btst		#0,d2													; Taille impaire ?
	beq.s		.EvenSize
	addq.l	#1,d2
.EvenSize:
	add.l		d2,a0													; On passe tout le chunk
	cmp.l		d0,a0													; On a atteind la fin du fichier ?
	blt.s		.NextChunk										; Non on continu
.CheckData:
	lea			IFFBmhd,a6										; Maintenant on regarde si on a tout récupéré
	move.w	(a6),d0
	beq.s		.BadFileFormat								; Pas la largeur, on arrête
	move.w	2(a6),d0
	beq.s		.BadFileFormat								; Pas la hauteur, on arrête
	move.w	4(a6),d0
	beq.s		.BadFileFormat								; Pas la profondeur, on arrête
	move.l	8(a6),d0
	beq.s		.BadFileFormat								; Pas la palette, on arrête
	move.l	12(a6),d0
	beq.s		.BadFileFormat								; Pas l'image, on arrête
.DecodePal:
	moveq.l	#1,d0
	move.w	4(a6),d2											; Nombre de plans
	rol.w		d2,d0													; Nombre de couleurs
	move.l	IFFCmap,a0										; Adresse de la palette dans l'image
	jsr			DecodeIFFPalette							; Décode la palette
.DecodeBody:
	moveq.l	#0,d0
	move.w	(a6),d0												; Largeur en pixels
	lsr.l		#3,d0													; Divisée par 8 pour avoir la taille en octets
	moveq.l	#0,d1
	move.w	2(a6),d1											; Hauteur en pixels
	mulu.w	d2,d1													; Multiplié par le nombre de plans
	move.l	IFFBody,a0										; Adresse de l'image à décoder
	move.l	a2,a1													; Buffer de sauvegarde de l'image
	jsr			DecodeIFFBody									; Et on décompresse
	mulu.w	d1,d0													; Taille de l'image
	bra.s		.EndDecode
.BadFileFormat:
	clr.l		d0														; Signale une erreur
.EndDecode:
	movem.l	(sp)+,d1-a6
	rts

;*******************************************************************************
; Transforme une palette IFF en palette chargeable en registre
;	d0.W = nombre de couleurs
;	a0.L = adresse palette IFF
; a1.L = adresse buffer palette
;*******************************************************************************
;	Bits		1F. . . . . . . . . . . . . .10 F . . . . . . . . . . . . . . 0
; IFF			x x x x x x x x R R R R r r r r V V V V v v v v B B B B b b b b
; Amiga		x x x x R R R R V V V V B B B B x x x x r r r r v v v v b b b b
;*******************************************************************************
DecodeIFFPalette:
	movem.l	d0-a6,-(sp)
	subq.w	#1,d0
.NextColor:
	move.b	(a0)+,d1											; Hop le rouge
	move.b	(a0)+,d2											; Hop le vert
	move.b	(a0)+,d3											; Hop le bleu
	move.b	d1,d4													; On commence par le rouge
	andi.w	#$00F0,d4											; On garde les 4 bits fort
	rol.w		#4,d4													; Que l'on met à la bonne place
	move.b	d2,d4
	andi.w	#$0FF0,d4											; Voilà pour le vert
	move.b	d3,d5
	andi.w	#$00F0,d5											; Toujours les 4 bits fort du bleu
	ror.w		#4,d5													; Que l'on place
	or.w		d5,d4													; Et on fini la couleur
	move.w	d4,(a1)+											; On sauve le tout
	andi.w	#$000F,d1
	rol.w		#8,d1													; On place les 4 bits faibles du rouge
	andi.w	#$000F,d2
	rol.w		#4,d2													; On place les 4 bits faibles du vert
	andi.w	#$000F,d3											; On isole les 4 bits faibles du bleu
	or.w		d2,d3
	or.w		d1,d3													; On combine le rouge et le vert
	move.w	d3,(a1)+											; On sauve le tout
	dbf			d0,.NextColor
	movem.l	(sp)+,d0-a6
	rts

;*******************************************************************************
; Décompacte une image IFF (Byterun algorythme)
;	d0.W = largeur en octets d'un plan
; d1.W = hauteur en lignes * nbr de plans
;	a0.L = adresse source
;	a1.L = adresse destination
;*******************************************************************************
DecodeIFFBody:
	movem.l	d0-a6,-(sp)
	lea 		IFFBmhd,a6
	tst.w		6(a6)													; Image compressé ?
	beq			.CopyBody											; Non, on recopie directement
	subq.w	#1,d1
.DecompLine:
	move.w	d0,d7
.DecompByte:
	moveq.l	#0,d2
	move.b	(a0)+,d2
	cmpi.w	#128,d2
	beq.s		.DecompByte
	bgt.s		.Greater128
.Lesser128:
	sub.w		d2,d7
	subq.w	#1,d7
.CopyPattern:
	move.b	(a0)+,(a1)+
	dbf			d2,.CopyPattern
	tst.w		d7
	bne.s		.DecompByte
	dbf			d1,.DecompLine
	movem.l	(sp)+,d0-a6
	rts
.Greater128:
	move.w	#256,d3
	sub.w		d2,d3
	sub.w		d3,d7
	subq.w	#1,d7
	move.b	(a0)+,d2
.ExtendByte:
	move.b	d2,(a1)+
	dbf			d3,.ExtendByte
	tst.w		d7
	bne.s		.DecompByte
	dbf			d1,.DecompLine
	movem.l	(sp)+,d0-a6
	rts
.CopyBody:
	subq.w	#1,d0													; Octets à copier par ligne
	subq.w	#1,d1													; Lignes à copier
.CopyLine:
	move.w	d0,d2
.CopyByte:
	move.b	(a0)+,(a1)+
	dbf			d2,.CopyByte
	dbf			d1,.CopyLine
	movem.l	(sp)+,d0-a6
	rts

;*******************************************************************************
; Charge une palette de couleurs 12 bits pour ECS/OCS
; d0.W = nombre de couleurs (max 32)
; d1.W = couleur de départ de la palette (max 31)
; a0.L = adresse du buffer des couleurs (codées 32 bits)
;*******************************************************************************
SetPalette:
	movem.l	d0-a6,-(sp)
	lea			CUSTOM+COLOR00,a1							; Premier registre des couleurs
	subq.w	#1,d0													; Nombre de couleurs à charger
	andi.w	#$1F,d0												; 32 couleurs max à charger
	move.w	d1,d3													; On isole le registre de couleur
	andi.w	#$1F,d1												; On ne garde que les bits 0 à 4 (32 registres)
	add.w		d1,d1													; Multiplié par 2
	adda.w	d1,a1													; Registre de départ
.LoadColor:
	move.w	(a0),(a1)+										; Charge les composantes de poids fort
	lea			4(a0),a0											; Couleur suivante
	dbf			d0,.LoadColor
	movem.l	(sp)+,d0-a6
	rts

;*******************************************************************************
; Charge une palette de couleurs 32 bits pour AGA
; d0.W = nombre de couleurs
; d1.W = couleur de départ de la palette
; a0.L = adresse du buffer des couleurs (codées en 32 bits)
;*******************************************************************************
SetAGAPalette:
	movem.l	d0-a6,-(sp)
	lea			CUSTOM+COLOR00,a1							; Premier registre des couleurs
	subq.w	#1,d0													; Nombre de couleurs à charger
.LoadColor:
	move.w	d1,d2													; On va isoler d'abord la bank de couleur
	andi.w	#$E0,d2												; On ne garde que les bits 5, 6 et 7 (8 banks)
	lsl.w		#8,d2													; On les place dans l'octet de poids fort
	move.w	d1,d3													; On isole ensuite le registre de couleur
	andi.w	#$1F,d3												; On ne garde que les bits 0 à 4 (32 registres)
	andi.w	#$FDFF,d2											; Clear le bit pour charger rgb poids fort
	move.w	d2,CUSTOM+BPLCON3							; Et on écrit
	add.w		d3,d3													; d3 * 2
	move.w	(a0)+,(a1,d3.w)								; Charge les composantes de poids fort
	ori.w		#$0200,d2											; Set le bit pour charge rgb poids faible
	move.w	d2,CUSTOM+BPLCON3							; Et on écrit
	move.w	(a0)+,(a1,d3.w)								; Charge les composantes de poids faible
	addq.w	#1,d1													; Couleur suivante
	dbf			d0,.LoadColor
	movem.l	(sp)+,d0-a6
	rts

;*******************************************************************************
; Décode un son 8SVX (Récupère le sample, sa longueur, son bitrate et son volume)
;	a0.L = adresse du son
;	a1.L = adresse du buffer pour le sample
; d0.L = retour 0 si erreur, taille sample sinon
;	d1.L = H : fréquence, L : volume
;*******************************************************************************
DecodeSound:
	movem.l	d2-a6,-(sp)
	lea			SNDVhdr,a6										; Petite séance de nettoyage
	clr.l		(a6)+
	clr.l		(a6)+
	clr.l		(a6)+
	cmp.l		#'FORM',(a0)+									; Est ce un fichier iff
	bne			.BadFileFormat								; On dirait que non
	move.l	(a0)+,d0											; On récupère la taille
	add.l		a0,d0													; L'adresse de fin du fichier
	cmp.l		#'8SVX',(a0)+									; Est ce un son
	bne			.BadFileFormat								; Et non
.NextChunk:
	move.l	(a0)+,d1											; On récupère le chunk
	move.l	(a0)+,d2											; Et sa taille
.BmhdChunk:
	cmp.l		#'VHDR',d1										; Voiceheader
	bne.s		.BodyChunk										; On passe au suivant
	lea			SNDVhdr,a6										; Pour garder les infos
	move.w	2(a0),(a6)+										; Nombre de samples
	move.w	12(a0),(a6)+									; Fréquence d'échantillonage
	move.l	16(a0),d3											; Volume, en virgule fixe
	move.w	#AUDIO_MAXVOLUME,d4						; Volume max amiga
	move.w	d4,d5
	mulu.w	d3,d4													; Partie décimale
	swap		d3
	mulu.w	d3,d5
	swap		d5														; Partie entière
	add.l		d4,d5													; Volume en fixed float
	swap		d5														; On ne garde que la partie entière
	move.w	d5,(a6)+
	move.b	15(a0),d3
	and.w		#$000F,d3
	move.w	d3,(a6)+											; Compression
	bra.s		.SkipChunk
.BodyChunk:
	cmp.l		#'BODY',d1										; Body
	bne.s		.SkipChunk										; On a pas trouvé de chunk qui nous intéresse
	lea			SNDBody,a6										; Pour garder l'adresse
	move.l	a0,(a6)
.SkipChunk:
	btst		#0,d2													; Taille impaire ?
	beq.s		.EvenSize
	addq.l	#1,d2
.EvenSize:
	add.l		d2,a0													; On passe tout le chunk
	cmp.l		d0,a0													; On a atteind la fin du fichier ?
	blt.s		.NextChunk										; Non on continu
.CheckData:
	lea			SNDVhdr,a6										; Maintenant on regarde si on a tout récupéré
	move.w	(a6),d0
	beq.s		.BadFileFormat								; Pas de sample, on arrête
	move.w	2(a6),d0
	beq.s		.BadFileFormat								; Pas de fréquence, on arrête
	move.l	8(a6),d0
	beq.s		.BadFileFormat								; Pas de son, on arrête
.DecodeBody:
	moveq.l	#0,d0
	move.w	(a6),d0												; Nombre de samples
	move.l	SNDBody,a0										; Adresse du son à décoder
	jsr			DecodeSNDBody									; Et on décompresse
	move.l	2(a6),d1
	bra.s		.EndDecode
.BadFileFormat:
	clr.l		d0														; Signale une erreur
.EndDecode:
	movem.l	(sp)+,d2-a6
	rts

;*******************************************************************************
; Décompacte un son 8SVX (Fibonacci Delta Compression algorythme)
;	d0.W = longueur du sample
;	a0.L = adresse source
;	a1.L = adresse destination
;*******************************************************************************
DecodeSNDBody:
	movem.l	d0-a6,-(sp)
	lea 		SNDVhdr,a6
	tst.w		6(a6)													; Son compressé ?
	beq			.CopyBody											; Non, on recopie directement
	move.b	(a0)+,d1											; Le premier octet sert de padding
	move.b	(a0)+,d1											; Le deuxième octet sert de point de départ pour la décompression
	lea			SNDTable,a2										; Table des deltas
	lsr.w		#1,d0													; On traite deux sample par boucle
	subq.w	#1,d0
.DecompSample:
	move.b	(a0)+,d2											; Deux samples de 4 bit
	move.b	d2,d3
	lsl.b		#4,d2
	andi.w	#$F,d2												; Bits de poids fort, premier delta
	add.b		(a2,d2.w),d1									; Premier sample
	move.b	d1,(a1)+
	andi.w	#$F,d3												; Bits de poids faible, second delta
	add.b		(a2,d3.w),d1									; Deuxième sample
	move.b	d1,(a1)+
	dbf			d0,.DecompSample							; Deltas suivant
	movem.l	(sp)+,d0-a6
	rts
.CopyBody:
	subq.w	#1,d0													; Octets à copier pour le son
.CopyByte:
	move.b	(a0)+,(a1)+
	dbf			d0,.CopyByte
	movem.l	(sp)+,d0-a6
	rts

;*******************************************************************************
	SECTION	IFFTOOL,DATA
;*******************************************************************************

IFFBmhd:
	dc.w		0,0,0,0												; Largeur, Hauteur, Plan, Compression
IFFCmap:
	dc.l		0															; Adresse de la palette
IFFBody:
	dc.l		0															; Adresse de l'image

SNDVhdr:
	dc.w		0,0,0,0												; Samples, Fréquence, Volume, Compression
SNDBody:
	dc.l		0															; Adresse du sample

SNDTable:
	dc.b		-34,-21,-13,-8,-5,-3,-2,-1
	dc.b		0,1,2,3,5,8,13,21							; Table de décompression du son
