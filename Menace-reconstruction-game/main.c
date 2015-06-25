/*
File: menace.c

What follows is ta reconstruction of the Amiga game Menace, originally coded in 68000 assembler by David Jones of DMA Design, and published by Psygnosis in 1988. This version was coded by Jimmy Maher in 2010, and may be freely distributed.

This program was developed with Lattice C 5.02 running under KickStart and Workbench 1.3, and therefore should be certain compile successfully in that environment. Your milage may vary with other environments.

This code is best understood in tandem with Chapter 8 of my book "The Future Was Here: The Commodore Amiga," which describes its techniques at a much higher level of abstraction than the comments which follow in the source proper.
*/

#include "includes.prl"
#include <exec/types.h>
#include <exec/memory.h>
#include <dos.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include <hardware/custom.h>
#include "prototypes.h"

struct GfxBase *GraphicsBase;
extern struct DosLibrary *DOSBase;
UBYTE *MemBase;
struct Custom *Hardware;

UWORD SystemInts;
UWORD SystemDMA;

UBYTE *ICRA=(UBYTE *)0xbfed01;
UBYTE *PortA=(UBYTE *)0xbfe001;

ULONG Level2Vector;
ULONG Level3Vector;

UWORD __chip copperlist[] = {
   0xa01,0xff00,

   bplpt,0,bplpt+2,0,
   bplpt+8,0,bplpt+10,0,
   bplpt+16,0,bplpt+18,0,
   bplpt+4,0,bplpt+6,0,
   bplpt+12,0,bplpt+14,0,
   bplpt+20,0,bplpt+22,0,
   bplcon0,0x6600,
   
   bplcon1,0xff,bpl1mod,0x36,
   bpl2mod,0x2e,bplcon2,0x44,
   ddfstrt,0x28,ddfstop,0xd8,
   diwstrt,0x1f78,diwstop,0xffc6,

   color,0,color+2,0,
   color+4,0,color+6,0,
   color+8,0,color+10,0,
   color+12,0,color+14,0,
   color+16,0,color+18,0,
   color+20,0,color+22,0,
   color+24,0,color+26,0,
   color+28,0,color+30,0,
   color+32,0,color+34,0,
   color+36,0,color+38,0,
   color+40,0,color+42,0,
   color+44,0,color+46,0,
   color+48,0,color+50,0,
   color+52,0,color+54,0,
   color+56,0,color+58,0,
   color+60,0,color+62,0,

   sprpt,0,sprpt+2,0,
   sprpt+4,0,sprpt+6,0,
   sprpt+8,0,sprpt+10,0,
   sprpt+12,0,sprpt+14,0,
   sprpt+16,0,sprpt+18,0,
   sprpt+20,0,sprpt+22,0,
   sprpt+24,0,sprpt+26,0,
   sprpt+28,0,sprpt+30,0,
   
   0xdf01,0xff00,
   bplcon1,0x0,bplcon0,0x4200,ddfstrt,0x30,
   
   bplpt,0,bplpt+2,0,
   bplpt+4,0,bplpt+6,0,
   bplpt+8,0,bplpt+10,0,
   bplpt+12,0,bplpt+14,0,
   
   color+20,0,color+30,0,
   color+2,0,color+4,0,
   color+6,0,color+8,0,
   color+10,0,color+12,0,
   color+14,0,color+16,0,
   color+18,0,color+22,0,
   color+24,0,color+26,0,
   color+28,0,color,0,
   bpl1mod,0x0,bpl2mod,0x0,
   0xdf01,0xff00,INTREQ,0x8010,
   0xffff,0xfffe
};

UWORD panel_colors[] = {
         		0x0600,0x0333,0x0fb3,0x0d00,0x0b00,0x0720,0x0fc2,0x0c90,
			0x0a40,0x0eb0,0x0eca,0x0456,0x0577,0x0252,0x0444,0x0000
};

BYTE explosion_sequence[] = {
                        0,1,2,3,4,5,6,7,8,9,10,-1
};

BOOL screen_num=FALSE;
BOOL vcount=FALSE; /*Keeps track of screen paints to let us perform different tasks on alternating frames. */
BOOL mult1_on=FALSE; /*Set if power-ups attached to ship. */
BOOL mult2_on=FALSE; /*Additional power-ups. */
BOOL cannons_on=FALSE; /*What additional weapons are attached? */
BOOL lasers_on=FALSE;
UBYTE ship_status=0; /*All of these values let us know how to draw the ship sprites. */
UBYTE activemissiles=0;
BOOL firekey=FALSE;
BOOL ship_dead=FALSE;
BOOL kill_game=FALSE;

UWORD ship_speed; /*Ship's speed in pixels. */
WORD xpos; /*Ship's x position. */
WORD ypos; /*Ship's y position. */
UWORD mult_number; /*Ship's animation number. */
UWORD mult_delay=0; /*Animation delay. */
WORD xvector=0; /*Used for adding inertia to mouse input. */
WORD yvector=0;
UWORD oldmousex,oldmousey; /*We track the mouse by comparing these previous values with the current values. */
WORD up_down=0; /*Sprite data offset to reflect ship's current tilt. */

UWORD pf1count; /*Current background playfield position in words. */
UWORD pf2count; /*Current foreground playfield position in words. */
WORD pf1scroll; /*Number of additional pixels to scroll background. */
WORD pf2scroll; /*Number of additional pixels to scroll foreground. */
WORD pf1scroll2;
UWORD pf2offset=0;
UWORD screenend;
UWORD level_end;
UWORD level_number=0;
UWORD fire_delay=0;
WORD ship_energy;
UWORD field_count=0;

UWORD path_delay;
UWORD last_x=0;
UWORD last_y;
UWORD path_number=0;
UWORD all_coords=0;
BOOL x_equal=FALSE;
BOOL y_equal=FALSE;
UWORD upsidedown;
BOOL last_path=FALSE;
UWORD explosion_delay=0;

struct Missile {
   UWORD x_pos,y_pos;
   UWORD range;
};
struct Missile missiledata[3];

UWORD fgndpointer; /*Pointer to foreground map data position. */
PLANEPTR displayraster[6]; /*Starting pointers to bitplanes. */
PLANEPTR rasters[9]; /*Scrolling bitplane pointers. */
UWORD shipaddress; /*Pointer to current ship sprite data. */
UWORD explosion_ptr;

struct Alien {
   UWORD x_pos,y_pos;
   UBYTE sprite_num,anim_num;
};
struct Alien alien_buffer[12];

struct CoordPair{
   UBYTE x,y;
};

/*Map of background tiles. Each tile number is stored in 4 bits.*/
UBYTE backgroundtable[] = {
                	0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,
			0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,
			0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,
			0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,
			0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,
			0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,
			0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,
			0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,
			0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,
			0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,0x01,0x23,
			0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,0x45,0x67,
			0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB,0x89,0xAB
};

UWORD backgrounds[] = {
             	  0x1430,0x0C30,0x0C10,0x0413,0x8403,0x4442,0x2142,0x3120,
		  0x3110,0x2810,0x141C,0x1417,0x140B,0x0C71,0x0A00,0xDC08,
		  0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		  0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

		  0x0210,0x0230,0x1118,0x0118,0x8128,0x414C,0x314C,0x10CA,
		  0x1005,0x1013,0x5103,0x31C3,0x5023,0xD011,0x3018,0x6304,
		  0x0129,0x0908,0x0884,0x0884,0x4294,0x22A2,0x08A2,0x0825,
		  0x0842,0x0908,0x2880,0xC824,0x2890,0x2808,0x0904,0x1092,

		  0x0300,0x4220,0x0461,0x0451,0x0850,0x08C8,0x38C8,0x1848,
		  0x0848,0x0848,0x0848,0x08C8,0x0148,0x1050,0xE060,0x4041,
		  0x2084,0x2110,0x2210,0x0228,0x0429,0x1424,0x0424,0x0424,
		  0x8424,0x8424,0x8424,0x8424,0x80A4,0x8828,0x1011,0x2420,

		  0x1430,0x0C08,0x0800,0x0413,0x8083,0x4442,0x2242,0x2120,
		  0x2110,0x2810,0x040C,0x1487,0x104B,0x0C71,0x0A10,0xD908,
		  0x8A08,0x9000,0x042D,0x8008,0x4244,0x2221,0x1121,0x0091,
		  0x0888,0x048A,0x0A03,0x2240,0x8220,0x9208,0x8124,0x2004,

		  0x7028,0x6218,0x8218,0xC208,0xC210,0xC210,0x6420,0x2420,
		  0x4820,0x4820,0x8843,0x0882,0x108C,0x2118,0x2220,0x4421,
		  0x0914,0x1104,0x4104,0x2504,0x2508,0x2508,0x9250,0x1250,
		  0x2450,0x2410,0x4420,0x8441,0x0840,0x1080,0x1110,0xA210,

		  0xC60C,0xC60A,0x4609,0x2209,0xA201,0xA223,0x9011,0x901F,
		  0x900C,0x8C18,0x0620,0x060F,0x0213,0x0404,0x1405,0x1419,
		  0x2102,0x2105,0x2104,0x5104,0x5124,0x5110,0x4908,0x4900,
		  0x4902,0x4204,0x8912,0x0900,0x2508,0x0212,0x4A02,0xEA04,

		  0x0080,0x0100,0x0110,0x0918,0x0A14,0x0A33,0x8A31,0x0621,
		  0x0220,0x8204,0x8204,0x860D,0x860A,0x8A0C,0x0108,0x0101,
		  0x4940,0x0888,0x8888,0x8484,0x852A,0x8508,0x4508,0x8110,
		  0x4911,0x4902,0x490A,0x4102,0x4905,0x4502,0x8084,0x9088,

		  0x7028,0x4318,0x8298,0xC248,0x8250,0xC210,0x6120,0x20A0,
		  0x5820,0x4820,0x8813,0x188A,0x108C,0x2100,0x2220,0x4421,
		  0x0914,0x1004,0x4144,0x2504,0x4528,0x2528,0x9290,0x1010,
		  0x2010,0x2410,0x4428,0x8405,0x0842,0x1080,0x1100,0xA210,

		  0x8820,0x0440,0x0204,0x1202,0x8221,0x8220,0x8300,0x8410,
		  0xC080,0x0180,0x20C0,0x6040,0x7060,0x31A1,0x2820,0x2820,
		  0x4410,0x8A24,0x0122,0x8921,0x4910,0x4110,0x4090,0x4208,
		  0x2448,0x8048,0x1020,0x1220,0x0910,0x0850,0x9492,0x1412,

		  0xCE02,0x4A02,0x5402,0x0442,0x0844,0x9048,0xE148,0x4150,
		  0x23A0,0x31C4,0x3185,0x3842,0x2820,0x11A0,0x23A0,0x6322,
		  0x0101,0x2501,0x2A01,0x9221,0x9422,0x68A4,0x10A4,0xA0A8,
		  0x1050,0x0822,0x0842,0x04A1,0x1412,0x8850,0x5051,0x1091,

		  0x3100,0x11C0,0x10C1,0x10E1,0x4862,0x8850,0x9850,0x88F0,
		  0xE830,0x7031,0x0028,0x8248,0x4088,0x4018,0x2018,0x3018,
		  0x0881,0x0821,0x0820,0x0810,0x2411,0x5429,0x44A9,0x4408,
		  0x0408,0x0808,0x8114,0x4124,0x2144,0x2104,0x1224,0x0844,

		  0xC820,0x4440,0x2204,0x1002,0x8225,0x8224,0x8308,0x8418,
		  0xC980,0x1188,0x20C4,0x6040,0x6260,0x2121,0x2820,0x2820,
		  0x0410,0xAA24,0x1122,0x8921,0x4912,0x0110,0x4094,0x4380,
		  0x2448,0x8844,0x1020,0x1220,0x0110,0x0850,0x9492,0x1412
};

