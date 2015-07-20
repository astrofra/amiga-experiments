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
