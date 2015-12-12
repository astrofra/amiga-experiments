/* 
    Rog by Funkentstört,
    Amiga port.
*/

#include "includes.prl"

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <devices/keyboard.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <graphics/copper.h>
#include <graphics/videocontrol.h>
#include <clib/timer_protos.h>
#include <graphics/sprite.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <clib/graphics_protos.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#include "input_routines.h"
#include "bitmap_routines.h"
#include "cosine_table.h"
#include "common.h"

#include "image_bg_fish.h"
#include "cat_copper_imgdata.h"
#include "cat_copper_list.h"
#include "ilkke_font.h"

#define OPT_CREATE_WINDOW   FALSE
#define SCR_WIDTH           320
#define SCR_HEIGHT          256
#define SCROLLTEXT_HEIGHT   32
#define TOP_MARGIN          18
#define BOTTOM_MARGIN       32
#define OVERSCAN_W          80
#define OVERSCAN_H          64

void doDualPF (void);
BOOL installDualPF( struct Screen *, struct RasInfo * );
void handleIDCMP ( struct Window * );
void removeDualPF( struct Screen *s );
void drawBackgroundFish(struct BitMap *dest_bitmap);

struct Library *IntuitionBase;
struct Library *GfxBase;

struct Window   *win;
struct Screen   *scr;

struct Screen   *myscreen;
struct RasInfo  *rinfo2;
struct BitMap   *bmap2;
struct RastPort *rport2;

short bg_scroll_x = 0, bg_scroll_y = 0, bg_scroll_phase = 0;

extern struct Custom custom;

/*
    Demo data
*/

struct  BitMap *bitmap_bg_fish;
struct  BitMap *bitmap_cat;

UWORD chip blank_pointer[32]=
{
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
};

void InitMainScreen(void)
{
    struct Rectangle    screen_clip;

    screen_clip.MinX = 0;
    screen_clip.MinY = 0;
    screen_clip.MaxX = SCR_WIDTH - 1;
    screen_clip.MaxY = SCR_HEIGHT - 1;

    IntuitionBase = OpenLibrary("intuition.library",37);
    if (IntuitionBase != NULL)
    {
        GfxBase = OpenLibrary("graphics.library", 37);
        if (GfxBase != NULL)
        {
            scr = OpenScreenTags(NULL,
                                 SA_Depth,     3,
                                 SA_Width,   SCR_WIDTH + OVERSCAN_W,
                                 SA_Height,  SCR_HEIGHT + OVERSCAN_H,                             
                                 SA_DisplayID, 0,
                                 SA_Title,     "Rog",
                                 SA_DClip, &screen_clip,
                                 TAG_END);
            if (OPT_CREATE_WINDOW && scr != NULL )
            {
                win = OpenWindowTags(NULL,
                                     WA_Title,        "Rog",
                                     WA_IDCMP,        IDCMP_CLOSEWINDOW,
                                     WA_Width,        SCR_WIDTH + OVERSCAN_W,
                                     WA_Height,       SCR_HEIGHT + OVERSCAN_H,
                                     WA_DragBar,        FALSE,
                                     WA_CloseGadget,    FALSE,
                                     WA_Borderless,     FALSE,
                                     WA_NoCareRefresh,  TRUE,
                                     WA_RMBTrap,        TRUE,
                                     WA_CustomScreen, scr,
                                     TAG_END);
                // if ( win != NULL )
                //     doDualPF(win);
            }

            doDualPF();
        }
    }
}

VOID doDualPF(void)
{
    myscreen = scr; //win->WScreen;   /* Find the window's screen */

    SetRast(&(myscreen->RastPort), 0);

    /* Allocate the second playfield's rasinfo, bitmap, and bitplane */
    rinfo2 = (struct RasInfo *) AllocMem(sizeof(struct RasInfo), MEMF_PUBLIC | MEMF_CLEAR);
    if ( rinfo2 != NULL )
    {
        /* Get a rastport, and set it up for rendering into bmap2 */
        rport2 = (struct RastPort *) AllocMem(sizeof(struct RastPort), MEMF_PUBLIC );
        if (rport2 != NULL )
        {
            bmap2 = (struct BitMap *) AllocMem(sizeof(struct BitMap), MEMF_PUBLIC | MEMF_CLEAR);
            if (bmap2 != NULL )
            {
                InitBitMap(bmap2, 3, myscreen->Width, myscreen->Height);

                /* extra playfield will only use one bitplane here. */
                bmap2->Planes[0] = (PLANEPTR) AllocRaster(myscreen->Width, myscreen->Height);
                bmap2->Planes[1] = (PLANEPTR) AllocRaster(myscreen->Width, myscreen->Height);
                bmap2->Planes[2] = (PLANEPTR) AllocRaster(myscreen->Width, myscreen->Height);
                if (bmap2->Planes[0] != NULL )
                {
                    InitRastPort(rport2);
                    rport2->BitMap = rinfo2->BitMap = bmap2;

                    SetRast(rport2, 0);

                    if (installDualPF(myscreen,rinfo2))
                    {
                        /* Set foreground color; color 9 is color 1 for
                        ** second playfield of hi-res viewport
                        */
                        // SetRGB4(&myscreen->ViewPort, 9, 0, 0xF, 0);
                        
                        //drawSomething(rport2, 1);
                    }
                }
            }
        }
    }
}

