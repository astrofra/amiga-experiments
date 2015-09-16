/*
	Data for "element_city" Image
*/

#include <exec/types.h>
#include <intuition/intuition.h>


UWORD element_cityPaletteRGB4[8] =
{
	0x0F0F,0x0D1F,0x0959,0x0AAA,0x0888,0x0666,0x0555,0x0000
};

struct Image element_city =
{
	0, 0,		/* LeftEdge, TopEdge */
	348, 114, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

struct Image element_tree =
{
	0, 0,		/* LeftEdge, TopEdge */
	117, 256, 1,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0001, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

UWORD element_bridgePaletteRGB4[8] =
{
	0x0F0F,0x0000,0x0312,0x0523,0x0734,0x0835,0x0B47,0x0FC1
};

struct Image element_bridge =
{
	0, 0,		/* LeftEdge, TopEdge */
	320, 94, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

/* Trabant Screen #0 */

UWORD trabant_facing_groundPaletteRGB4[8] =
{
	0x0000,0x0312,0x0513,0x0824,0x0B36,0x0C47,0x0000,0x0000
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

struct Image trabant_side_ground =
{
	0, 0,		/* LeftEdge, TopEdge */
	280, 60, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

UWORD trabant_side_PaletteRGB4[8] =
{
	0x0000,0x0111,0x0333,0x0455,0x0378,0x01AC,0x0BBB,0x00EF
};

struct Image trabant_side_car =
{
	0, 0,		/* LeftEdge, TopEdge */
	432, 102, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

UWORD mistral_title_PaletteRGB4[8] =
{
	0x0000,0x0205,0x0409,0x0706,0x0905,0x0C08,0x0F0B,0x0000
};

struct Image mistral_title_0 =
{
	0, 0,		/* LeftEdge, TopEdge */
	65, 38, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

struct Image mistral_title_1 =
{
	0, 0,		/* LeftEdge, TopEdge */
	320, 40, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

struct Image mistral_title_2 =
{
	0, 0,		/* LeftEdge, TopEdge */
	320, 36, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

struct Image mistral_title_3 =
{
	0, 0,		/* LeftEdge, TopEdge */
	253, 37, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

UWORD title_logoPaletteRGB4[8] =
{
	0x0000,0x0526,0x005C,0x0748,0x0A6A,0x0D6B,0x0D9C,0x0FAE
};

struct Image title_logo =
{
	0, 0,		/* LeftEdge, TopEdge */
	320, 122, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

UWORD title_placePaletteRGB4[8] =
{
	0x0000,0x0525,0x0A47,0x0F59,0x0000,0x0000,0x0000,0x0000
};

struct Image title_place =
{
	0, 0,		/* LeftEdge, TopEdge */
	320, 68, 2,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0003, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

UWORD element_towerPaletteRGB4[8] =
{
	0x0000,0x0836,0x0947,0x0968,0x0889,0x08AB,0x08CC,0x0EEB
};

struct Image element_tower =
{
	0, 0,		/* LeftEdge, TopEdge */
	28, 256, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};

UWORD element_city_2bPaletteRGB4[8] =
{
	0x0000,0x0222,0x0333,0x0444,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

struct Image element_city_2b =
{
	0, 0,		/* LeftEdge, TopEdge */
	320, 116, 2,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0003, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};