UWORD level_colors[] = {
			0x0332,0x0055,0x0543,0x0000,0x0000,0x0000,0x0000,0x0000,
			0x0000,0x0F55,0x0B05,0x0700,0x08A7,0x0182,0x0065,0x0055,
			0x0000,0x0FF6,0x0000,0x0FD0,0x0A00,0x0BDF,0x06AF,0x004F,
			0x0FFF,0x0CDD,0x0ABB,0x0798,0x0587,0x0465,0x0243,0x0E32
};

UWORD alien_colors[] = {
         		0x0332,0x0055,0x0543,0x0000,0x0DFF,0x06AC,0x036A,0x0038,
			0x0332,0x0055,0x0543,0x0000,0x0FE0,0x0DA0,0x0B62,0x0900,
			0x0332,0x0055,0x0543,0x0000,0x0FE0,0x0DA0,0x0B62,0x0900,
			0x0332,0x0055,0x0543,0x0000,0x0DE9,0x0FE0,0x0F90,0x0D32,
			0x0332,0x0055,0x0543,0x0000,0x0FE0,0x0DA0,0x0B62,0x0900,
			0x0332,0x0055,0x0543,0x0000,0x09E7,0x06A0,0x0D00,0x0460,
			0x0332,0x0055,0x0543,0x0DDF,0x099E,0x0569,0x0348,0x0235,
			0x0332,0x0055,0x0543,0x0FFC,0x0FF2,0x0DE9,0x0BC6,0x09A4,
			0x0332,0x0055,0x0543,0x09FF,0x0AF5,0x03DF,0x01BF,0x008F,
			0x0332,0x0055,0x0543,0x0F77,0x08A7,0x0385,0x0265,0x0722,
			0x0332,0x0055,0x0543,0x0F66,0x08A7,0x0283,0x0065,0x0700,
			0x0332,0x0055,0x0543,0x0F66,0x08A7,0x0283,0x0065,0x0700,
			0x0332,0x0055,0x0543,0x0F66,0x08A7,0x0283,0x0065,0x0700,
			0x0332,0x0055,0x0543,0x0B85,0x0DDA,0x0C00,0x0952,0x0700,
			0x0332,0x0055,0x0543,0x0D8C,0x0B5A,0x000E,0x0937,0x0705,
			0x0332,0x0055,0x0543,0x0E00,0x0A98,0x0B60,0x0832,0x0733,
			0x0332,0x0055,0x0543,0x0E00,0x0A98,0x0B60,0x0832,0x0733,
			0x0332,0x0055,0x0543,0x0E00,0x0A98,0x0B60,0x0832,0x0733,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0
};

UBYTE *map;
ULONG mapSize;

UBYTE *panel;
ULONG panelSize;

UBYTE *graphics;
ULONG graphicsSize;

UBYTE *ships;
ULONG shipsSize;

UBYTE *paths;
ULONG pathsSize;

UBYTE *aliens;
ULONG aliensSize;

/*This is the alien path data header. */
struct Path {
   UWORD next_path; /*Offset to start of the next path. */
   UWORD x_pos; /*Current X position. */
   UWORD y_pos; /*Current Y position. */
   UWORD kills_what; /*Does it kill others when it dies? Which ones? (0-11) */
   UWORD table_offset; /*Offset to beginning of actual path script. */
   UBYTE sprite_num; /*Software sprite number. */
   BYTE anim_num; /*Animation number. */
   UBYTE anim_delay; /*Delay between anim frames, in increments of 1/25 second. */
   UBYTE anim_delay2; /*Static, used for refreshing anim_delay. */
   UBYTE speed; /*Speed in pixels. */
   UBYTE pause_count; /*Dynamic pause counter. */
   UBYTE mode; /*Mode flags bitmap. (See below).*/
   UBYTE loop_offset; /*Current loop offset. */
   UBYTE loop_count; /*Dynamic loop counter. */
   UBYTE hits_num; /*Number of hits needed to kill. */
   UBYTE num_anims; /*Number of animation frames. */
   UBYTE seek_count; /*Dynamic seek count. */
   UBYTE table_size;
   
   /*After this header comes the actual script. It consists of pairs of bytes, the first representing the X value to move to next, the second the Y value. X can also have special codes:
   
   0x0: Path finished, terminate alien.
   0xe0: Pause. The Y byte holds the pause value, with 0xff representing forever.
   0xe1: Start a loop.
   0xe2: Toggle to go to or from offset movement mode.
   0xe3: Change speed. The Y byte holds the new speed.
   0xe4: Change sprite. The Y byte holds the new sprite and new frame count.
   0xe5: Start seek mode. The Y value holds the count, and the next 2 bytes are 0.
   0xe6: Reload the X and Y coordinates with new values immediately. The next X and Y pair hold the new coordinates.
   0xe7: Load new table offset. The old is stored in loop_offset.
   0xe8: Restore the table offset from loop_offset.
   0xe9: Fire a heat seeker. The Y value holds the count.
   0xea: Start new animation. The Y value holds the new anim_num.
   0xeb: Repeat the entire script.
   
   Mode bits:
   0: Offset mode
   1: Seek mode
   3: Animation type
   4: Animate up (0) or down (1)
   5: Seek on ship's X and Y coordinates (heat seeker)
   6: Seek on ship's X coordinate only
   7: Seek on ship's Y coordinate only
*/
};

ULONG alien_pointers[] = {
   explosion1,0,0,guardian_eye1,explosion1,tadpole,eye,bubble,jellyfish1,jellyfish2,
   bordertl,borderbl,bordertr,borderbr,mouth,slime,snakebody,snakehead,
   0,0,0,0,0,0
};

UBYTE path_buffer[2048];
UBYTE __chip buffer0[6144];
UBYTE __chip buffer1[6144];
UBYTE __chip fgnd_buffer[2048];

struct Background{
   UWORD size;
   UWORD scroll;
   PLANEPTR plane1;
   PLANEPTR plane2;
   PLANEPTR plane3;};

struct Background screen0bgnds[16];
struct Background screen1bgnds[16];
struct Background missiletable[3];

main()
{
   ULONG i;

   /* We will use the graphics library only to locate and restore the system copper list once we are through. */
   GraphicsBase=(struct GfxBase *)OpenLibrary("graphics.library",0);

   /*We will use the DOS library to load in data prior to taking over the system. */
   DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",0);

   map=readFile("map.dat",(int *)&mapSize,0);
   panel=readFile("panel.dat",(int *)&panelSize,MEMF_CHIP);
   graphics=readFile("foregrounds.dat",(int *)&graphicsSize,MEMF_CHIP);
   ships=readFile("ships.dat",(int *)&shipsSize,MEMF_CHIP);
   paths=readFile("paths.dat",(int *)&pathsSize,0);
   aliens=readFile("aliens.dat",(int *)&aliensSize,MEMF_CHIP);
   if (map==NULL||panel==NULL||graphics==NULL||ships==NULL||paths==NULL||aliens==NULL)
      CleanExit(FALSE);
      
   /*Allocate the chip memory we will use.*/
   MemBase=(UBYTE *)AllocMem(MemNeeded,MEMF_CHIP);
   if (MemBase==NULL)
      CleanExit(FALSE);

   /*Set up pointers to each of the 9 bitplanes we will need. (Remember that the background is double-buffered.) */
   i=(ULONG)MemBase;
   rasters[0]=(PLANEPTR)i;
   displayraster[0]=(PLANEPTR)i;
   i+=BytesPerBackPlane;
   rasters[1]=(PLANEPTR)i;
   displayraster[1]=(PLANEPTR)i;
   i+=BytesPerBackPlane;
   rasters[2]=(PLANEPTR)i;
   displayraster[2]=(PLANEPTR)i;
   i+=BytesPerBackPlane;
   rasters[3]=(PLANEPTR)i;
   i+=BytesPerForePlane;
   rasters[4]=(PLANEPTR)i;
   i+=BytesPerForePlane;
   rasters[5]=(PLANEPTR)i;
   i+=BytesPerForePlane;
   rasters[6]=(PLANEPTR)i;
   displayraster[3]=(PLANEPTR)i;
   i+=BytesPerBackPlane;
   rasters[7]=(PLANEPTR)i;
   displayraster[4]=(PLANEPTR)i;
   i+=BytesPerBackPlane;
   rasters[8]=(PLANEPTR)i;
   displayraster[5]=(PLANEPTR)i;

   /*Set up a pointer to the hardware registers that will let us control the machine on the most fundamental level. */
   Hardware=(struct Custom *)0xdff000;

   TakeSystem();

   Hardware->bltafwm=0xffff;
   Hardware->bltalwm=0xffff;
   
   GameInit();

   Hardware->cop1lc=(ULONG)copperlist;
   
   /*Reenable DMA. */
   Hardware->dmacon=0x87e0;

   /*Clear any interrupts that were pending. */
   Hardware->intreq=0x7fff;
   
   Hardware->clxcon=0xc30;

   /*Here begins the main game loop. */
   do{
      /*Wait for the display to reach line 223, the start of the status panel and end of the action area. */
      waitline223();
      
      /*Perform different tasks on alternating frames. */
      vcount=!vcount;
      if (vcount){
         checkpf(FALSE);
         moveship();
         check_collision();
         erase_missiles();
         update_missiles();
         drawfgnds();
         check_path();}
         
      else{
         checkpf(TRUE);

         /*These routines execute every 2nd frame. */
         flipbgnd();
         moveship();

         /*Restore backgrounds where we drew aliens last time around. */
         restorebgnds();

         process_aliens();

         /*Save the backgrounds where we will draw aliens. */
         save_aliens();

         /*And draw them in. */
         draw_aliens();};
   }

   while(!kill_game&&(fgndpointer<mapSize));

   CleanExit(TRUE);
}