BOOL installDualPF(struct Screen *scrn, struct RasInfo *rinfo2)
{
    ULONG screen_modeID;
    BOOL return_code = FALSE;

    screen_modeID = GetVPModeID(&(scrn->ViewPort));
    if( screen_modeID != INVALID_ID )
    {
        /* you can only play with the bits in the Modes field
        ** if the upper half of the screen mode ID is zero!!!
        */
        if ( (screen_modeID & 0xFFFF0000L) == 0L )
        {
            return_code = TRUE;

            Forbid();

            /* Install rinfo for viewport's second playfield */
            scrn->ViewPort.RasInfo->Next = rinfo2;
            scrn->ViewPort.Modes |= (DUALPF | PFBA);

            Permit();

            /* Put viewport change into effect */
            MakeScreen(scrn);
            RethinkDisplay();
        }
    }

    return(return_code);
}

/*
** remove the effects of installDualPF().
** only call if installDualPF() succeeded.
*/
VOID removeDualPF(struct Screen *scrn)
{
    Forbid();

    scrn->ViewPort.RasInfo->Next = NULL;
    scrn->ViewPort.Modes &= ~DUALPF;
    // scrn->ViewPort.Modes &= ~PFBA;

    Permit();

    MakeScreen(scrn);
    RethinkDisplay();
}

/*  Demo exits  */
void  DemoEnd(void)
{
    if (myscreen) removeDualPF(myscreen);

    if (bmap2->Planes[0]) FreeRaster(bmap2->Planes[0], myscreen->Width, myscreen->Height);
    if (bmap2->Planes[1]) FreeRaster(bmap2->Planes[1], myscreen->Width, myscreen->Height);
    if (bmap2->Planes[2]) FreeRaster(bmap2->Planes[2], myscreen->Width, myscreen->Height);

    if (bmap2) FreeMem(bmap2, sizeof(struct BitMap));

    if (rport2) FreeMem(rport2, sizeof(struct RastPort));

    if (rinfo2) FreeMem(rinfo2, sizeof(struct RasInfo));

    if (win) CloseWindow(win);
    if (scr) CloseScreen(scr);
    if (GfxBase) CloseLibrary(GfxBase);
    if (IntuitionBase) CloseLibrary(IntuitionBase);

    exit(0);
}

void ScrollBackground(void)
{
    bg_scroll_phase += 2;

    if (bg_scroll_phase >= COSINE_TABLE_LEN)
        bg_scroll_phase -= COSINE_TABLE_LEN;

    bg_scroll_x = OVERSCAN_W + ((tcos[bg_scroll_phase] + 512) * OVERSCAN_W) >> 10;
    bg_scroll_y = OVERSCAN_H + ((tsin[(bg_scroll_phase << 1) & 511] + 512) * OVERSCAN_H) >> 11;

    myscreen->ViewPort.RasInfo->RxOffset = bg_scroll_x;
    myscreen->ViewPort.RasInfo->RyOffset = bg_scroll_y;
    ScrollVPort(&(myscreen->ViewPort));
}

/*
    Fish background
*/
void drawBackgroundFish(struct BitMap *dest_bitmap)
{
    USHORT i,j;
    USHORT c, fish_padding;
    bitmap_bg_fish = load_array_as_bitmap(image_bg_fishData, 32 * 2, image_bg_fish.Width, image_bg_fish.Height, image_bg_fish.Depth);

    fish_padding = 2;
    for (j = 0; j + image_bg_fish.Height < SCR_HEIGHT + OVERSCAN_H; j += image_bg_fish.Height + fish_padding)
        for (i = 0; i + image_bg_fish.Width < SCR_WIDTH + OVERSCAN_W; i += image_bg_fish.Width + fish_padding)
            BLIT_BITMAP_S(bitmap_bg_fish, dest_bitmap, image_bg_fish.Width, image_bg_fish.Height, i, j);

    for(c = 0; c < 4; c++)
        SetRGB4(&myscreen->ViewPort, c, (image_bg_fishPaletteRGB4[c] & 0x0f00) >> 8, (image_bg_fishPaletteRGB4[c] & 0x00f0) >> 4, (image_bg_fishPaletteRGB4[c] & 0x000f));
}

