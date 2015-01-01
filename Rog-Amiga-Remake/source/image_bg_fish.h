/*
	Data for "image_bg_fish" Image
*/

#include <exec/types.h>
#include <intuition/intuition.h>


extern UWORD image_bg_fishPaletteRGB4[8];

extern UWORD chip image_bg_fishData[64];

struct Image image_bg_fish =
{
	0, 0,		/* LeftEdge, TopEdge */
	16, 16, 2,	/* Width, Height, Depth */
	image_bg_fishData,	/* ImageData */
	0x0002, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};