void waitline223()
{
   /*We have programmed the copper to set an interrupt flag when it reaches line 223. This function (busy)waits for that to happen. */
   while (!(Hardware->intreqr&16));
   Hardware->intreq=16;
}

void checkpf(BOOL pf1)
{
   /*If the end boss is on, we don't scroll. */
   if (level_end==3)
      return;
      
   if (pf1){

      /*Scroll 1 pixel to the left. */
      pf1scroll--;
      
      /*Reposition bitplane pointers and reset scroll to 15 if we have scrolled 16 pixels. */
      if (pf1scroll<0)
         resetpf1();};
   
   /*Scroll 1 pixel to the left. */
   pf2scroll--;

   /*If we have scrolled 16 pixels, reset the scroll values and bitplane pointers. */
   if (pf2scroll<0)
      resetpf2();
      
   /*Set a flag if we've reached the end of the 2 screens. */
   screenend=((pf1count-1)|pf1scroll);

   /*Plug the new scroll values into the copper list. */
   copperlist[29]=((pf2scroll<<4)|pf1scroll);
}

void resetpf1()
{
   /*Reset scroll back to 15. */
   pf1scroll=15;

   /*Decrement start position. */
   pf1count--;
   
   /*If scroll is 0, reset bitplane pointers to their starting position. */
   if (pf1count==0){
      pf1count=23;
      
      /* Double-buffered, so 6 pointers to reset. */
      displayraster[0]=rasters[0];
      displayraster[1]=rasters[1];
      displayraster[2]=rasters[2];
      displayraster[3]=rasters[6];
      displayraster[4]=rasters[7];
      displayraster[5]=rasters[8];
      
      /*No more to do here. */
      return;};
      
   /*Increment background bitmap pointers by 2 bytes. */
   displayraster[0]+=2;
   displayraster[1]+=2;
   displayraster[2]+=2;
   displayraster[3]+=2;
   displayraster[4]+=2;
   displayraster[5]+=2;
}

void resetpf2()
{
   /*Reset scroll back to 15. */
   pf2scroll=15;

   /*Decrement start position. */
   pf2count--;

   /*If count is 0, reset. */
   if (pf2count==0){
      pf2count=23;

      /*Offset is the complement of pf2count, which we will plug into the copper list. So we reset that to 0. */
      pf2offset=0;

      /*Check if we have reached the end of the level and should begin to bring out the boss. (More to come here.) */
      if (level_end==1)
         level_end++;}

   /*Increment the offset. */
   pf2offset+=2;

   /*And store the new bitplane pointers in the copper list. */
   copperlist[15]=HIWORD(rasters[3]+pf2offset);
   copperlist[17]=LOWORD(rasters[3]+pf2offset);
   copperlist[19]=HIWORD(rasters[4]+pf2offset);
   copperlist[21]=LOWORD(rasters[4]+pf2offset);
   copperlist[23]=HIWORD(rasters[5]+pf2offset);
   copperlist[25]=LOWORD(rasters[5]+pf2offset);
}

void flipbgnd()
{
   /*The double-buffering routine. Swaps bitplane pointers every 2nd frame. */
   PLANEPTR temp0,temp1,temp2;
   
   temp0=displayraster[0];
   temp1=displayraster[1];
   temp2=displayraster[2];
   displayraster[0]=displayraster[3];
   displayraster[1]=displayraster[4];
   displayraster[2]=displayraster[5];
   displayraster[3]=temp0;
   displayraster[4]=temp1;
   displayraster[5]=temp2;

   /*Add 4 bytes to pointers for a clipping area. */
   temp0+=4;
   temp1+=4;
   temp2+=4;
   
   /*And plug them into the copper list. */
   copperlist[3]=HIWORD(temp0);
   copperlist[5]=LOWORD(temp0);
   copperlist[7]=HIWORD(temp1);
   copperlist[9]=LOWORD(temp1);
   copperlist[11]=HIWORD(temp2);
   copperlist[13]=LOWORD(temp2);
   
   screen_num=!screen_num;
}

void drawfgnds()
{
   /*If we are at the end boss, nothing to do here. */
   if (level_end>1)
      return;
      
   /*We draw a new column of foreground graphics into the hidden parts of the screen every 16 pixels, using the blitter.*/

   if (pf2scroll==0){

      /*Offset will always be 0 at this stage, letting us draw the column just outside of the visible screen. */
      drawfgnd(0);

      /*Increment map position for drawing the next column next time around. */
      fgndpointer+=12;}

   else

       if (pf2scroll==14)

          if (map[fgndpointer]!=255)

             /*Using an offset of 46 bytes (the width of the screen) lets us draw the next column on just to the other side of the visible screen. */
             drawfgnd(46);

          else

             /*A block number of 255 means we have reached the end of the level and should bring out the boss. */
             level_end=1;
}

void drawfgnd(int offset)
{
   PLANEPTR plane1,plane2,plane3;
   int i; 

   setupblit();

   /*Calculate start positions with offsets. */
   plane1=rasters[3]+offset+pf2offset;
   plane2=rasters[4]+offset+pf2offset;
   plane3=rasters[5]+offset+pf2offset;

   for (i=0;i<12;i++){

       /*Blit the next block in. Each block consumes 96 bytes.*/
       blitfgnd(plane1,plane2,plane3,(PLANEPTR)&graphics[map[fgndpointer+i]*96]);

       /*And increment pointers to the next starting position 16 lines down. */
       plane1+=(ForegroundWidth*16);
       plane2+=(ForegroundWidth*16);
       plane3+=(ForegroundWidth*16);};
}

void blitfgnd(PLANEPTR plane1,PLANEPTR plane2,PLANEPTR plane3,PLANEPTR src)
{
   /*Blit a 16X16 block onto the screen. */
   
   Hardware->bltdpt=(APTR)plane1;
   Hardware->bltapt=(APTR)src;
   Hardware->bltsize=0x401;
   Hardware->bltdpt=(APTR)plane2;
   Hardware->bltapt=(APTR)(src+32);
   Hardware->bltsize=0x401;
   Hardware->bltdpt=(APTR)plane3;
   Hardware->bltapt=(APTR)(src+64);
   Hardware->bltsize=0x401;
}

void setupblit()
{
   /*Straight copy from channel A to channel D, with no transformations. See the Hardware Manual for more info.*/
   Hardware->bltcon0=0x9f0;
   Hardware->bltcon1=0x0;
   
   /*No mod for source, as data is stored sequentially. Destination mod is set up to draw into a rectangle carved out of the destination bitmaps.*/
   Hardware->bltamod=0x0;
   Hardware->bltdmod=(ForegroundWidth-2);
}

void buildbackgnd()
{
   UWORD *planeA1,*planeA2,*planeB1,*planeB2;
   UBYTE *table;
   int x,y;

   /*Set up pointers to 4 background bitplanes -- 2 for each screen, referred to here as screens A and B. (Remember, we are double-buffering.) The 3rd bitplane for each screen we will ignore for now, as it will not be used for the background pattern. We skip forward 4 bytes from the start of each bitplane to account for the clipping area at the edge which does not concern us now.*/
   planeA1=(UWORD *)rasters[0]+2;
   planeA2=(UWORD *)rasters[1]+2;
   planeB1=(UWORD *)rasters[6]+2;
   planeB2=(UWORD *)rasters[7]+2;

   /*Find our starting point for the level map. The complete game obviously contains more maps than just the 1 that is recreated here. Each uses 144 bytes.*/
   table=backgroundtable+(level_number*144);

   for (y=0;y<12;y++){ /*12 blocks per column. */

      for (x=0;x<12;x++){ /*23 blocks per row. We draw 2 per inner loop, except the last time through. */
          /*2 block numbers per byte, in the upper and lower 4 bits. */
          drawback((UBYTE)(table[(y*12)+x]>>4),planeA1+(2*x),planeA2+(2*x),planeB1+(2*x),planeB2+(2*x));
          if (x<11)
             drawback((UBYTE)(table[(y*12)+x+1]&15),planeA1+(2*x)+1,planeA2+(2*x)+1,planeB1+(2*x)+1,planeB2+(2*x)+1);};
      /*Increment pointers to point to 16 pixels down before drawing the next row of blocks. */
      planeA1+=BackgroundWidth*8;
      planeA2+=BackgroundWidth*8;
      planeB1+=BackgroundWidth*8;
      planeB2+=BackgroundWidth*8;};
}

void drawback(UBYTE block,UWORD *planeA1,UWORD *planeA2,UWORD *planeB1,UWORD *planeB2)
{
   UWORD *data;
   int blockLoc;
   int i;

   /*The background data consumes 1024 bytes per level. */
   data=backgrounds+(level_number*512);

   /*Each block consumes 64 bytes (32 words). */
   blockLoc=block*32;

   /*Each block is copied twice into each of the two background screens. */
   for (i=0;i<16;i++){
      planeA1[0]=data[blockLoc+i];
      planeB1[0]=data[blockLoc+i];
      planeA1[23]=data[blockLoc+i];
      planeB1[23]=data[blockLoc+i];
      planeA2[0]=data[blockLoc+16+i];
      planeB2[0]=data[blockLoc+16+i];
      planeA2[23]=data[blockLoc+16+i];
      planeB2[23]=data[blockLoc+16+i];
      planeA1+=(BackgroundWidth/2);
      planeA2+=(BackgroundWidth/2);
      planeB1+=(BackgroundWidth/2);
      planeB2+=(BackgroundWidth/2);};
}

