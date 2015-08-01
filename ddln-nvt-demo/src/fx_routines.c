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

extern struct Image element_city;
extern struct BitMap *bitmap_element_city;

extern struct Image trabant_facing_ground;
extern struct Image trabant_facing_car;

struct BitMap *bitmap_facing_car = NULL;
struct BitMap *bitmap_carlight_0 = NULL;
struct BitMap *bitmap_carlight_1 = NULL;

struct UCopList *copper;

UWORD chip blank_pointer[4]=
{
    0x0000, 0x0000,
    0x0000, 0x0000
};

void drawTrabantFacingGround(struct BitMap *dest_bitmap)
{
    struct BitMap *tmp_bitmap;
    tmp_bitmap = load_zlib_file_as_bitmap("assets/trabant_facing_ground.dat", 5933, 12960, trabant_facing_ground.Width, trabant_facing_ground.Height, trabant_facing_ground.Depth);
    BLIT_BITMAP_S(tmp_bitmap, dest_bitmap, trabant_facing_ground.Width, trabant_facing_ground.Height, 0, HEIGHT1 - trabant_facing_ground.Height);

    free_allocated_bitmap(tmp_bitmap);
}

void drawTrabantFacingCar(struct BitMap *dest_bitmap)
{
    bitmap_facing_car = load_zlib_file_as_bitmap("assets/trabant_facing_car.dat", 2662, 6480, trabant_facing_car.Width, trabant_facing_car.Height, trabant_facing_car.Depth);
    BLIT_BITMAP_S(bitmap_facing_car, dest_bitmap, trabant_facing_car.Width, trabant_facing_car.Height, 0, HEIGHT1 - trabant_facing_car.Height - 16);
}

void freeTrabantFacingCar(void)
{    free_allocated_bitmap(bitmap_facing_car);  }

void loadTrabantLight(void)
{
    bitmap_carlight_0 = load_zlib_file_as_bitmap("assets/trabant_facing_car_l0.dat", 1812, 5568, trabant_facing_car_l0.Width, trabant_facing_car_l0.Height, trabant_facing_car_l0.Depth);
    bitmap_carlight_1 = load_zlib_file_as_bitmap("assets/trabant_facing_car_l1.dat", 2164, 5568, trabant_facing_car_l1.Width, trabant_facing_car_l1.Height, trabant_facing_car_l1.Depth);
}

void drawTrabantLight(struct BitMap *dest_bitmap, UBYTE light_level)
{
    switch(light_level)
    {
        case 0:
            BLIT_BITMAP_S(bitmap_facing_car, dest_bitmap, trabant_facing_car.Width, trabant_facing_car.Height, 0, HEIGHT1 - trabant_facing_car.Height - 16);
            break;
        case 1:
            BLIT_BITMAP_S(bitmap_carlight_0, dest_bitmap, trabant_facing_car_l0.Width, trabant_facing_car_l0.Height, 0, HEIGHT1 - trabant_facing_car.Height - 16 + 35);
            break;
        case 2:
            BLIT_BITMAP_S(bitmap_carlight_1, dest_bitmap, trabant_facing_car_l0.Width, trabant_facing_car_l0.Height, 0, HEIGHT1 - trabant_facing_car.Height - 16 + 35);
            break;
    }
}

void freeTrabantLight(void)
{
    free_allocated_bitmap(bitmap_carlight_0);
    free_allocated_bitmap(bitmap_carlight_1);
}

void drawElementCity(struct BitMap *dest_bitmap)
{
    bitmap_element_city = load_zlib_file_as_bitmap("assets/element_city.dat", 12065, 75264, element_city.Width, element_city.Height, element_city.Depth);
    BLIT_BITMAP_S(bitmap_element_city, dest_bitmap, element_city.Width, element_city.Height, 0, 0);

    WaitBlit();

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