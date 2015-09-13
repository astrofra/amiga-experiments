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

extern struct Custom far custom;

extern struct GfxBase *GfxBase;
extern struct ViewPort view_port1;

extern int dbuffer_offset_1;
extern int dbuffer_offset_2;

extern struct Image element_city;
extern struct BitMap *bitmap_element_city;
extern struct BitMap *bitmap_element_tree;
extern struct BitMap *bitmap_element_bridge;

extern struct Image trabant_facing_ground;
extern struct Image trabant_facing_car;
extern struct Image trabant_facing_car_l0;
extern struct Image trabant_facing_car_l1;

extern struct Image trabant_side_ground;
extern struct Image trabant_side_car;

extern struct Image mistral_title_0;
extern struct Image mistral_title_1;
extern struct Image mistral_title_2;
extern struct Image mistral_title_3;

extern struct Image title_place;
extern struct Image title_logo;

extern struct Image element_tree;
extern struct Image element_bridge;

extern struct BitMap *bitmap_facing_ground;
extern struct BitMap *bitmap_facing_car;
extern struct BitMap *bitmap_carlight_0;
extern struct BitMap *bitmap_carlight_1;

extern struct BitMap *bitmap_side_ground;
extern struct BitMap *bitmap_side_car;

extern UWORD element_bridgePaletteRGB4[8];

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

/*
    Various titles
*****************************/

void __inline loadAndDrawDemoTitle(struct BitMap *dest_bitmap)
{   
    struct BitMap *bitmap_title;

    bitmap_title = load_file_as_bitmap("assets/title_logo.dat", 14640, title_logo.Width, title_logo.Height, title_logo.Depth);
    WaitTOF();
    BLIT_BITMAP_S(bitmap_title, dest_bitmap, title_logo.Width, title_logo.Height, ((DISPL_WIDTH1 - title_logo.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - title_logo.Height) >> 1) - 32);

    WaitBlit();
    free_allocated_bitmap(bitmap_title);
}

void __inline loadAndDrawDemoPlace(struct BitMap *dest_bitmap)
{   
    struct BitMap *bitmap_title;

    bitmap_title = load_file_as_bitmap("assets/title_place.dat", 5440, title_place.Width, title_place.Height, title_place.Depth);
    WaitTOF();
    BLIT_BITMAP_S(bitmap_title, dest_bitmap, title_place.Width, title_place.Height, 320 + ((DISPL_WIDTH1 - title_place.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - title_place.Height) >> 1) + 40);

    WaitBlit();
    free_allocated_bitmap(bitmap_title);
}

void __inline loadAndDrawMistralTitle(struct BitMap *dest_bitmap, UBYTE title_number)
{   
    struct BitMap *bitmap_title;

    switch(title_number)
    {
        case 0:
            bitmap_title = load_file_as_bitmap("assets/mistral_title_0.dat", 1140, mistral_title_0.Width, mistral_title_0.Height, mistral_title_0.Depth);
            WaitTOF();
            BLIT_BITMAP_S(bitmap_title, dest_bitmap, mistral_title_0.Width, mistral_title_0.Height, ((DISPL_WIDTH1 - mistral_title_0.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - mistral_title_0.Height) >> 1));
            break;

        case 1:
            bitmap_title = load_file_as_bitmap("assets/mistral_title_1.dat", 4800, mistral_title_1.Width, mistral_title_1.Height, mistral_title_1.Depth);
            WaitTOF();
            BLIT_BITMAP_S(bitmap_title, dest_bitmap, mistral_title_1.Width, mistral_title_1.Height, ((DISPL_WIDTH1 - mistral_title_1.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - mistral_title_1.Height) >> 1));
            break;
        case 2:
            bitmap_title = load_file_as_bitmap("assets/mistral_title_2.dat", 4320, mistral_title_2.Width, mistral_title_2.Height, mistral_title_2.Depth);
            WaitTOF();
            BLIT_BITMAP_S(bitmap_title, dest_bitmap, mistral_title_2.Width, mistral_title_2.Height, ((DISPL_WIDTH1 - mistral_title_2.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - mistral_title_2.Height) >> 1));
            break; 
        case 3:
            bitmap_title = load_file_as_bitmap("assets/mistral_title_3.dat", 3552, mistral_title_3.Width, mistral_title_3.Height, mistral_title_3.Depth);
            WaitTOF();
            BLIT_BITMAP_S(bitmap_title, dest_bitmap, mistral_title_3.Width, mistral_title_3.Height, ((DISPL_WIDTH1 - mistral_title_3.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - mistral_title_3.Height) >> 1));
            break;                   
    }

    WaitBlit();
    free_allocated_bitmap(bitmap_title);
}