void GameInit()
{
   int i;

   /*Set up map data pointer. */
   fgndpointer=0;

   /*Background is 23 words wide. */
   pf1count=23;

   /*1 extra here to make the numbers line up the first time through the foreground scroll. After that we will reset to 23. */
   pf2count=24;

   /*Initial scroll is 15. */
   pf1scroll=15;
   pf2scroll=15;
   
   /*Set up ship's initial position, speed, and animation. */
   xpos=100;
   ypos=80;
   ship_speed=2;
   mult_number=1;
   
   /*Set up pointer to ship's initial sprite data. */
   shipaddress=ship1_2;
   
   path_delay=10;

   ship_energy=128;
   max_energy(324-12);
   explosion_ptr=0;

   /*Copy bitplane addresses into copper list. */
   copperlist[3]=HIWORD(rasters[0]);
   copperlist[5]=LOWORD(rasters[0]);
   copperlist[7]=HIWORD(rasters[1]);
   copperlist[9]=LOWORD(rasters[1]);
   copperlist[11]=HIWORD(rasters[2]);
   copperlist[13]=LOWORD(rasters[2]);
   copperlist[15]=HIWORD(rasters[3]);
   copperlist[17]=LOWORD(rasters[3]);
   copperlist[19]=HIWORD(rasters[4]);
   copperlist[21]=LOWORD(rasters[4]);
   copperlist[23]=HIWORD(rasters[5]);
   copperlist[25]=LOWORD(rasters[5]);

   /*Skip forward 2 words in the background bitplanes. */
   copperlist[5]=copperlist[5]+4;
   copperlist[9]=copperlist[9]+4;
   copperlist[13]=copperlist[13]+4;
   copperlist[29]=0xff;

   /*Load the 4 status panel bitplane addresses into the copper list. The status panel is 352X32, consuming 1408 bytes per bitplane.*/
   for (i=0;i<4;i++){
      copperlist[149+(4*i)]=HIWORD(panel+32+(i*1408));
      copperlist[151+(4*i)]=LOWORD(panel+32+(i*1408));};

   /*Load the level colors into the copper list. */
   for (i=0;i<32;i++)
      copperlist[45+(2*i)]=level_colors[i];

   /*Load the status panel colors into the copper list. */
   for (i=0;i<16;i++)
      copperlist[165+(2*i)]=panel_colors[i];
      
   /*Set up sprites. */
   ship_to_copper();

   clear_screen();

   /*Draw in the background pattern. */
   buildbackgnd();

   setup_mouse();
   
   for (i=0;i<3;i++)
       missiletable[i].size=0;

   #ifdef FullFirepower
   mult1_on=TRUE;
   mult2_on=TRUE;
   cannons_on=TRUE;
   lasers_on=TRUE;
   ship_speed=2;
   ship_status=3;
   change_ship();
   #endif
}

/*Clear all of screen memory. */
void clear_screen()
{
   ULONG *i;

   for (i=(ULONG *)rasters[0];i<(ULONG *)(rasters[0]+MemNeeded);i++)
       *i=0;
}

void check_collision()
{
   UWORD clx;
   UWORD x,y;
   ULONG exp1,exp2,exp3;
   ULONG *sprite;
   int i;

   /*Nothing to do here if the player's already dead. */
   if (ship_dead)
      return;

   /*The clxdat register is cleared when we read it, so we preserve its value in a variable. */
   clx=Hardware->clxdat;
   
   /*Nothing to do here if no collision has occured, or if the player has a force field. */
   if (!(clx&0x46)||field_count)
      return;
      
   /*If bit 1 and/or 2 of clxdat are set, the ship has collided with an alien. */
   if (clx&6){

   ship_energy--;

   /*If shields are completely drained, start to explode the player's ship. */
   if (ship_energy<0){

      /*This flag signals the moveship routine that the ship is exploding. */
      ship_dead=TRUE;

      /*We need to caculate the sprite control longwords for the explosion that will replace the usual ship sprites. */
      x=xpos;
      y=ypos;

      /*Vertical adjustment to convert screen coordinates into hardware coordinates. */
      y+=44;

      /*Move the y value into the upper 8 bits of y. Our y values will never exceed 255, so no need to worry about the high bits of y. */
      y=(y<<8);

      /*Horizontal adjustment. */
      x+=128;

      /*Get x and y into the upper 16 bits of the control longword, discarding bit 0 of x and shifting the other bits over. */
      exp1=((y|(x>>1))<<16);

      /*Bit 0 of x goes into bit 0 of the longword. */
      exp1+=(x&1);

      /* Calculate vertical ending position for a 24 pixel high sprite. (Sprites are always 16 pixels wide, so no need for a horizontal ending position.) */
      y+=0x1800;

      /*And plug it into bits 8-15 of the control longword.*/
      exp1=(exp1|y);

      /*We need 3 sprites for the explosion, 16 horizontal pixels apart. */
      exp2=exp1+0x00080000;
      exp3=exp1+0x00100000;

      /*Store the sprite control words into all 10 frames of the animation. */
      sprite=(ULONG *)&ships[ship_explosion];
      for (i=0;i<10;i++){
         sprite[0]=exp1;
         sprite[26]=exp2;
         sprite[52]=exp3;
         sprite+=78;};

      /*Change the copper list to reflect the colors used for the explosion. */
      copperlist[77]=0xfd0;
      copperlist[85]=0xfd0;
      copperlist[79]=0xf80;
      copperlist[87]=0xf80;
      copperlist[81]=0xc21;
      copperlist[89]=0xc21;

      explode_ship();}

   else{

      /*Reset ship_count. */
      decrease_energy(324-12,ship_energy);};};
}

void explode_ship()
{
   ULONG *sprite;
   int i;

   explosion_delay++;
   if (explosion_delay%4)
      return;
      
   /*The frames sequence of the explosion animation is stored in explosion_sequence. -1 indicates the end of the animation. */
   if (explosion_sequence[explosion_ptr]<0){
      kill_game=TRUE;
      return;};
      
   /*Work out the sprite addresses of the next frame. Each frame consists of 3 non-attached sprites. */
   sprite=(ULONG *)&ships[ship_explosion+(explosion_sequence[explosion_ptr]*312)];
   explosion_ptr++;
   
   /*Put the sprites into the copper list. */
   for (i=0;i<3;i++){
      copperlist[109+(4*i)]=HIWORD((ULONG)sprite);
      copperlist[111+(4*i)]=LOWORD((ULONG)sprite);
      sprite+=26;};
      
   /*Make sure the other sprites used for the ship are no longer visible. */
   copperlist[121]=0;
   copperlist[123]=0;
}

void max_energy(UWORD offset)
{
   UWORD *loc;
   int i;

   /*Offset determines which of the 3 energy bars to fill. */
   loc=(UWORD *)(panel+offset);

   /*Turn on bits to fill the bar with color. */
   for (i=0;i<8;i++){
      loc[0]=(loc[0]|0x5555);
      loc[22]=(loc[22]|0x5555);
      loc[44]=(loc[44]|0x5555);
      loc++;};
}

void moveship()
{
   UWORD up,down,right,left;
   ULONG ctl;
   ULONG *addr;
   WORD madjust=0;
   
   if (ship_dead){
      explode_ship();
      return;};
      
   if (field_count){
      
      /*Decrement the force field counter. */
      field_count--;
      
      /*If force field is expired, reset the ship outline to black. Otherwise strobe dark and light blue. */
      if (field_count==0)
         copperlist[77]=0;
      if (field_count&1)
         copperlist[77]=0xc;
      else
         copperlist[77]=0xf;};

   /*Read joystick and mouse. */
   joy(&up,&down,&right,&left);
   
   /*Upper movement limit is 0. */
   if (ypos<0){
      
      /*No further upward movement allowed. */
      up=0;
      yvector=0;}

   else{
      /*Lower movement limit is 150. */
      if (ypos>150){

         /*No further downward movement allowed. */
         down=0;
         yvector=0;};};

   /*Leftward movement limit is 0. */
   if (xpos<0){
      left=0;
      xvector=0;}
      
   else{
      /*Rightward movement limit is 266. */
      if (xpos>266){
         right=0;
         xvector=0;};};

   /*Untilt ship. */
   up_down=0;

   /*Are we going up? */
   if (up){

      /*Sprite data is 560 bytes long. We will adjust the pointer using up_down to point to the correct image. */
      up_down=-560;

      /*Are we already moving at maximum speed? */
      if (-(ship_speed)!=yvector)

         /*If not, adjust yvector. */
         yvector--;}

   else{
      /*Are we going down? */
      if (down){

         /*Tilt the ship down. */
         up_down=560;

         /*Are we at maximum speed? */
         if (ship_speed!=yvector)

            /*Adjust yvector if not. */
            yvector++;};};

   /*Now do the same drill for horizontal movement, minus the tilting. */
   if (right){
      madjust=1;
      if (ship_speed!=xvector)
         xvector++;}
   else{
      madjust=-1;
      if (left)
         if (-(ship_speed)!=xvector)
            xvector--;};
            
   /*If we have not moved left or right, we may need to change the outrider. */
   if (right==0&&left==0){
      mult_delay=4;

      /*If ship is not being actively moved but still has intertia, decrease X and Y vectors. */
      if (xvector>0)
         xvector--;
      else
          if (xvector<0)
             xvector++;};

   if (up==0&&down==0)
      if (yvector>0)
         yvector--;
      else
          if (yvector<0)
             yvector++;

   /*Calculate new X and Y coordinates of the ship. */
   xpos+=xvector;
   ypos+=yvector;

   /*Convert the X and Y coordinates into a single sprite control longword. */
   ctl=xytosprite(xpos,ypos);

   /*Set up a pointer to the correct ship sprite data, accounting for tilt. */
   addr=(ULONG*)&ships[shipaddress+up_down];
   
   /*First sprite, rear end. */
   *addr=ctl;

   /*Second sprite, rear end, with attach bit set. */
   addr+=46;
   *addr=(ctl|0x80);

   /*Add 11 to horizontal and 16 to vertical starting positions of the second sprite pair. */
   ctl+=0x0b080000;

   /*Subtract 11 from vertical ending position of the second sprite pair. */
   ctl-=0x0b00;

   /*Third sprite, front end. */
   addr+=46;
   *addr=ctl;
   
   /*Fourth sprite, front end, with attach bit set. */
   addr+=24;
   *addr=(ctl|0x80);
   
   /*Put the ship into the copper list. */
   ship_to_copper();
   
   /*Which outrider to display? */
   mult_delay--;
   if (mult_delay==0){
      mult_delay=4;
      mult_number+=madjust;

      /*Outrider number must be between 1 and 5. */
      if (mult_number==0)
         mult_number=1;
      else
          if (mult_number==6)
             mult_number=5;};

   /*Draw the outrider into the sprite data. */
   draw_outrider();
   
   check_firekey();
}

