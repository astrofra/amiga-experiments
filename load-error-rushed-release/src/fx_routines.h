#ifndef FX_ROUTINES
#define FX_ROUTINES

#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

BOOL progressiveClearRaster(struct RastPort *rp, unsigned int fx_clock, const int max_width, const int max_height,  UBYTE color_index);
void setEmptyCopperList(struct ViewPort *vp);

void loadRedbotSprite(void);
void drawRedbotSprite(struct BitMap *dest_bitmap);
void freeRedbotSprite(void);

#endif // #ifndef FX_ROUTINES