/*
    Facing car
*****************************/
void __inline loadTrabantFacingGround(void)
{   bitmap_facing_ground = load_zlib_file_as_bitmap("assets/trabant_facing_ground.dat", 5933, 12960, trabant_facing_ground.Width, trabant_facing_ground.Height, trabant_facing_ground.Depth);  }

void __inline drawTrabantFacingGround(struct BitMap *dest_bitmap)
{   BLIT_BITMAP_S(bitmap_facing_ground, dest_bitmap, trabant_facing_ground.Width, trabant_facing_ground.Height, ((DISPL_WIDTH1 - trabant_facing_ground.Width) >> 1) + dbuffer_offset_2, HEIGHT1 - trabant_facing_ground.Height);   }

void __inline freeTrabantFacingGround(void)
{   
    free_allocated_bitmap(bitmap_facing_ground);
    bitmap_facing_ground = NULL;
    }

void __inline loadTrabantFacingCar(void)
{   bitmap_facing_car = load_zlib_file_as_bitmap("assets/trabant_facing_car.dat", 2662, 6480, trabant_facing_car.Width, trabant_facing_car.Height, trabant_facing_car.Depth);  }

void __inline drawTrabantFacingCar(struct BitMap *dest_bitmap)
{   BLIT_BITMAP_S(bitmap_facing_car, dest_bitmap, trabant_facing_car.Width, trabant_facing_car.Height, ((DISPL_WIDTH1 - trabant_facing_car.Width) >> 1) + dbuffer_offset_2, HEIGHT1 - trabant_facing_car.Height - 32); }

void __inline freeTrabantFacingCar(void)
{   
    free_allocated_bitmap(bitmap_facing_car);
    bitmap_facing_car = NULL; 
}

void __inline loadTrabantLight(void)
{
    bitmap_carlight_0 = load_zlib_file_as_bitmap("assets/trabant_facing_car_l0.dat", 1812, 5568, trabant_facing_car_l0.Width, trabant_facing_car_l0.Height, trabant_facing_car_l0.Depth);
    bitmap_carlight_1 = load_zlib_file_as_bitmap("assets/trabant_facing_car_l1.dat", 2164, 5568, trabant_facing_car_l1.Width, trabant_facing_car_l1.Height, trabant_facing_car_l1.Depth);
}

#define FX_CLEAR_CARLIGHT SetAPen(rp, 0); RectFill(rp, dbuffer_offset_2, HEIGHT1 - trabant_facing_car.Height - 32 + 35, DISPL_WIDTH1 - 1 + dbuffer_offset_2, (HEIGHT1 - trabant_facing_car.Height - 32 + 35) + trabant_facing_car_l0.Height - 1);
void drawTrabantLight(struct BitMap *dest_bitmap, struct RastPort *rp, UBYTE light_level)
{
    switch(light_level)
    {
        case 0:
            FX_CLEAR_CARLIGHT;
            if (bitmap_facing_car)
                BLIT_BITMAP_S(bitmap_facing_car, dest_bitmap, trabant_facing_car.Width, trabant_facing_car.Height, ((DISPL_WIDTH1 - trabant_facing_car.Width) >> 1) + dbuffer_offset_2, HEIGHT1 - trabant_facing_car.Height - 32);
            break;
        case 1:
            FX_CLEAR_CARLIGHT;
            if (bitmap_carlight_0)
                BLIT_BITMAP_S(bitmap_carlight_0, dest_bitmap, trabant_facing_car_l0.Width, trabant_facing_car_l0.Height, ((DISPL_WIDTH1 - trabant_facing_car_l0.Width) >> 1) + 9 + dbuffer_offset_2, HEIGHT1 - trabant_facing_car.Height - 32 + 35);
            break;
        case 2:
            FX_CLEAR_CARLIGHT;
            if (bitmap_carlight_1)
                BLIT_BITMAP_S(bitmap_carlight_1, dest_bitmap, trabant_facing_car_l0.Width, trabant_facing_car_l0.Height, ((DISPL_WIDTH1 - trabant_facing_car_l1.Width) >> 1) + dbuffer_offset_2, HEIGHT1 - trabant_facing_car.Height - 32 + 35);
            break;
    }
}