ULONG xytosprite(UWORD x,UWORD y)
{
   /*This function returns a register sprite control longword based on the desired X and Y starting positions. */
   ULONG result;
   
   /*Vertical adjustment to convert screen coordinates into hardware coordinates. */
   y+=44-11;
   
   /*Move the y value into the upper 8 bits of y. Our y values will never exceed 255, so no need to worry about the high bits of y. */
   y=(y<<8);

   /*Horizontal adjustment. */
   x+=128;
   
   /*Get x and y into the upper 16 bits of the control longword, discarding bit 0 of x and shifting the other bits over. */
   result=((y|(x>>1))<<16);

   /*Bit 0 of x goes into bit 0 of the longword. */
   result+=(x&1);

   /* Calculate vertical ending position for a 44 pixel high sprite. (Sprites are always 16 pixels wide, so no need for a horizontal ending position.) */
   y+=0x2c00;

   /*And plug it into bits 8-15 of the control longword.*/
   result=(result|y);

   return (result);
}

void draw_outrider()
{
   int i;
   UWORD adjust;
   UWORD *plane1,*plane2,*dest;

   if (mult1_on)
      adjust=mult_number*44;
   else
      adjust=0;

   /*Outriders stored as conventional bitplanes. We find the beginning address here. */
   plane1=(UWORD *)(&ships[outriders]+adjust);
   plane2=plane1+132;
   
   /*Address to copy outriders into sprite data. */
   dest=(UWORD *)(&ships[shipaddress]+up_down+4);

   /*Copy 11 lines of data, interpolating the bitplane data to conform with the Amiga's sprite format. */
   for (i=0;i<11;i++){
      *dest=*plane1;
      dest++;
      plane1++;
      *dest=*plane2;
      dest++;
      plane2++;};

   /*If outrider 2 not on, point both bitplane sources to 0's. */
   if (!mult2_on){
      plane1=(UWORD *)&ships[outriders];
      plane2=plane1;};

   /*Jump past the central ship graphic to add the outrider below. */
   dest+=(22*2);

   for (i=0;i<11;i++){
      *dest=*plane1;
      dest++;
      plane1++;
      *dest=*plane2;
      dest++;
      plane2++;};

   /*Now we start on the next sprite pair, painting in bitplanes 3 and 4 of the outrider image data. (Plane 1 is now really plane3, plane 2 plane 4.) */
   plane1=(UWORD *)(&ships[outriders]+adjust+528);
   plane2=plane1+132;
   
   /*Jump to the next sprite. */
   dest=(UWORD *)(&ships[shipaddress]+up_down+4+44*4+8);

   /*And paint the outrider in, as before. */
   for (i=0;i<11;i++){
      *dest=*plane1;
      dest++;
      plane1++;
      *dest=*plane2;
      dest++;
      plane2++;};

   /*If outrider 2 not on, we need to readjust the source pointers to point to all 0's. */
   if (!mult2_on){
      plane1=(UWORD *)&ships[outriders];
      plane2=plane1;};

   /*Jump past the central area of the sprite that holds the main ship. */
   dest+=(22*2);

   for (i=0;i<11;i++){
      *dest=*plane1;
      dest++;
      plane1++;
      *dest=*plane2;
      dest++;
      plane2++;};
      
/*And now all 4 outrider bitplanes have been painted in, divided between the 2 attached sprites that make up the back part of the ship. */
}

void check_firekey()
{
   int i;

   if (!get_firekey()){
      firekey=FALSE;
      return;};

   if (fire_delay){
      fire_delay=firedelay;
      return;};
      
   fire_delay=firedelay;
   
   if (firekey)
      return;

   firekey=TRUE;
   
   for (i=0;i<3;i++)
      if (!(activemissiles&(1<<i))){
         activemissiles=(activemissiles|(1<<i));
         missiledata[i].x_pos=xpos+58;
         missiledata[i].y_pos=ypos+24;
         missiledata[i].range=16;
         break;};
}

BOOL get_firekey()
{
   if (((*PortA&128)==0)||((*PortA&64)==0))
      return (TRUE);
   else
      return (FALSE);
}

void decrease_energy(UWORD offset,UWORD energy)
{
   /*Offset 324 for shields; 676 for cannons; 1028 for lasers. */
   UWORD *loc;

   /*Find the part of the panel we are modifying. */
   loc=(UWORD *)(panel+offset+(energy/8));

   /*Remove yellow color from that section. */
   loc[0]=((loc[0]&0xaaaa)|((loc[0]&0x5555)<<2));
   loc[22]=((loc[22]&0xaaaa)|((loc[0]&0x5555)<<2));
   loc[44]=((loc[44]&0xaaaa)|((loc[0]&0x5555)<<2));
}

void setup_mouse()
{
   /*Read the current mouse X and Y values. */
   oldmousex=(Hardware->joy0dat&0xff);
   oldmousey=((Hardware->joy0dat&0xff00)>>8);
}

void joy(UWORD *up,UWORD *down,UWORD* right,UWORD *left)
{
   /*Read the current position of the joystick. The joystick registers work in a rather counterintuitive way that requires some shifting and exclusive oring. As usual, see the Hardware Manual for the nitty gritty. */
   *up=(Hardware->joy1dat&0x100);
   *down=(Hardware->joy1dat&0x1);
   *right=(Hardware->joy1dat&0x2);
   *left=(Hardware->joy1dat&0x200);
   *down=((*down<<1)^*right);
   *up=((*up<<1)^*left);

   /*Read the mouse, and if it has been moved since last time set up our variables to mimic a joystick movement. We don't carry about proportional values; we just want to know if it has been moved, and if so in what direction. */
   if (((Hardware->joy0dat&0xff)-oldmousex)>0)
      *right=1;
   else
       if (((Hardware->joy0dat&0xff)-oldmousex)<0)
          *left=1;
   if (((Hardware->joy0dat>>8)-oldmousey)>0)
      *down=1;
   else
       if (((Hardware->joy0dat>>8)-oldmousey)<0)
          *up=1;

   /*Save the current mouse values for next time around. */
   oldmousex=(Hardware->joy0dat&0xff);
   oldmousey=(Hardware->joy0dat>>8);
}

void ship_to_copper()
{
   /*Update the copper list to reflect the current ship sprite. */
   copperlist[109]=HIWORD(&ships[shipaddress]+up_down);
   copperlist[111]=LOWORD(&ships[shipaddress]+up_down);
   copperlist[113]=HIWORD(&ships[shipaddress]+up_down+184);
   copperlist[115]=LOWORD(&ships[shipaddress]+up_down+184);
   copperlist[117]=HIWORD(&ships[shipaddress]+up_down+(184*2));
   copperlist[119]=LOWORD(&ships[shipaddress]+up_down+(184*2));
   copperlist[121]=HIWORD(&ships[shipaddress]+up_down+(184*2)+96);
   copperlist[123]=LOWORD(&ships[shipaddress]+up_down+(184*2)+96);
}

void change_ship()
{
   /*Status 0 == no additional weapons (ship 1). Status 1 == cannons (ship 2). Status 2 == lasers (ship 3). Status 3 == cannons and lasers (ship 4). We find the address of the proper ship and store it in shipaddress. */
   shipaddress=ship1_2+(1680*ship_status);
}

void restorebgnds()
{
   struct Background *screenbgnds;
   UBYTE *buffer;

   screenbgnds=getscreeninfo((ULONG *)&buffer);

   /*Point blitter channel A to the saved background data. */
   Hardware->bltamod=0;
   Hardware->bltapt=(APTR)buffer;
   Hardware->bltcon1=0;

   /*A size of 0 indicates that we have restored all of the backgrounds. */
   while (screenbgnds->size){

      /*Word out correct destination modulo from blit size. */
      Hardware->bltdmod=100-((screenbgnds->size&0x3f)*2);

      /*Simple copy from channel A to channel D. */
      Hardware->bltcon0=0x9f0;

      /*Blit all 3 bitplanes. */
      Hardware->bltdpt=(APTR)screenbgnds->plane1;
      Hardware->bltsize=screenbgnds->size;
      Hardware->bltdpt=(APTR)screenbgnds->plane2;
      Hardware->bltsize=screenbgnds->size;
      Hardware->bltdpt=(APTR)screenbgnds->plane3;
      Hardware->bltsize=screenbgnds->size;
      screenbgnds++;};
}

void savebgnds(UWORD x,UWORD y,UBYTE * buffer,struct Background *screenbgnds)
{
   PLANEPTR *planes;
   UWORD boffset,soffset;

   /*Deal with screen wrap-around if necessary. */
   planes=check_end_screen();

   /*Convert an X and Y locations into memory addresses the blitter can deal with. */
   coords_to_pf1offsets(x,y,&boffset,&soffset);

   Hardware->bltamod=94;
   Hardware->bltdmod=0;
   Hardware->bltcon1=0;
   
   /*Simple copy, channel A to channel D. */
   Hardware->bltcon0=0x9f0;
   
   /*Set up the data structure for the blit_to_buffer function. */
   setup_addresses(screenbgnds,aliensize,soffset,boffset,planes);

   /*And blit the background area into a buffer for safekeeping. */
   blit_to_buffer(buffer,screenbgnds);
}

void drawbobs(struct Background *screenbgnd,UBYTE *alien,UBYTE *mask)
{

   /*Set up playfield source and destination modulos. If the alien is to be drawn upside down, we need to work in reverse -- thus the negative modulo. */
   if (upsidedown){
      Hardware->bltdmod=(unsigned short)-106;
      Hardware->bltcmod=(unsigned short)-106;}
   else{
      Hardware->bltdmod=(unsigned short)94;
      Hardware->bltcmod=(unsigned short)94;};

   /*Channel B (the actual alien data) scroll value. */
   Hardware->bltcon1=(screenbgnd->scroll<<12);
   
   /*Channel A (the mask) scroll value. We also set up bltcon0 to merge channels A, B, and C during the copy. */
   Hardware->bltcon0=((screenbgnd->scroll<<12)|(0xfca));

   Hardware->bltbmod=(unsigned short)-2;
   Hardware->bltamod=(unsigned short)-2;
   Hardware->bltbpt=(APTR)alien;
   
   /*This performs the actual blit. */
   blit_to_backgnd(screenbgnd,mask);
}

