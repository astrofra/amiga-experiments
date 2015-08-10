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

struct BitMap *bitmap_facing_ground = NULL;
struct BitMap *bitmap_facing_car = NULL;
struct BitMap *bitmap_carlight_0 = NULL;
struct BitMap *bitmap_carlight_1 = NULL;

struct BitMap *bitmap_side_ground = NULL;
struct BitMap *bitmap_side_car = NULL;

struct UCopList *copper;

UWORD chip blank_pointer[4]=
{
    0x0000, 0x0000,
    0x0000, 0x0000
};

/*
    Various titles
*****************************/

void __inline loadAndDrawMistralTitle(struct BitMap *dest_bitmap, UBYTE title_number)
{   
    struct BitMap *bitmap_title;

    switch(title_number)
    {
        case 0:
            bitmap_title = load_zlib_file_as_bitmap("assets/mistral_title_0.dat", 839, 1140, mistral_title_0.Width, mistral_title_0.Height, mistral_title_0.Depth);
            BLIT_BITMAP_S(bitmap_title, dest_bitmap, mistral_title_0.Width, mistral_title_0.Height, ((DISPL_WIDTH1 - mistral_title_0.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - mistral_title_0.Height) >> 1));
            break;

        case 1:
            bitmap_title = load_zlib_file_as_bitmap("assets/mistral_title_1.dat", 2508, 4800, mistral_title_1.Width, mistral_title_1.Height, mistral_title_1.Depth);
            BLIT_BITMAP_S(bitmap_title, dest_bitmap, mistral_title_1.Width, mistral_title_1.Height, ((DISPL_WIDTH1 - mistral_title_1.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - mistral_title_1.Height) >> 1));
            break;
        case 2:
            bitmap_title = load_zlib_file_as_bitmap("assets/mistral_title_2.dat", 2109, 4320, mistral_title_2.Width, mistral_title_2.Height, mistral_title_2.Depth);
            BLIT_BITMAP_S(bitmap_title, dest_bitmap, mistral_title_2.Width, mistral_title_2.Height, ((DISPL_WIDTH1 - mistral_title_2.Width) >> 1) + dbuffer_offset_2, ((DISPL_HEIGHT1 - mistral_title_2.Height) >> 1));
            break; 
        case 3:
            bitmap_title = load_zlib_file_as_bitmap("assets/mistral_title_3.dat", 1681, 3552, mistral_title_3.Width, mistral_title_3.Height, mistral_title_3.Depth);
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
{   free_allocated_bitmap(bitmap_facing_ground);   }

void __inline loadTrabantFacingCar(void)
{   bitmap_facing_car = load_zlib_file_as_bitmap("assets/trabant_facing_car.dat", 2662, 6480, trabant_facing_car.Width, trabant_facing_car.Height, trabant_facing_car.Depth);  }

void __inline drawTrabantFacingCar(struct BitMap *dest_bitmap)
{   BLIT_BITMAP_S(bitmap_facing_car, dest_bitmap, trabant_facing_car.Width, trabant_facing_car.Height, ((DISPL_WIDTH1 - trabant_facing_car.Width) >> 1) + dbuffer_offset_2, HEIGHT1 - trabant_facing_car.Height - 32); }

void __inline freeTrabantFacingCar(void)
{   free_allocated_bitmap(bitmap_facing_car);  }

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
}

/*
    Side car
*****************************/
void __inline loadTrabantSideGround(void)
{   bitmap_side_ground = load_zlib_file_as_bitmap("assets/trabant_side_ground.dat", 776, 6480, trabant_side_ground.Width, trabant_side_ground.Height, trabant_side_ground.Depth);  }


void __inline drawTrabantSideGround(struct BitMap *dest_bitmap)
{   BLIT_BITMAP_S(bitmap_side_ground, dest_bitmap, trabant_side_ground.Width, trabant_side_ground.Height, ((DISPL_WIDTH1 - trabant_side_ground.Width) >> 1) + dbuffer_offset_2, HEIGHT1 - trabant_side_ground.Height - 8);   }

void __inline freeTrabantSideGround(void)
{   free_allocated_bitmap(bitmap_side_ground);   }

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
{   free_allocated_bitmap(bitmap_side_car);  }

/*
    City scape
*****************************/
void loadElementCity(void)
{    bitmap_element_city = load_zlib_file_as_bitmap("assets/element_city.dat", 4697, 15048, element_city.Width, element_city.Height, element_city.Depth);  }

void drawElementCity(struct BitMap *dest_bitmap)
{
    BLIT_BITMAP_S(bitmap_element_city, dest_bitmap, element_city.Width, element_city.Height, 0, 71);
    BLIT_BITMAP_S(bitmap_element_city, dest_bitmap, element_city.Width, element_city.Height, 320, 71);
    WaitBlit();
}

void freeElementCity(void)
{
    free_allocated_bitmap(bitmap_element_city);
    bitmap_element_city = NULL;
}


void setCityCopperList(struct ViewPort *vp)
{
	UWORD loop, zloop, c_loop, max_color, line_index;

    copper = (struct UCopList *)
    AllocMem( sizeof(struct UCopList), MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR );

    CINIT(copper, CL_CITY_LEN);
    CWAIT(copper, 0, 0);

    CMOVE(copper, *((UWORD *)SPR0PTH_ADDR), (LONG)&blank_pointer);
    CMOVE(copper, *((UWORD *)SPR0PTL_ADDR), (LONG)&blank_pointer);

    loop = 0;
    printf("CL_CITY_LEN = %d\n", CL_CITY_LEN);

    while(loop < CL_CITY_LEN)
    {
    	/*	Scanline position */
    	CWAIT(copper, cl_city[loop++], 0);

    	/*	How many color registers ? */
        max_color = cl_city[loop++];
    	for (c_loop = 0; c_loop < max_color; c_loop++)
    		CMOVE(copper, custom.color[cl_city[loop++]], cl_city[loop++]);
    }
    // CWAIT(copper, 63, 0);
    // CMOVE(copper, custom.color[0], 0xF0F);

    // CWAIT(copper, 96, 0);
    // CMOVE(copper, custom.color[0], 0xFF0);

    CEND(copper);

    vp->UCopIns = copper;
}