void freeTrabantLight(void)
{
    free_allocated_bitmap(bitmap_carlight_0);
    free_allocated_bitmap(bitmap_carlight_1);
    bitmap_carlight_0 = NULL;
    bitmap_carlight_1 = NULL;
}

/*
    Side car
*****************************/
void __inline loadTrabantSideGround(void)
{   bitmap_side_ground = load_zlib_file_as_bitmap("assets/trabant_side_ground.dat", 776, 6480, trabant_side_ground.Width, trabant_side_ground.Height, trabant_side_ground.Depth);  }


void __inline drawTrabantSideGround(struct BitMap *dest_bitmap)
{   BLIT_BITMAP_S(bitmap_side_ground, dest_bitmap, trabant_side_ground.Width, trabant_side_ground.Height, ((DISPL_WIDTH1 - trabant_side_ground.Width) >> 1) + dbuffer_offset_2, HEIGHT1 - trabant_side_ground.Height - 8);   }

void __inline freeTrabantSideGround(void)
{   
    free_allocated_bitmap(bitmap_side_ground);
    bitmap_side_ground = NULL;
}

void __inline loadTrabantSideCar(void)
{   
   bitmap_side_car = load_zlib_file_as_bitmap("assets/trabant_side_car.dat", 6241, 16524, trabant_side_car.Width, trabant_side_car.Height, trabant_side_car.Depth);  
    // bitmap_side_car = load_file_as_bitmap("assets/trabant_side_car.bin", 16524, trabant_side_car.Width, trabant_side_car.Height, trabant_side_car.Depth);  
}

#define FX_SIDE_TRABANT_X ((DISPL_WIDTH1 - 246) >> 1) + dbuffer_offset_2 - 8
#define FX_SIDE_TRABANT_Y (HEIGHT1 - trabant_side_car.Height - 45)
void drawTrabantSideCar(struct BitMap *dest_bitmap, UBYTE door_step)
{   
    switch (door_step)
    {
        case 0:
            if (bitmap_side_car)
                BLIT_BITMAP_S(bitmap_side_car, dest_bitmap, 246, trabant_side_car.Height, FX_SIDE_TRABANT_X, FX_SIDE_TRABANT_Y);
            break;

        case 1:
            if (bitmap_side_car)
            {
                BLIT_BITMAP_S(bitmap_side_car, dest_bitmap, 246, trabant_side_car.Height, FX_SIDE_TRABANT_X, FX_SIDE_TRABANT_Y);
                BltBitMap(bitmap_side_car, 246, 0,
                            dest_bitmap, FX_SIDE_TRABANT_X + 95, FX_SIDE_TRABANT_Y,
                            88, trabant_side_car.Height,
                            0xC0, 0xFF, NULL);
            }
            break;

        case 2:
            if (bitmap_side_car)
            {
                BLIT_BITMAP_S(bitmap_side_car, dest_bitmap, 246, trabant_side_car.Height, FX_SIDE_TRABANT_X, FX_SIDE_TRABANT_Y);
                BltBitMap(bitmap_side_car, 246 + 90, 0,
                            dest_bitmap, FX_SIDE_TRABANT_X + 95, FX_SIDE_TRABANT_Y,
                            88, trabant_side_car.Height,
                            0xC0, 0xFF, NULL);
            }            
            break;         
    }
}

void __inline freeTrabantSideCar(void)
{   
    free_allocated_bitmap(bitmap_side_car);
    bitmap_side_car = NULL;
}

/*
    City scape
*****************************/
void loadElementCity(void)
{    bitmap_element_city = load_zlib_file_as_bitmap("assets/element_city.dat", 4671, 15048, element_city.Width, element_city.Height, element_city.Depth);  }