struct Background *getscreeninfo(ULONG *buffer)
{
   /*Return the address of the current screen and background buffers. (Because we are double-buffering, we have 2 of each.) */
   if (screen_num){
      *buffer=(ULONG)buffer1;
      return (screen1bgnds);};

   *buffer=(LONG)buffer0;
   return (screen0bgnds);
}

void process_aliens()
{
   /*We will store the most essential information into alien_buffer for passing to the graphics routines. */
   struct Path *path;
   struct CoordPair *script;
   int i;

   #define FINISHED path=path_finished(path,&alien_buffer[i]); continue
   #define PATH_FINISHED path->x_pos=0; FINISHED

   all_coords=0;
   path=(struct Path *)path_buffer;

   /*Loop through the 12 possible active aliens. */
   for (i=0;i<12;i++)
   {
      /*Get current script location. */
      script=(struct CoordPair *)((UBYTE *)path+path->table_offset);

      /*If x_pos is not 0, this alien is alive. */
      if (path->x_pos){
         last_x=0;
         last_y=0;
         path->anim_delay=(path->anim_delay-1);

         /*If anim_delay is 0, load the next animation frame. */
         if (path->anim_delay==0&&path->num_anims>0){

            /*If mode bit 3 is set, the animation reverses direction at the limit to cycle backward and forward. */
            if (path->mode&8){
               if (path->mode&16){
                  path->anim_num=(path->anim_num-1);
                  if (path->anim_num<0){
                     path->anim_num=1;
                     path->mode=(path->mode-16);};}
               else{
                  path->anim_num=(path->anim_num+1);
                  if (path->anim_num==path->num_anims){
                     path->mode=(path->mode+16);};};}
            else{
               path->anim_num=(path->anim_num+1);
               if (path->anim_num>path->num_anims){
                  path->anim_num=0;

                  /*Sprite_num of 0 indicates an explosion. */
                  if (path->sprite_num==0){
                     path->x_pos=0;

                     /*Store coordinates of the explosion. */
                     last_x=path->x_pos;
                     last_y=path->y_pos;

                     /*Nothing more to do here, as the alien is dead. */
                     FINISHED;};};};

            /*Reset the animation delay counter. */
            path->anim_delay=path->anim_delay2;};

         /*If we have an explosion, nothing to move. */
         if (path->sprite_num==0){
            FINISHED;};

         /*Are we pausing? */
         if (path->pause_count){

            /*Pause_count of 255 means infinite pause. */
            if (path->pause_count!=255)
               path->pause_count=(path->pause_count-1);

            /*If pause_count has reached 0, update the script. */
            if (path->pause_count==0)
               update_script(path,script);

            FINISHED;}

         else{

            /*Check seek bits and update the next X and Y pair if necessary to reflect the current location of the player's ship. */
            check_seek(path,script);

            /*Make sure we don't have an explosion. */
            if (path->sprite_num){

               /*Are we in offset mode? */
               if (path->mode&1){

                  /*Add the offsets to current coordinates. We typecast them to BYTEs to allow negative offsets. */
                  path->x_pos=(path->x_pos+(BYTE)script->x);
                  path->y_pos=(path->y_pos+(BYTE)script->y);

                  /*Signal to advance the script. */
                  x_equal=TRUE;
                  y_equal=TRUE;}

               else{

                  /*Is there anything to do on the X coordinate? */
                  if (path->x_pos!=(script->x*2)){
                     x_equal=FALSE;

                     /*If current X is greater than desired X, decrease current X. */
                     if (path->x_pos>(script->x*2)){
                        path->x_pos=(path->x_pos-path->speed);

                        /*Make sure we haven't gone past our goal. */
                        if (path->x_pos<(script->x*2))
                           path->x_pos=(script->x*2);}

                     else{
                        path->x_pos=(path->x_pos+path->speed);

                        /*Make sure we haven't gone past the goal. */
                        if (path->x_pos>(script->x*2))
                           path->x_pos=(script->x*2);};}

                  else{
                       x_equal=TRUE;};

                  /*Is there anything to do on the Y coordinates? */
                  if (path->y_pos!=(script->y*2)){
                     y_equal=FALSE;

                     /*Increase of decrease current Y to move toward desired Y. */
                     if (path->y_pos>(script->y*2)){
                        path->y_pos=(path->y_pos-path->speed);
                        if (path->y_pos<(script->y*2))
                           path->y_pos=(script->y*2);}
                     else{
                        path->y_pos=(path->y_pos+path->speed);
                        if (path->y_pos>(script->y*2))
                           path->y_pos=(script->y*2);};}
                  else{
                     y_equal=TRUE;};};

               /*If we have already reached our desired X and Y coordinates, or we are in offset mode, time to advance the script. */
               if (x_equal&&y_equal){
                  
                  /*Is this a heat seeker? If so, it explodes when it reaches its target. (More logic to come here.) */
                  if (path->mode&64){}

                  else{
                     update_script(path,script);};};};};};
      FINISHED;};
}

void update_script(struct Path *path,struct CoordPair *script)
{
   /*Are we in seek mode? */
   if (path->mode&2){
      path->seek_count=(path->seek_count-1);
      if (path->seek_count)
         return;

      /*Turn off seek mode if counter has reached 0. */
      else
         path->mode=(path->mode-2);};

   /*Advance to the next script entry. */
   script++;
   path->table_offset=(UWORD)((UBYTE *)script-(UBYTE *)path);

   /*X values not in the range of 0xe0 to 0xee are simple coordinates or offsets. */
   if ((script->x<0xe0)||(script->x>0xee)){

      /*X of 0 when not in offset mode means that the script ends and the alien is no longer active. */
      if ((script->x==0)&&!(path->mode&1))
         path->x_pos=0;
      return;};

   /*X values from 0xe0 to 0xee are special commands. We jump to the appropriate functions to deal with them. */
   switch (script->x){
      case 0xe0:
         init_pause(path,script);
         return;
      case 0xe1:
         loop_back(path,script);
         return;
      case 0xe2:
         toggle_offset(path,script);
         return;
      case 0xe3:
         change_speed(path,script);
         return;
      case 0xe4:
         change_sprite(path,script);
         return;
      case 0xe5:
         seek_mode(path,script);
         return;
      case 0xe6:
         reload_coords(path,script);
         return;
      case 0xe7:
         new_table(path,script);
         return;
      case 0xe8:
         restore_offset(path,script);
         return;
      case 0xe9:
         fire_heatseeker(path,script);
         return;
      case 0xea:
         change_anim(path,script);
         return;
      case 0xeb:
         restart_table(path,script);
         return;
      case 0xec:
         start_xy(path,script);
         return;
      case 0xed:
         start_seekx(path,script);
         return;
      case 0xee:
         start_seeky(path,script);};
}

void check_seek(struct Path *path,struct CoordPair *script)
{
   /*If bit 6 is set, we seek on the ship's X coordinate. */
   if (path->mode&64){

      /*Make sure Y doesn't get changed. */
      y_equal=TRUE;

      /*Get new X coordinate to go to, accounting for the difference between hardware sprite coordinates and our playfield pixels. We divide locations by 2 so that they can fit into a single byte. */
      script->x=((xpos+54)/2);

      /*If seek_count has reached 0, discontinue seek mode. */
      path->seek_count=path->seek_count-1;
      if (path->seek_count==0)
         path->mode=(path->mode-64);};

   /*Bit 7 means we seek on the ship's Y coordinate. */
   if (path->mode&128){

      /*Make sure X doesn't get changed. */
      x_equal=TRUE;
      
      /*Get new Y coordinate to go to. */
      script->y=((ypos+14)/2);
      
      /*Discontinue seek mode if we've reached 0. */
      path->seek_count=(path->seek_count-1);
      if (path->seek_count==0)
         path->mode=(path->mode-128);};

   /*Bit 5 means this is a heat-seeking mine, seeking on both X and Y coordinates. */
   if (path->mode&32){
      
      /*Get new X coordinate to go to. */
      script->x=((xpos+56)/2);

      /*Get new Y coordinate to go to. */
      script->y=((ypos+16)/2);
      
      /*And decrement seek_count. We will add logic here later to explode the mine if seek_count has reached 0. */
      path->seek_count=(path->seek_count-1);};
}

struct Path *path_finished(struct Path *path,struct Alien *alien)
{
       /*Save changes to alien_buffer for the graphics routines. */
      all_coords=(all_coords|path->x_pos);
      alien->x_pos=path->x_pos;
      alien->y_pos=path->y_pos;
      alien->sprite_num=path->sprite_num;
      alien->anim_num=path->anim_num;
      return ((struct Path *)((UBYTE *)path+path->next_path));
}

void init_pause(struct Path *path,struct CoordPair *script)
{
   /*Initiate a pause. */
   path->pause_count=script->y;
}

void loop_back(struct Path *path,struct CoordPair *script)
{
   /*Decrement the loop counter. If it is still greater than 0, initiate a loop. */
   path->loop_count=(path->loop_count-1);
   if (path->loop_count){
      
      /*Loop_offset is stored in increments of 2 bytes, the same size as the script structure. */
      script-=path->loop_offset;

      /*Set up the new table_offset for next time around. */
      path->table_offset=(UWORD)((UBYTE *)script-(UBYTE *)path);

      return;};
      
   update_script(path,script);
}

void toggle_offset(struct Path *path,struct CoordPair *script)
{
   /*Start or end offset movement mode. */
   if (path->mode&1)
      path->mode=(path->mode&0xfe);
   else
      path->mode=(path->mode|1);

   /*Advance the script again. */
   update_script(path,script);
}

void seek_mode(struct Path *path,struct CoordPair *script)
{
   /*Seek mode is signalled by bit 1 in mode. */
   path->mode=(path->mode|2);
   
   /*Seek mode continues for y cycles. */
   path->seek_count=script->y;

   script++;
   path->table_offset=(UWORD)((UBYTE *)script-(UBYTE *)path);
   copy_coords(script);
}

void start_seekx(struct Path *path,struct CoordPair *script)
{
   /*Set the "seek on X" bit. */
   path->mode=(path->mode|64);
   
   /*Seek mode continues for y cycles. */
   path->seek_count=script->y;

   script++;
   path->table_offset=(UWORD)((UBYTE *)script-(UBYTE *)path);
}