void drawForegroundCat(struct BitMap *dest_bitmap, USHORT offset_y)
{
    /*
        Cat head
    */
    bitmap_cat = load_array_as_bitmap(cat_copper_imdata, 7728, cat_copper_image.Width, cat_copper_image.Height, cat_copper_image.Depth);
    BLIT_BITMAP_S(bitmap_cat, dest_bitmap, cat_copper_image.Width, cat_copper_image.Height, (SCR_WIDTH - cat_copper_image.Width) / 2, offset_y);    
}

void setForegroundCopperList(USHORT offset_y)
{
    struct UCopList *cl;
    struct TagItem  uCopTags[] =
            {
                  { VTAG_USERCLIP_SET, NULL },
                  { VTAG_END_CM, NULL }
            };

    short   _idx = 0,
            scanline_y,
            scanline_color_size,
            n;

    printf("setForegroundCopperList()\n");

    cl = (struct UCopList *) AllocMem(sizeof(struct UCopList), MEMF_PUBLIC|MEMF_CLEAR);

    CWAIT(cl, 0, 0);

    CMOVE(cl, *((UWORD *)0xDFF120), (LONG)&blank_pointer);
    CMOVE(cl, *((UWORD *)0xDFF120), (LONG)&blank_pointer);

    for(n = 0; n < 15; n++)
        CMOVE(cl, custom.color[n], 0x000);

    CWAIT(cl, TOP_MARGIN, 0);
    for(n = 0; n < 4; n++)
        CMOVE(cl, custom.color[n], image_bg_fishPaletteRGB4[n]);

    while(cat_copper_palette[_idx] != 999) // && _idx < 50)
    {
        //  Fetch the current scanline Y coord
        scanline_y = cat_copper_palette[_idx];

        //  Fetch the current scanline Y coord
        _idx++;
        scanline_color_size = cat_copper_palette[_idx];

        CWAIT(cl, scanline_y + offset_y, 0);
        for (n = 0; n  < scanline_color_size; n++)
        {
            _idx++;
            if (n > 0 && scanline_y + offset_y > TOP_MARGIN && scanline_y + offset_y < SCR_HEIGHT - BOTTOM_MARGIN)
                CMOVE(cl, custom.color[n] + 8, cat_copper_palette[_idx]);
        }

        //  Go to the next scanline
        _idx++;
    }

    CWAIT(cl, SCR_HEIGHT - BOTTOM_MARGIN, 0);
    for(n = 0; n < 15; n++)
        CMOVE(cl, custom.color[n], 0x000);

    CEND(cl);

    Forbid();       /*  Forbid task switching while changing the Copper list.  */
    (myscreen->ViewPort).UCopIns = cl;
    Permit();       /*  Permit task switching again.  */

    (VOID) VideoControl( (myscreen->ViewPort).ColorMap, uCopTags );
    RethinkDisplay();
}

void displayScrollText(void)
{
    
}

/* Main program entry point */
int main(void)
{
    USHORT c;
    USHORT cat_y;

    InitKeyboardDevice();
    InitMainScreen();

    cat_y = ((SCR_HEIGHT - cat_copper_image.Height) >> 1) + 4;

    setForegroundCopperList(cat_y);
    drawBackgroundFish(&(myscreen->BitMap));
    drawForegroundCat(rport2->BitMap, cat_y);

    // displayScrollText("this is an amiga remake");


    for(c = 0; c < 7; c++)
        SetRGB4(&myscreen->ViewPort, c + 8, (cat_copper_paldataRGB4[c] & 0x0f00) >> 8, (cat_copper_paldataRGB4[c] & 0x00f0) >> 4, (cat_copper_paldataRGB4[c] & 0x000f));

    Forbid();
    Disable();

    while((*(UBYTE *)0xBFE001) & 0x40)
    {
        WaitTOF();
        ScrollBackground();
        // KeyboardAbortCheck(&DemoEnd);
    }

    Enable();
    Permit();    

    DemoEnd();
}