void drawElementCity(struct RastPort *rp, struct BitMap *dest_bitmap)
{
    // SetAPen(rp, 7);
    // RectFill(rp, 0, 0, (DEFAULT_WIDTH << 1) - 1, 70);

    BLIT_BITMAP_S(bitmap_element_city, dest_bitmap, element_city.Width, element_city.Height, 0, 71);
    BLIT_BITMAP_S(bitmap_element_city, dest_bitmap, element_city.Width, element_city.Height, DEFAULT_WIDTH, 71);
    WaitBlit();
}

BOOL __inline drawElementCityRefl(struct BitMap *dest_bitmap, unsigned int fx_clock)
{
    if (fx_clock < element_city.Height)
    {
        BltBitMap(bitmap_element_city, 0, element_city.Height - fx_clock - 1,
                    dest_bitmap, 0, 70 + element_city.Height + (fx_clock >> 1),
                    element_city.Width, 1,
                    0xC0, 0xFF, NULL);

        BltBitMap(bitmap_element_city, 0, element_city.Height - fx_clock - 1,
                    dest_bitmap, DEFAULT_WIDTH, 70 + element_city.Height + (fx_clock >> 1),
                    element_city.Width, 1,
                    0xC0, 0xFF, NULL);

        return TRUE;
    }
    else
        return FALSE;
}


void freeElementCity(void)
{
    free_allocated_bitmap(bitmap_element_city);
    bitmap_element_city = NULL;
}

void loadElementTree(void)
{    bitmap_element_tree = load_zlib_file_as_bitmap("assets/element_tree.dat", 799, 4096, element_tree.Width, element_tree.Height, element_tree.Depth);  }

void drawElementTree(struct BitMap *dest_bitmap)
{
    BLIT_BITMAP_S(bitmap_element_tree, dest_bitmap, element_tree.Width, element_tree.Height, 0, 0);
    WaitBlit();
}

void freeElementTree(void)
{
    free_allocated_bitmap(bitmap_element_tree);
    bitmap_element_tree = NULL;
}

void loadElementBridge(void)
{    bitmap_element_bridge = load_zlib_file_as_bitmap("assets/element_bridge.dat", 2739, 11280, element_bridge.Width, element_bridge.Height, element_bridge.Depth);  }

#define BRIDGE_OFFSET_Y 100
void drawElementBridge(struct BitMap *dest_bitmap)
{
    BLIT_BITMAP_S(bitmap_element_bridge, dest_bitmap, element_bridge.Width, element_bridge.Height, DISPL_WIDTH1, BRIDGE_OFFSET_Y);
    WaitBlit();
}

BOOL __inline drawElementBridgeRefl(struct BitMap *dest_bitmap, unsigned int fx_clock)
{
    if (fx_clock < element_bridge.Height)
    {
        BltBitMap(bitmap_element_bridge, 0, element_bridge.Height - fx_clock - 1,
                    dest_bitmap, DISPL_WIDTH1, BRIDGE_OFFSET_Y + element_bridge.Height + (fx_clock >> 1),
                    element_bridge.Width, 1,
                    0xC0, 0xFF, NULL);

        return TRUE;
    }
    else
        return FALSE;
}

void freeElementBridge(void)
{
    free_allocated_bitmap(bitmap_element_bridge);
    bitmap_element_bridge = NULL;
}

void setEmptyCopperList(struct ViewPort *vp)
{
    copper = (struct UCopList *)
    AllocMem( sizeof(struct UCopList), MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR );

    CINIT(copper, 16);
    CWAIT(copper, 0, 0);

    CMOVE(copper, *((UWORD *)SPR0PTH_ADDR), (LONG)&blank_pointer);
    CMOVE(copper, *((UWORD *)SPR0PTL_ADDR), (LONG)&blank_pointer);

    CEND(copper);

    vp->UCopIns = copper;    
}