void start_seeky(struct Path *path,struct CoordPair *script)
{
   /*Set the "seek on y" bit. */
   path->mode=(path->mode|128);
   
   /*Seek mode continues for y cycles. */
   path->seek_count=script->y;

   script++;
   path->table_offset=(UWORD)((UBYTE *)script-(UBYTE *)path);
}

void copy_coords(struct CoordPair *script)
{
   /*Copy the coordinates of the player's spaceship into an alien script which has seek mode enabled. */
   script->x=((xpos+54)/2);
   script->y=((ypos+14)/2);
}

void change_speed(struct Path *path,struct CoordPair *script)
{
   /*Change the speed of an alien. The new speed is stored in y. */
   path->speed=script->y;
   update_script(path,script);
}

void change_sprite(struct Path *path,struct CoordPair *script)
{
   script++;
   
   /*Plug in the new sprite number, stored in the X coordinate of the next coordinate pair. */
   path->sprite_num=script->x;
   
   /*The number of animations is stored in the Y coordinate. */
   path->num_anims=script->y;
   
   /*Reset animation counter and delay. */
   path->anim_num=0;
   path->anim_delay=path->anim_delay2;
   
   update_script(path,script);
}

void reload_coords(struct Path *path,struct CoordPair *script)
{
   /*Immediately reposition the alien at a new set of X and Y coordinates. */
   script++;
   path->x_pos=(script->x*2);
   path->y_pos=(script->y*2);
   update_script(path,script);
}

void new_table(struct Path *path,struct CoordPair *script)
{
   /*Start a new script, saving the current location in the old one into loop_offset. */

   script++;
   path->loop_offset=(UWORD)((UBYTE *)script-(UBYTE *)path);
   script=(struct CoordPair *)((ULONG)path+(((ULONG)script->x<<8)+script->y));
   update_script(path,script-1);
}

void restore_offset(struct Path *path,struct CoordPair *script)
{
   /*Restore the old script stored in loop_offset. */

   script=(struct CoordPair *)((UBYTE *)path+path->loop_offset);
   update_script(path,script);
}

void fire_heatseeker(struct Path *path,struct CoordPair *script)
{
   struct Path *seeker;
   int i;

   /*Start a new heat seeker path from the current X and Y coordinates of this path. */
   seeker=(struct Path *)path_buffer;
   script++;
   
   /*We need to find an available heat seeker path. */
   for (i=0;i<12;i++){
      if ((seeker->mode&32)&&(seeker->x_pos==0)){
         seeker->x_pos=path->x_pos;
         seeker->y_pos=path->y_pos;
         seeker->seek_count=script->x;
         seeker->hits_num=script->y;
         seeker->sprite_num=2;
         seeker->anim_delay=2;
         seeker->anim_num=0;
         seeker->num_anims=3;
         break;};
      seeker=(struct Path *)((UBYTE *)seeker+seeker->next_path);};
         
   update_script(path,script);
}

void change_anim(struct Path *path,struct CoordPair *script)
{
   /*New animation number stored in y. */
   path->anim_num=script->y;
   update_script(path,script);
}

void restart_table(struct Path *path,struct CoordPair *script)
{
   /*Restart this path from the top. */
   script=(struct CoordPair *)((UBYTE *)path+(TableSize-2));
   update_script(path,script);
}

void start_xy(struct Path *path,struct CoordPair *script)
{
   /*Start a new path. The path number is stored in y. */
   struct Path *new_path;
   struct CoordPair *new_script;
   int i;

   /*Find the requested path. Note that path 0 cannot be selected. */
   new_path=(struct Path *)path_buffer;
   for (i=1;i<=script->y;i++)
      new_path=(struct Path *)((UBYTE *)new_path+new_path->next_path);

   new_script=(struct CoordPair *)((UBYTE *)new_path+TableSize);
   
   /*Start the new path at the current path's coordinates. */
   new_path->x_pos=path->x_pos;
   new_path->y_pos=path->y_pos;
   
   /*And set up the beginning of the script. */
   new_script->x=(new_path->x_pos/2);
   new_script->y=(new_path->y_pos/2);
   new_path->table_offset=TableSize;
   
   /*Indestructable. */
   new_path->hits_num=128;
   
   /*Reset animation. */
   new_path->anim_delay=2;
   new_path->pause_count=0;
   new_path->anim_num=0;

   update_script(path,script);
}

void save_aliens()
{
   UBYTE *buffer;
   struct Background *screenbgnds;
   int i;
   int j=0;

   screenbgnds=getscreeninfo((ULONG *)&buffer);

   /*Pass X and Y coordinates of each active alien to savebgnds, along with data spaces for it to use. */
   for (i=11;i>=0;i--)
      if (alien_buffer[i].x_pos){
         savebgnds(alien_buffer[i].x_pos,alien_buffer[i].y_pos,&buffer[j*(3*6*24)],
            &screenbgnds[j]);
         j++;};
         
   screenbgnds[j].size=0;
}

void draw_aliens()
{
   UBYTE *buffer,*alien;
   struct Background *screenbgnds;
   int i;
   int j=0;

   Hardware->bltalwm=0;
   screenbgnds=getscreeninfo((ULONG *)&buffer);

   /*We word backward through alien_buffer. */
   for (i=11;i>=0;i--){
      if (alien_buffer[i].x_pos){
         if (alien_buffer[i].sprite_num&128)
            upsidedown=(23*100);
         else
            upsidedown=0;

         /*Work out the address where the alien image data is stored. The mask data for all 3 bitplanes is always at this address + 288. */
         alien=&aliens[alien_pointers[(alien_buffer[i].sprite_num&0x7f)+(24*level_number)]]+
             (alien_buffer[i].anim_num*384);
         drawbobs(&screenbgnds[j],alien,(alien+288));

         j++;};};

   Hardware->bltalwm=0xffff;
}

void blit_to_backgnd(struct Background *screenbgnd,UBYTE *mask)
{
   /*Blit all 3 bitplanes representing an alien to the background playfield. */
   
   /*Plane 1. Channel A = mask data. Channel C = Screen playfield (source). Channel D = Screen playfield (destination). */
   Hardware->bltcpt=(APTR)(screenbgnd->plane1+upsidedown);
   Hardware->bltdpt=(APTR)(screenbgnd->plane1+upsidedown);
   Hardware->bltapt=(APTR)mask;
   Hardware->bltsize=screenbgnd->size;

   /*Plane 2. */
   Hardware->bltcpt=(APTR)(screenbgnd->plane2+upsidedown);
   Hardware->bltdpt=(APTR)(screenbgnd->plane2+upsidedown);
   Hardware->bltapt=(APTR)mask;
   Hardware->bltsize=screenbgnd->size;

   /*Plane 3. */
   Hardware->bltcpt=(APTR)(screenbgnd->plane3+upsidedown);
   Hardware->bltdpt=(APTR)(screenbgnd->plane3+upsidedown);
   Hardware->bltapt=(APTR)mask;
   Hardware->bltsize=screenbgnd->size;
}

void blit_to_buffer(UBYTE *buffer,struct Background *screenbgnd)
{
   
   /*Copy an area of the background into a buffer before drawing in an alien. */

   Hardware->bltdpt=(APTR)buffer;
   Hardware->bltapt=(APTR)screenbgnd->plane1;
   Hardware->bltsize=screenbgnd->size;
   Hardware->bltapt=(APTR)screenbgnd->plane2;
   Hardware->bltsize=screenbgnd->size;
}

void coords_to_pf1offsets(ULONG x,ULONG y,UWORD *boffset,UWORD *soffset)
{
   /*Convert a pair of X and Y coordinates into a playfield byte offset and scroll value. */

   x+=(16-pf1scroll2);
   y=((((y*100)<<16)+x)&0xffff000f);
   y=(y<<16)+(y>>16);
   x=(x>>3)+y;
   y=(y>>16);

   *boffset=x;
   *soffset=y;
}

void coords_to_pf2offsets(ULONG x,ULONG y,UWORD *boffset,UWORD *soffset)
{
   /*Same as above, only for the foreground playfield, which is 92 rather than 100 bytes wide. In this case we return the scroll already shifted into the top 4 bits of the word, ready to be plugged into a bltcon register for a source scroll. */

   x-=pf2scroll;
   y=((((y*92)<<16)+x)&0xffff000f);
   y=(y<<16)+(y>>16);
   x=(x>>3)+y;
   y=((y>>16)<<12);
   x+=pf2offset;

   *boffset=x;
   *soffset=y;
}

BOOL setup_addresses(struct Background *screenbgnd,UWORD size,UWORD soffset,UWORD boffset,
PLANEPTR *planes)
{
   /*Set up the data structure holding information about a blit. */

   screenbgnd->size=aliensize;
   screenbgnd->scroll=soffset;

   /*Are the pointers already set up correctly? */
   if (screenbgnd->plane1==(planes[0]+boffset))
      return (FALSE);

   screenbgnd->plane1=planes[0]+boffset;
   screenbgnd->plane2=planes[1]+boffset;
   screenbgnd->plane3=planes[2]+boffset;
   return (TRUE);
}

PLANEPTR *check_end_screen()
{
   if (screenend==0){
      pf1scroll2=16;
      return (rasters);};

   pf1scroll2=pf1scroll;
   return (displayraster);
}

void check_path()
{
   UBYTE *bptr;
   UWORD *wptr;
   ULONG *lptr;
   struct Path *path;
   int i;

   /*We start a new path when only all X coordinates of the current are 0. */
   if (all_coords!=0)
      return;
      
   /*If we have reached the end boss, no need to start more paths. */
   if (level_end>0)
      return;
      
   path_delay--;
   if (path_delay>0)
      return;

   /*Start a normal path. */
   path_delay=10;

   /*We copy the next path into path_buffer and set up the bottom 8 color registers to conform with the new sprites. */
   lptr=(ULONG *)&paths[path_number];
   if (*lptr==0){
      path_number=0;
      lptr=(ULONG *)&paths[0];};
   path_number+=4;
   path=(struct Path *)((ULONG)paths+*lptr+2);
   
   /*The colors for the path are stored in the alien_colors table; 8 words (colors) per sprite. */
   i=((192*level_number)+(((path->sprite_num&0x7f)-1)*8));

   /*Copy the colors into the copper list. */
   copperlist[45]=alien_colors[i];
   copperlist[47]=alien_colors[i+1];
   copperlist[49]=alien_colors[i+2];
   copperlist[51]=alien_colors[i+3];
   copperlist[53]=alien_colors[i+4];
   copperlist[55]=alien_colors[i+5];
   copperlist[57]=alien_colors[i+6];
   copperlist[59]=alien_colors[i+7];

   wptr=(UWORD *)((ULONG)paths+*lptr);
   bptr=(UBYTE *)path;
   for (i=0;i<*wptr;i++)
       path_buffer[i]=bptr[i];

}

