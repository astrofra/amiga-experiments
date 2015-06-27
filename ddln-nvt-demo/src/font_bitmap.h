/*
	Data for "mario_font" Image
*/

#include <exec/types.h>
#include <intuition/intuition.h>


UWORD mario_fontPaletteRGB4[8] =
{
	0x0F0F,0x0000,0x0888,0x0BBB,0x0FFF,0x0000,0x0000,0x0000
};

struct Image mario_font =
{
	0, 0,		/* LeftEdge, TopEdge */
	512, 16, 3,	/* Width, Height, Depth */
	NULL,	/* ImageData */
	0x0007, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* NextImage */
};
