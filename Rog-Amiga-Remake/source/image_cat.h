/*
	Data for "image_cat" Image
*/

#include <exec/types.h>
#include <intuition/intuition.h>


extern UWORD image_catPaletteRGB4[8];

extern UWORD chip image_catData[3864];

struct Image image_cat =
{
	0, 0,		/* LeftEdge, TopEdge */
	108, 184, 3,	/* Width, Height, Depth */
	image_catData,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};