void update_missiles()
{
   int i;

   if (fire_delay)
      fire_delay--;

   /*Loop through the 3 possible missiles. */
   for (i=2;i>=0;i--){
      if (activemissiles&(1<<i)){
         

         missiledata[i].range--;
         missiledata[i].x_pos+=10;
         
         /*If it has reached its range limit or has moved offscreen, deactivate it. */
         if (missiledata[i].range==0||missiledata[i].x_pos>360){
            activemissiles-=(1<<i);}
            
         /*Otherwise we need to redraw it and check for collisions with aliens. */
         else{
            if (draw_missile(missiledata[i].x_pos,missiledata[i].y_pos,&missiletable[i],0x82,
               &fgnd_buffer[i*24],&ships[missiles1+(72*level_number)+16],&ships[missilemask+16])){
                  
               /*Missile tip for collision detection is x + 8, y + 2. */
               kill_alien((UWORD)(missiledata[i].x_pos+8),(UWORD)(missiledata[i].y_pos+2));
               activemissiles-=(1<<i);};};};};
}

void kill_alien(UWORD x,UWORD y)
{
   struct Path *path,*kill_path;
   int i,j;

   path=(struct Path *)path_buffer;

   for (i=0;i<12;i++){
      
      /*Is alien already exploding? Is it active? */
      if (path->sprite_num&&path->x_pos){
         
         /*Is alien within a box around the missile? */
         if ((x>(path->x_pos-16))&&(x<(path->x_pos+16))){
            if ((y>path->y_pos)&&(y<(path->y_pos+24))){
               
               /*Is alien indestructable? */
               if (path->hits_num&128)
                  return;
                  
               /*It's a hit! */
               path->hits_num=(path->hits_num-1);
               if (path->hits_num)
                  return;
               explode_alien(path);
               kill_path=(struct Path *)path_buffer;
               for (j=0;j<12;j++){
                  if (path->kills_what&(1<<j))
                     explode_alien(kill_path);
                  kill_path=(struct Path *)((UBYTE *)kill_path+kill_path->next_path);};
               return;};};};

      path=(struct Path *)((UBYTE *)path+path->next_path);};
}

void explode_alien(struct Path *path)
{
   /*Start an explosion animation. */
   path->anim_num=0;
   path->sprite_num=0;
   path->num_anims=7;
   path->anim_delay=2;
   path->anim_delay2=2;
   
   /*Turn off animation cycle mode if currently enabled. */
   if (path->mode&8)
      path->mode=(path->mode-8);
}

BOOL draw_missile(UWORD x,UWORD y,struct Background *table,UWORD size,UBYTE *buffer,UBYTE *sprite,
     UBYTE *mask)
{
   UWORD boffset,scroll;

   /*Convert X and Y coordinates to a playfield byte offset and scroll. */
   coords_to_pf2offsets(x,y,&boffset,&scroll);

   or_foregnds(table,size,boffset,buffer);

   /*Copy sprite data & mask data NOT ORed foreground data into the buffer. */
   Hardware->bltcmod=0;
   Hardware->bltamod=0;
   Hardware->bltdmod=0;
   Hardware->bltbmod=0;
   Hardware->bltapt=(APTR)sprite;
   Hardware->bltbpt=(APTR)mask;
   Hardware->bltdpt=(APTR)&buffer[(size>>4)];
   Hardware->bltcon1=scroll;
   Hardware->bltcon0=(scroll|0xf40);
   Hardware->bltcpt=(APTR)buffer;
   Hardware->bltsize=size;
   Hardware->bltcpt=(APTR)buffer;
   Hardware->bltsize=size;

   /*Copy buffer | foreground playfield into the foreground playfield. */
   Hardware->bltdmod=88;
   Hardware->bltbmod=88;
   Hardware->bltcon1=0;
   Hardware->bltcon0=0xdfc;
   Hardware->bltapt=(APTR)&buffer[(size>>4)];
   Hardware->bltbpt=(APTR)table->plane1;
   Hardware->bltdpt=(APTR)table->plane1;
   Hardware->bltsize=size;
   Hardware->bltbpt=(APTR)table->plane2;
   Hardware->bltdpt=(APTR)table->plane2;
   Hardware->bltsize=size;
   table->plane3=0;

   /*Check for collision with an alien in the background playfield by doing sprite data & background bitplane 3. We are only checking for collisions, so channel D goes unused. */
   pf1scroll2=pf1scroll;
   coords_to_pf1offsets(x,y,&boffset,&scroll);
   Hardware->bltcon0=((scroll<<12)+0xaa0);
   Hardware->bltapt=(APTR)sprite;
   Hardware->bltcpt=(APTR)(displayraster[5]+boffset);
   Hardware->bltcmod=96;
   Hardware->bltsize=size;
   
   if (Hardware->dmaconr&0x2000)
      return (FALSE);
   else
      return (TRUE);
}

void or_foregnds(struct Background *table,UWORD size,UWORD boffset,UBYTE *buffer)
{
   /*OR 2 foreground bitplanes together and store them for use as a mask for a missile BOB. */

   Hardware->bltamod=88;
   Hardware->bltbmod=88;
   Hardware->bltdmod=0;

   /*Store the bltsize. */
   table->size=size;
   
   /*Plane 1. */
   table->plane1=rasters[3]+boffset;
   Hardware->bltapt=(APTR)table->plane1;
   
   /*Plane 2. */
   table->plane2=rasters[4]+boffset;
   Hardware->bltbpt=(APTR)table->plane2;
   
   /*Copy channels A and B to D, ORing them together in the process. */
   Hardware->bltcon1=0;
   Hardware->bltcon0=0xffc;
   Hardware->bltdpt=(APTR)buffer;
   Hardware->bltsize=size;
}

void erase_missiles()
{
   /*Copy the old foreground back in from the buffer to erase a missile. We do a logical AND on the current foreground (source A) and the foreground stored in the buffer (source B) in the course of the copy. */
   int i=0;

   Hardware->bltcon1=0;
   Hardware->bltamod=0;
   Hardware->bltcon0=0xdc0;
   Hardware->bltbmod=88;
   Hardware->bltdmod=88;

   for (i=0;i<3;i++)
   
      /*A size of 0 indicates this missile is not currently active. */
      if (missiletable[i].size){
         Hardware->bltbpt=(APTR)missiletable[i].plane1;
         Hardware->bltdpt=(APTR)missiletable[i].plane1;
         Hardware->bltapt=(APTR)&fgnd_buffer[i*(missiletable[i].size>>4)*3];
         Hardware->bltsize=missiletable[i].size;
         Hardware->bltbpt=(APTR)missiletable[i].plane2;
         Hardware->bltdpt=(APTR)missiletable[i].plane2;
         Hardware->bltapt=(APTR)&fgnd_buffer[i*(missiletable[i].size>>4)*3];
         Hardware->bltsize=missiletable[i].size;
         missiletable[i].size=0;};
}

void CleanExit(BOOL freesystem)
{
   if (freesystem)
      FreeSystem();

   /*Free the memory we allocated. */
   if (MemBase)
      FreeMem(MemBase,MemNeeded);
   if (map)
      FreeMem(map,mapSize);
   if (panel)
      FreeMem(panel,panelSize);
   if (graphics)
      FreeMem(graphics,graphicsSize);
   if (ships)
      FreeMem(ships,shipsSize);
   if (paths)
      FreeMem(paths,pathsSize);
   if (aliens)
      FreeMem(aliens,aliensSize);

   /*Close the graphics and DOS libraries and end the program. */
   CloseLibrary((struct Library *)GraphicsBase);
   CloseLibrary((struct Library *)DOSBase);
   exit(0);
}

void TakeSystem()
{
   /*Save current interrupts and DMA settings so we can restore them upon exit. */
   SystemInts=Hardware->intenar;
   SystemDMA=Hardware->dmaconr;

   /*Kill all interrupts and DMA. */
   Hardware->intena=0x7fff;
   Hardware->dmacon=0x7fff;

   /*Also kill interrupts at the CIA controller itself, which handles keyboard interrupts among others, just to be on the safe side. */
   *ICRA=0x7f;

   /*Save current vectors for level 2 and 3 interrupts (those dealing with keyboard and other I/O and with the video hardware), so that we can replace them with new routines. */
   Level2Vector=*(ULONG *)0x68;
   Level3Vector=*(ULONG *)0x6c;
}

void FreeSystem()
{
   /*Restore level 2 and 3 interrupt vectors. */
   *(ULONG *)0x68=Level2Vector;
   *(ULONG *)0x6c=Level3Vector;

   /*Restore system copper list(s). */
   Hardware->cop1lc=(ULONG)GraphicsBase->copinit;
   Hardware->cop2lc=(ULONG)GraphicsBase->LOFlist;

   /*Restore all interrupts and DMA settings. */
   Hardware->intena=(SystemInts|0xc000);
   Hardware->dmacon=(SystemDMA|0x8100);
   *ICRA=0x9b;
}

UBYTE *readFile(char *filename,int *size,ULONG memType)
{
   BPTR FileLock=NULL;
   struct FileInfoBlock *FileInfoBlock=NULL;
   UBYTE *data;
   BPTR File=NULL;

   FileLock=Lock(filename,ACCESS_READ);
   if (FileLock==NULL)
      return(NULL);

   FileInfoBlock=(struct FileInfoBlock *)AllocMem(sizeof(struct FileInfoBlock),NULL);
   if (FileInfoBlock==NULL)
      return(NULL);

   if (Examine(FileLock,FileInfoBlock)==FALSE)
      return(NULL);

   *size=FileInfoBlock->fib_Size;

   data=(UBYTE *)AllocMem(*size,memType);
   if (data==NULL)
      return(NULL);

   File=Open(filename,MODE_OLDFILE);
      if (File==NULL)
         return(NULL);

    Read(File,data,*size);

    Close(File);
    UnLock(FileLock);
    FreeMem(FileInfoBlock,sizeof(struct FileInfoBlock));

    return (data);
}