#define CL_C0_START 102
#define CL_C1_START 30
void setLogoCopperList(struct ViewPort *vp)
{
    UWORD c_loop;

    copper = (struct UCopList *)
    AllocMem( sizeof(struct UCopList), MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR );

    CINIT(copper, 32);
    CWAIT(copper, 0, 0);

    CMOVE(copper, *((UWORD *)SPR0PTH_ADDR), (LONG)&blank_pointer);
    CMOVE(copper, *((UWORD *)SPR0PTL_ADDR), (LONG)&blank_pointer);

    for (c_loop = 0; c_loop < 255; c_loop++)
    {    
        if (c_loop > CL_C0_START && c_loop < 208)
        {
            CWAIT(copper, c_loop, 0);
            if (c_loop < CL_C0_START + CL_LOGO_LEN)
                CMOVE(copper, custom.color[2], cl_logo0[c_loop - CL_C0_START]);
            if (c_loop > CL_C0_START + CL_C1_START && c_loop < CL_C0_START + CL_C1_START + CL_LOGO_LEN)
                CMOVE(copper, custom.color[8 + 3], cl_logo1[c_loop - CL_C0_START - CL_C1_START]);

        }
        // CMOVE(copper, custom.color[8 + 3], 0x0F59);
    }

    CEND(copper);

    vp->UCopIns = copper;    
}

void setCityCopperList(struct ViewPort *vp)
{
	UWORD loop, zloop, c_loop, max_color, line_index;
    UWORD tmp_line;
    UBYTE pal_phase = 0;

    copper = (struct UCopList *)
    AllocMem( sizeof(struct UCopList), MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR );

    CINIT(copper, CL_CITY_LEN * 4);
    CWAIT(copper, 0, 0);

    CMOVE(copper, *((UWORD *)SPR0PTH_ADDR), (LONG)&blank_pointer);
    CMOVE(copper, *((UWORD *)SPR0PTL_ADDR), (LONG)&blank_pointer);

    loop = 0;
    printf("CL_CITY_LEN = %d\n", CL_CITY_LEN);

    while(loop < CL_CITY_LEN)
    {
        if (loop & 0x4 == 0x4)
            WaitTOF();

    	/*	Scanline position */
        tmp_line = loop;
        loop++;

    	CWAIT(copper, cl_city[tmp_line], 0);

    	/*	How many color registers ? */
        max_color = cl_city[loop++];
    	for (c_loop = 0; c_loop < max_color; c_loop++)
    		CMOVE(copper, custom.color[cl_city[loop++]], cl_city[loop++]);

        if (cl_city[tmp_line] > 0 && pal_phase == 0)
        {
            CMOVE(copper, custom.color[8], 0x000);
            CMOVE(copper, custom.color[9], element_bridgePaletteRGB4[1]);
            CMOVE(copper, custom.color[10], element_bridgePaletteRGB4[2]);
            CMOVE(copper, custom.color[11], element_bridgePaletteRGB4[3]);
            CMOVE(copper, custom.color[12], element_bridgePaletteRGB4[4]);
            CMOVE(copper, custom.color[13], element_bridgePaletteRGB4[5]);
            CMOVE(copper, custom.color[14], element_bridgePaletteRGB4[6]);
            CMOVE(copper, custom.color[15], element_bridgePaletteRGB4[7]);
            pal_phase++;
        }
        else
        if (cl_city[tmp_line] >= (BRIDGE_OFFSET_Y + element_bridge.Height) && pal_phase == 1)
        {
            CMOVE(copper, custom.color[9], element_bridgePaletteRGB4[1]);
            CMOVE(copper, custom.color[10], mixRGB4Colors(0x04A, element_bridgePaletteRGB4[2], 8));
            CMOVE(copper, custom.color[11], mixRGB4Colors(0x04A, element_bridgePaletteRGB4[3], 8));
            CMOVE(copper, custom.color[12], mixRGB4Colors(0x04A, element_bridgePaletteRGB4[4], 8));
            CMOVE(copper, custom.color[13], mixRGB4Colors(0x04A, element_bridgePaletteRGB4[5], 8));
            CMOVE(copper, custom.color[14], mixRGB4Colors(0x04A, element_bridgePaletteRGB4[6], 8));
            CMOVE(copper, custom.color[15], mixRGB4Colors(0x04A, element_bridgePaletteRGB4[7], 8));
            pal_phase++;
        }
    }

    CEND(copper);

    vp->UCopIns = copper;
}