/*
	Secret (demo) Project
	by Mandarine
*/

#include "includes.prl"
#include "time.h"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>
#include <hardware/custom.h>
#include <graphics/gfxmacros.h>

/*
Common
*/
#include "board.h"
#include "ptreplay.h"
#include "ptreplay_protos.h"
#include "ptreplay_pragmas.h"

/*
Routines
*/
#include "bitmap_routines.h"
#include "font_routines.h"
#include "sound_routines.h"
#include "font_desc.h"
#include "font_bitmap.h"
#include "font_routines.h"
#include "color_routines.h"
#include "voxel_routines.h"
#include "fx_routines.h"
#include "demo_strings.h"
#include "cosine_table.h"

/*
Graphic assets
*/
#include "screen_size.h"
#include "copper_lists.h"

/* Music */
struct Library *PTReplayBase;
struct Module *theMod;
UBYTE *mod = NULL;

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct Custom far custom;

struct Task *myTask = NULL;
BYTE oldPri;

struct View my_view;
struct View *my_old_view;

/*
	Double buffer and scrolling
*/
short scr1_x_offset = 0, scr1_y_offset = 0;
short scr2_x_offset = 0, scr2_y_offset = 0;
int scr2_x_offset_half_precision = 0;
int dbuffer_offset_1 = 0;
int dbuffer_offset_2 = 0;
BOOL enable_dbuffer_1 = FALSE;
BOOL enable_dbuffer_2 = FALSE;

/* ViewPort 1 */
struct ViewPort view_port1;
struct RasInfo ras_info1;
struct RasInfo ras_info1_1b;
struct BitMap bit_map1;
struct BitMap bit_map1_1b;
struct BitMap bit_map1_1b2;
struct BitMap bit_map1_1b3;
struct RastPort rast_port1;
struct RastPort rast_port1_1b;
struct RasInfo ras_info2;
struct BitMap bit_map2;
struct RastPort rast_port2;

/* Data */
UBYTE demo_string_index = 0;
struct BitMap *bitmap_font = NULL;

extern ULONG redbotPaletteRGB32[50];
extern ULONG astronautPaletteRGB32[50];
extern ULONG demo_titlePaletteRGB32[50];
extern ULONG facePaletteRGB32[50];
extern ULONG guardPaletteRGB32[50];
extern ULONG mountainPaletteRGB32[26];
extern ULONG mummyPaletteRGB32[50];
extern ULONG ufoPaletteRGB32[50];

struct BitMap *bitmap_redbot = NULL;
struct BitMap *bitmap_astronaut = NULL;
struct BitMap *bitmap_demo_title = NULL;
struct BitMap *bitmap_face = NULL;
struct BitMap *bitmap_guard = NULL;
struct BitMap *bitmap_mountain = NULL;
struct BitMap *bitmap_mummy = NULL;
struct BitMap *bitmap_ufo = NULL;

BOOL voxel_switch = FALSE;

#define PTREPLAY_MUSIC
struct SoundInfo *background = NULL;

#define SET_PALETTE_MACRO(PAL_NAME, COLOR_AMOUNT) \
{ \
	short loop, idx = 1; \
	unsigned long r, g, b; \
	for(loop = 0; loop < COLOR_AMOUNT; loop++) \
	{ \
		r = PAL_NAME[idx++]; \
		g = PAL_NAME[idx++]; \
		b = PAL_NAME[idx++]; \
		SetRGB32(&view_port1, loop + 8, r, g, b); \
	} \
};

UBYTE is_cpu_a_020(void)
{
	printf("Kickstart v%d, ", SysBase->LibNode.lib_Version);

	if (SysBase->AttnFlags && AFF_68020)
	{
		printf("CPU is a 68020 or higher.\n");
		return 1;
	}

	printf("CPU is a 68000 or 68010.\n");
	return 0;
}

void initMusic(void)
{
#ifdef PTREPLAY_MUSIC
	if (SysBase->LibNode.lib_Version >= 36)
		if (!AssignPath("Libs","Libs"))
			exit(0); //FIXME // init_conerr((UBYTE *)"Failed to Assign the local Libs drawer. Please copy ptreplay.library into your Libs: drawer.\n");

	if (!(PTReplayBase = OpenLibrary((UBYTE *)"ptreplay.library", 0)))
	{
		exit(0); //FIXME
	}

	mod = load_zlib_getchipmem((UBYTE *)"assets/bridgesoftime.data", 0, 201310);
	// mod = NULL;
	// mod = load_getchipmem((UBYTE *)"assets/miami_vice.mod", 7394);
#else
	background = PrepareSound( "assets/music_loop.8svx" );
	if( !background )
	{
		printf( "Could not prepare the sound effect!\n" );
		exit(0); //FIXME
	}
#endif
}

