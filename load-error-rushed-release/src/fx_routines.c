#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>

#include "board.h"
#include "screen_size.h"
#include "color_routines.h"
#include "bitmap_routines.h"
#include "cosine_table.h"
#include "copper_lists.h"

extern void close_demo(STRPTR message);

extern struct Custom far custom;

extern struct GfxBase *GfxBase;
extern struct ViewPort view_port1;

extern int dbuffer_offset_1;
extern int dbuffer_offset_2;

extern struct Image redbot;
extern struct BitMap *bitmap_redbot;

extern struct Image astronaut;
extern struct BitMap *bitmap_astronaut;

extern struct Image demo_title;
extern struct BitMap *bitmap_demo_title;

extern struct Image face;
extern struct BitMap *bitmap_face;

extern struct Image guard;
extern struct BitMap *bitmap_guard;

extern struct Image mountain;
extern struct BitMap *bitmap_mountain;

extern struct Image mummy;
extern struct BitMap *bitmap_mummy;

extern struct Image ufo;
extern struct BitMap *bitmap_ufo;

struct UCopList *copper;

UWORD chip blank_pointer[4]=
{
    0x0000, 0x0000,
    0x0000, 0x0000
};

#define RASTER_CLEAR_HSTEP 64
BOOL progressiveClearRaster(struct RastPort *rp, unsigned int fx_clock, const int max_width, const int max_height, UBYTE color_index)
{
    short x, max_x;
    x = fx_clock * RASTER_CLEAR_HSTEP;

    if (x >= max_width)
        return FALSE;

    max_x = x + RASTER_CLEAR_HSTEP;
    if (max_x >= max_width)
        max_x = max_width - 1;
    SetAPen(rp, color_index);
    RectFill(rp, x, 0, x + RASTER_CLEAR_HSTEP, max_height - 1);

    return TRUE;
}

void setEmptyCopperList(struct ViewPort *vp)
{
    copper = (struct UCopList *)
    AllocMem( sizeof(struct UCopList), MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR );

    CINIT(copper, 16);
    CWAIT(copper, 0, 0);

    CMOVE(copper, *((UWORD *)0xdff1fc), 0x0003);
    CMOVE(copper, *((UWORD *)SPR0PTH_ADDR), (LONG)&blank_pointer);
    CMOVE(copper, *((UWORD *)SPR0PTL_ADDR), (LONG)&blank_pointer);

    CEND(copper);

    vp->UCopIns = copper;    
}

/* redbot
*/
void __inline loadRedbotSprite(void)    {
    bitmap_redbot = load_zlib_file_as_bitmap("assets/redbot.data", 0, 20480, redbot.Width, redbot.Height, redbot.Depth);
    if (bitmap_redbot == NULL)
        close_demo( "Could NOT load or allocate the bitmap (assets/redbot.data)!" );
}

void __inline drawRedbotSprite(struct BitMap *dest_bitmap)  {   
    BLIT_BITMAP_S(bitmap_redbot, dest_bitmap, redbot.Width, redbot.Height, 0, 0);
    WaitBlit();
}

void __inline freeRedbotSprite(void)    {   
    FREE_ALLOCATED_BITMAP(bitmap_redbot);
    bitmap_redbot = NULL; 
}

/* astronaut
*/
void __inline loadAstronautSprite(void) {
    bitmap_astronaut = load_zlib_file_as_bitmap("assets/astronaut.data", 0, 10560, astronaut.Width, astronaut.Height, astronaut.Depth);
    if (bitmap_astronaut == NULL)
        close_demo( "Could NOT load or allocate the bitmap (assets/astronaut.data)!" );
}

void __inline drawAstronautSprite(struct BitMap *dest_bitmap)   {   
    BLIT_BITMAP_S(bitmap_astronaut, dest_bitmap, astronaut.Width, astronaut.Height, 0, 0);
    WaitBlit();
}

void __inline freeAstronautSprite(void) {   
    FREE_ALLOCATED_BITMAP(bitmap_astronaut);
    bitmap_astronaut = NULL; 
}

/* demo_title
*/
void __inline loadTitleSprite(void) {
    bitmap_demo_title = load_zlib_file_as_bitmap("assets/demo_title.data", 0, 10512, demo_title.Width, demo_title.Height, demo_title.Depth);
    if (bitmap_demo_title == NULL)
        close_demo( "Could NOT load or allocate the bitmap (assets/demo_title.data)!" );
}

