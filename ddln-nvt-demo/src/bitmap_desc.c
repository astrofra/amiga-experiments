/*
	Data for "element_city" Image
*/

#include <exec/types.h>
#include <intuition/intuition.h>


UWORD element_cityPaletteRGB4[8] =
{
	0x0F0F,0x0AE8,0x07BC,0x077D,0x0947,0x043A,0x0626,0x0000
};

struct Image element_city =
{
	0, 0,		/* LeftEdge, TopEdge */
	780, 256, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};


/* Trabant Screen #0 */

UWORD trabant_facing_groundPaletteRGB4[8] =
{
	0x0101,0x0312,0x0513,0x0824,0x0B36,0x0C47,0x0000,0x0000
};

struct Image trabant_facing_ground =
{
	0, 0,		/* LeftEdge, TopEdge */
	320, 108, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};


UWORD trabant_facing_carPaletteRGB4[8] =
{
	0x0F0F,0x0000,0x0333,0x0557,0x0888,0x049B,0x01DF,0x0DDD
};

struct Image trabant_facing_car =
{
	0, 0,		/* LeftEdge, TopEdge */
	140, 120, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

struct Image trabant_facing_car_l1 =
{
	0, 0,		/* LeftEdge, TopEdge */
	250, 58, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

struct Image trabant_facing_car_l0 =
{
	0, 0,		/* LeftEdge, TopEdge */
	250, 58, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};