void playMusic(void)
{
#ifdef PTREPLAY_MUSIC
	if (mod != NULL)
	{
		theMod = PTSetupMod((APTR)mod);
		PTPlay(theMod);
	}
#else
	if (background != NULL)
	{
		PlaySound( background, MAXVOLUME, LEFT0, NORMALRATE, NONSTOP );
		PlaySound( background, MAXVOLUME, RIGHT0, NORMALRATE, NONSTOP );
	}
#endif
}

/* Returns all allocated resources: */
void close_demo(STRPTR message)
{
	int loop;

	WaitBlit();

	FREE_ALLOCATED_BITMAP(bitmap_font);

	freeRedbotSprite();
	freeAstronautSprite();
	freeTitleSprite();
	freeFaceSprite();
	freeGuardSprite();
	freeMountainSprite();
	freeMummySprite();
	freeUfoSprite();	

	/*	Free the voxel structures */
	deleteMatrix();
	deletePointList();

	/* Free automatically allocated display structures: */
	FreeVPortCopLists( &view_port1 );
	FreeCprList( my_view.LOFCprList );

	/* Deallocate the display memory, BitPlane for BitPlane: */
	for( loop = 0; loop < DEPTH1; loop++ )
		if( bit_map1.Planes[ loop ] )
			FreeRaster( bit_map1.Planes[ loop ], WIDTH1, HEIGHT1 + 8);

	for( loop = 0; loop < DEPTH1; loop++ )
		if( bit_map2.Planes[ loop ] )
			FreeRaster( bit_map2.Planes[ loop ], WIDTH1, HEIGHT1 + 8);		

#ifdef PTREPLAY_MUSIC
	/*	Stop music */
	if (mod != NULL)
	{
		PTStop(theMod);
		PTFreeMod(theMod);
		FreeMem(mod, 201310);
	}

	if (PTReplayBase) CloseLibrary(PTReplayBase);
#else
	if (background != NULL)
	{
		StopSound( LEFT0 );
		StopSound( RIGHT0 );
		RemoveSound( background );
	}
#endif
	/* Close the Graphics library: */
	if(GfxBase)
		CloseLibrary((struct Library *)GfxBase);

	/* C Close the Intuition library:  */
	if(IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);

	/* Restore the old View: */
	LoadView( my_old_view );

	/* Print the message and leave: */
	printf( "%s\n", message );

	if (myTask != NULL)
		SetTaskPri(myTask, oldPri);

	exit(0);
}

void __inline swapDoubleBuffer1(void)
{
	if (dbuffer_offset_1 == 0)
		dbuffer_offset_1 = DISPL_WIDTH1;
	else
		dbuffer_offset_1 = 0;
}

void __inline swapDoubleBuffer2(void)
{		
	if (dbuffer_offset_2 == 0)
		dbuffer_offset_2 = DISPL_WIDTH1;
	else
		dbuffer_offset_2 = 0;
}

void __inline resetViewportOffset(void)
{
	dbuffer_offset_1 = 0;
	dbuffer_offset_2 = 0;

	(&view_port1)->RasInfo->RxOffset = 0;
	(&view_port1)->RasInfo->RyOffset = 0;
	(&view_port1)->RasInfo->Next->RxOffset = 0;
	(&view_port1)->RasInfo->Next->RyOffset = 0;
}

void setPaletteToBlack(void);
void fxVoxelRotation(UWORD *angle);
void loadTextWriterFont(void);
// void setPaletteRedbot(void);