void __inline drawTitleSprite(struct BitMap *dest_bitmap)   {   
    BLIT_BITMAP_S(bitmap_demo_title, dest_bitmap, demo_title.Width, demo_title.Height, 0, 0);
    WaitBlit();
}

void __inline freeTitleSprite(void) {   
    FREE_ALLOCATED_BITMAP(bitmap_demo_title);
    bitmap_demo_title = NULL; 
}

/* face
*/
void __inline loadFaceSprite(void) {
    bitmap_face = load_zlib_file_as_bitmap("assets/face.data", 0, 25536, face.Width, face.Height, face.Depth);
    if (bitmap_face == NULL)
        close_demo( "Could NOT load or allocate the bitmap (assets/face.data)!" );
}

void __inline drawFaceSprite(struct BitMap *dest_bitmap)   {   
    BLIT_BITMAP_S(bitmap_face, dest_bitmap, face.Width, face.Height, 0, 0);
    WaitBlit();
}

void __inline freeFaceSprite(void) {   
    FREE_ALLOCATED_BITMAP(bitmap_face);
    bitmap_face = NULL; 
}

/* guard
*/
void __inline loadGuardSprite(void) {
    bitmap_guard = load_zlib_file_as_bitmap("assets/guard.data", 0, 10464, guard.Width, guard.Height, guard.Depth);
    if (bitmap_guard == NULL)
        close_demo( "Could NOT load or allocate the bitmap (assets/guard.data)!" );
}

void __inline drawGuardSprite(struct BitMap *dest_bitmap)   {   
    BLIT_BITMAP_S(bitmap_guard, dest_bitmap, guard.Width, guard.Height, 0, 0);
    WaitBlit();
}

void __inline freeGuardSprite(void) {   
    FREE_ALLOCATED_BITMAP(bitmap_guard);
    bitmap_guard = NULL; 
}

/* mountain
*/
void __inline loadMountainSprite(void) {
    bitmap_mountain = load_zlib_file_as_bitmap("assets/mountain.data", 0, 52800, mountain.Width, mountain.Height, mountain.Depth);
    if (bitmap_mountain == NULL)
        close_demo( "Could NOT load or allocate the bitmap (assets/mountain.data)!" );
}

void __inline drawMountainSprite(struct BitMap *dest_bitmap)   {   
    BLIT_BITMAP_S(bitmap_mountain, dest_bitmap, mountain.Width, mountain.Height, 0, 0);
    WaitBlit();
}

void __inline freeMountainSprite(void) {   
    FREE_ALLOCATED_BITMAP(bitmap_mountain);
    bitmap_mountain = NULL; 
}

/* mummy
*/
void __inline loadMummySprite(void) {
    bitmap_mummy = load_zlib_file_as_bitmap("assets/mummy.data", 0, 12288, mummy.Width, mummy.Height, mummy.Depth);
    if (bitmap_mummy == NULL)
        close_demo( "Could NOT load or allocate the bitmap (assets/mummy.data)!" );
}

void __inline drawMummySprite(struct BitMap *dest_bitmap)   {   
    BLIT_BITMAP_S(bitmap_mummy, dest_bitmap, mummy.Width, mummy.Height, 0, 0);
    WaitBlit();
}

void __inline freeMummySprite(void) {   
    FREE_ALLOCATED_BITMAP(bitmap_mummy);
    bitmap_mummy = NULL; 
}

/* ufo
*/
void __inline loadUfoSprite(void) {
    bitmap_ufo = load_zlib_file_as_bitmap("assets/ufo.data", 0, 15104, ufo.Width, ufo.Height, ufo.Depth);
    if (bitmap_ufo == NULL)
        close_demo( "Could NOT load or allocate the bitmap (assets/ufo.data)!" );
}

void __inline drawUfoSprite(struct BitMap *dest_bitmap)   {   
    BLIT_BITMAP_S(bitmap_ufo, dest_bitmap, ufo.Width, ufo.Height, 0, 0);
    WaitBlit();
}

void __inline freeUfoSprite(void) {   
    FREE_ALLOCATED_BITMAP(bitmap_ufo);
    bitmap_ufo = NULL; 
}