void main()
{
	int loop;
	unsigned int fx_clock = 0;
	unsigned short demo_phase = 0;
	unsigned int palette_fade;
	unsigned int prev_clock;

	UWORD angle;

	/* Open the Intuition library: */
	IntuitionBase = (struct IntuitionBase *)
	OpenLibrary( "intuition.library", 0 );
	if( !IntuitionBase )
		close_demo( "Could NOT open the Intuition library!" );

	/* Open the Graphics library: */
	GfxBase = (struct GfxBase *)
	OpenLibrary( "graphics.library", 0 );
	if( !GfxBase )
		close_demo( "Could NOT open the Graphics library!" );

	printf("DEMO IS LOADING!\n");

	is_cpu_a_020();

	/*	Loading sequence 
		loads everything! 
	*/
	printf("\n");
	printf("                       ------------------\n");
	printf("               MINIZ DECRUNCHES WHILE U WAITING...\n");
	printf("                       ------------------\n");	

	// initMusic();
	loadRedbotSprite();
	loadAstronautSprite();
	loadTitleSprite();
	loadFaceSprite();
	loadGuardSprite();
	loadMountainSprite();
	loadMummySprite();
	loadUfoSprite();

	/* Save the current View, so we can restore it later: */
	my_old_view = GfxBase->ActiView;

	/* Prepare the View structure, and give it a pointer to */
	/* the first ViewPort:                                  */
	InitView( &my_view );
	my_view.ViewPort = &view_port1;

	/* 2. Prepare the ViewPort structures, and set some important values: */

	/* ViewPort 1 */
	InitVPort( &view_port1 );
	view_port1.DWidth = DISPL_WIDTH1;      /* Set the width.                */
	view_port1.DHeight = DISPL_HEIGHT1;    /* Set the height.               */
	view_port1.DxOffset = 0;         /* X position.                   */
	view_port1.DyOffset = 0;         /* Y position.                   */
	view_port1.RasInfo = &ras_info1; /* Give it a pointer to RasInfo. */
	view_port1.Modes = DUALPF|PFBA;         /* Low resolution.               */
	view_port1.Next = NULL;   /* Pointer to next ViewPort.     */

	/* 3. Get a colour map, link it to the ViewPort, and prepare it: */

	/* ViewPort 1 */
	view_port1.ColorMap = (struct ColorMap *) GetColorMap(256);
	if( view_port1.ColorMap == NULL )
		close_demo( "Could NOT get a ColorMap!" );

	/* Prepare the BitMaps */
	/* ViewPort 1, Bitmap 1 */
	InitBitMap( &bit_map1, DEPTH1, WIDTH1, HEIGHT1 + 8);
	InitBitMap( &bit_map1_1b, 1, WIDTH1, HEIGHT1 + 8);
	InitBitMap( &bit_map1_1b2, 1, WIDTH1, HEIGHT1 + 8);
	InitBitMap( &bit_map1_1b3, 1, WIDTH1, HEIGHT1 + 8);		
	
	/* Allocate memory for the Raster: */ 
	for( loop = 0; loop < DEPTH1; loop++ )
	{
		bit_map1.Planes[ loop ] = (PLANEPTR) AllocRaster( WIDTH1, HEIGHT1 + 8);
		if( bit_map1.Planes[ loop ] == NULL )
			close_demo( "Could NOT allocate enough memory for the raster!" );
	/* Clear the display memory with help of the Blitter: */
		BltClear( bit_map1.Planes[ loop ], RASSIZE( WIDTH1, HEIGHT1 + 8), 0 );
	}

	bit_map1_1b.Planes[0] = bit_map1.Planes[0];
	bit_map1_1b2.Planes[0] = bit_map1.Planes[1];
	bit_map1_1b3.Planes[0] = bit_map1.Planes[2];

	/* ViewPort 1, Bitmap 2 */
	InitBitMap( &bit_map2, DEPTH1, WIDTH1, HEIGHT1 + 8);
	/* Allocate memory for the Raster: */ 
	for( loop = 0; loop < DEPTH1; loop++ )
	{
		bit_map2.Planes[ loop ] = (PLANEPTR) AllocRaster( WIDTH1, HEIGHT1 + 8);
		if( bit_map2.Planes[ loop ] == NULL )
			close_demo( "Could NOT allocate enough memory for the raster!" );
	/* Clear the display memory with help of the Blitter: */
		BltClear( bit_map2.Planes[ loop ], RASSIZE( WIDTH1, HEIGHT1 + 8), 0 );
	}

	/* Prepare the RasInfo structure */

	/* ViewPort 1, Raster 1 */
	ras_info1.BitMap = &bit_map1; /* Pointer to the BitMap structure.  */
	ras_info1.RxOffset = 0;       /* The top left corner of the Raster */
	ras_info1.RyOffset = 0;       /* should be at the top left corner  */
	              /* of the display.                   */
	ras_info1.Next = &ras_info2;        /* Dual playfield */

	/* ViewPort 1, Raster 2 */
	ras_info2.BitMap = &bit_map2; /* Pointer to the BitMap structure.  */
	ras_info2.RxOffset = 0;       /* The top left corner of the Raster */
	ras_info2.RyOffset = 0;       /* should be at the top left corner  */
	              					/* of the display.                   */
	ras_info2.Next = NULL;        /* Single playfield - only one       */
	              				/* RasInfo structure is necessary.   */

	ras_info1_1b.BitMap = &bit_map1_1b;

	/* Prepare the RastPort, and give it a pointer to the BitMap. */
	/* ViewPort 1 */
	InitRastPort( &rast_port1 );
	rast_port1.BitMap = &bit_map1;
	InitRastPort( &rast_port2 );
	rast_port2.BitMap = &bit_map2;

	InitRastPort( &rast_port1_1b );
	rast_port1_1b.BitMap = &bit_map1_1b;	

	/* Create the display */
	MakeVPort(&my_view, &view_port1); /* Prepare ViewPort 1 */

	WaitTOF();

	// setCityCopperList(&view_port1);

	MrgCop(&my_view);

	/* 8. Show the new View: */
	LoadView( &my_view );

	WaitTOF();
	WaitTOF();

	myTask = FindTask(NULL);
	oldPri = SetTaskPri(myTask, 127);	

	Forbid();
	Disable();
	WaitBlit();
	
	OFF_SPRITE;
	// OFF_VBLANK;

	setupMatrix();
	// fillMatrixWithRandomData();
	buildLinesListAsCube(1);
	angle = 0;

	// setPaletteFacingCar();
	// setPaletteToBlack();

	// drawElementCity(&bit_map1);
	// drawElementCity(&bit_map2);
	// LoadRGB4(&view_port1, trabant_facing_groundPaletteRGB4, 8);

	WaitTOF();

	playMusic();

	#define DMPHASE_0		0
	#define DMPHASE_1		(1 << 4)
	#define DMPHASE_2		(2 << 4)
	#define DMPHASE_3		(3 << 4)
	#define DMPHASE_4		(4 << 4)
	#define DMPHASE_5		(5 << 4)
	#define DMPHASE_6		(6 << 4)
	#define DMPHASE_7		(7 << 4)
	#define DMPHASE_8		(8 << 4)

	demo_phase = 0;
	fx_clock = 0;

	while((*(UBYTE *)0xBFE001) & 0x40)
	{
		// fxVoxelRotation(&angle);
		// angle++;
		// printf("demo_phase = %d\n", demo_phase);
		// printf("Songpos = %d, Patpos = %d\n", PTSongPos(theMod), PTPatternPos(theMod));

		// if(0)
		switch(demo_phase)
		{
			/*	
				Phase 0
			***********************/
			case DMPHASE_0:
				palette_fade = 0;
				buildLinesListAsCube(1);
				resetViewportOffset();
				SET_PALETTE_MACRO(redbotPaletteRGB32, 16);
				drawRedbotSprite(&bit_map2);
				demo_phase++;
				break;

			case DMPHASE_0 | 1:
				if (1) // ((PTSongPos(theMod) == 0 && PTPatternPos(theMod) > 0x10) || (PTSongPos(theMod) > 0))
					demo_phase++;
				break;

			case DMPHASE_0 | 2:
				fxVoxelRotation(&angle);
				// LoadRGB4(&view_port1, pal_mistral_title_fadein + (palette_fade << 4), 16);
				palette_fade++;		

				if (palette_fade >= 16)
				{
					palette_fade = 0;
					fx_clock = 0;
					demo_phase++;
				}
				break;

			case DMPHASE_0 | 3:
				fxVoxelRotation(&angle);
				if (fx_clock++ > 180)
					demo_phase++;
				break;

			case DMPHASE_0 | 4:
				// setPaletteToBlack();
				demo_phase++;
				break;														

			/*	Clear the screen */
			case DMPHASE_0 | 5:
				if (progressiveClearRaster(&rast_port1, fx_clock, WIDTH1, HEIGHT1, 0))
					fx_clock++;
				else
				{
					fx_clock = 0;
					demo_phase++;
				}
				break;

			case DMPHASE_0 | 6:
				if (progressiveClearRaster(&rast_port2, fx_clock, WIDTH1, HEIGHT1, 0))
					fx_clock++;
				else
				{
					fx_clock = 0;
					demo_phase++;
				}
				break;						

			/*	Next fx!!! */
			case DMPHASE_0 | 7:
				resetViewportOffset();
				demo_phase = DMPHASE_1;
				break;

			/*	
				Phase 1
			***********************/
			case DMPHASE_1:
				palette_fade = 0;
				buildLinesListAsCube(2);
				resetViewportOffset();
				SET_PALETTE_MACRO(astronautPaletteRGB32, 16);
				drawAstronautSprite(&bit_map2);
				demo_phase++;
				break;

			case DMPHASE_1 | 1:
				if (1) // ((PTSongPos(theMod) == 0 && PTPatternPos(theMod) > 0x10) || (PTSongPos(theMod) > 0))
					demo_phase++;
				break;

			case DMPHASE_1 | 2:
				fxVoxelRotation(&angle);
				// LoadRGB4(&view_port1, pal_mistral_title_fadein + (palette_fade << 4), 16);
				palette_fade++;		

				if (palette_fade >= 16)
				{
					palette_fade = 0;
					fx_clock = 0;
					demo_phase++;
				}
				break;

			case DMPHASE_1 | 3:
				fxVoxelRotation(&angle);
				if (fx_clock++ > 180)
					demo_phase++;
				break;

			case DMPHASE_1 | 4:
				// setPaletteToBlack();
				demo_phase++;
				break;														

			/*	Clear the screen */
			case DMPHASE_1 | 5:
				if (progressiveClearRaster(&rast_port1, fx_clock, WIDTH1, HEIGHT1, 0))
					fx_clock++;
				else
				{
					fx_clock = 0;
					demo_phase++;
				}
				break;

			case DMPHASE_1 | 6:
				if (progressiveClearRaster(&rast_port2, fx_clock, WIDTH1, HEIGHT1, 0))
					fx_clock++;
				else
				{
					fx_clock = 0;
					demo_phase++;
				}
				break;						

			/*	Next fx!!! */
			case DMPHASE_1 | 7:
				resetViewportOffset();
				demo_phase = DMPHASE_0;
				break;															
		}

		if (enable_dbuffer_1)
			swapDoubleBuffer1();
		if (enable_dbuffer_2)
			swapDoubleBuffer2();

		(&view_port1)->RasInfo->RxOffset = scr1_x_offset + dbuffer_offset_1;
		(&view_port1)->RasInfo->RyOffset = scr1_y_offset;

		(&view_port1)->RasInfo->Next->RxOffset = scr2_x_offset + dbuffer_offset_2;
		(&view_port1)->RasInfo->Next->RyOffset = scr2_y_offset;

		ScrollVPort(&view_port1);	

		WaitTOF();
	}

	// ON_VBLANK;

	WaitTOF();	

	/*	Wait for mouse up
		to prevent the mouse up event
		to mess with the OS gui
	*/
	while(!((*(UBYTE *)0xBFE001) & 0x40))
		WaitTOF();

	ON_SPRITE;

	Enable();
	Permit();

	close_demo("Stay 16/32!");
}

void setPaletteToBlack(void)
{
	short loop;
	for(loop = 1; loop < 16; loop++)
		SetRGB4(&view_port1, loop, 0x0, 0x0, 0x0);
}

void loadTextWriterFont(void)
{
    bitmap_font = load_array_as_bitmap(font_data, 288 << 1, font_image.Width, font_image.Height, font_image.Depth);
}

void __inline fxVoxelRotation(UWORD *angle)
{
	if (voxel_switch)
	{
		// swapDoubleBuffer1();

		SetAPen(&rast_port1_1b, 0);
		RectFill(&rast_port1_1b, (DISPL_WIDTH1 >> 1) - 48 + dbuffer_offset_1, (DISPL_HEIGHT1 >> 1) - 48, (DISPL_WIDTH1 >> 1) + 48 + dbuffer_offset_1, (DISPL_HEIGHT1 >> 1) + 48);
		drawLinesListToViewport(&rast_port1_1b, dbuffer_offset_1);
		(*angle) += 2;
		(*angle) &= 0x1FF;
	}
	else
		rotatePointsOnAxisY(*angle);

	voxel_switch = !voxel_switch;
	// angle++;
}
