#ifndef FX_ROUTINES
#define FX_ROUTINES

#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

void drawElementCity(struct BitMap *dest_bitmap);
void setCityCopperList(struct ViewPort *vp);
void drawTrabantFacingGround(struct BitMap *dest_bitmap);
void drawTrabantFacingCar(struct BitMap *dest_bitmap);
void freeTrabantFacingCar(void);
void loadTrabantLight(void);
void drawTrabantLight(struct BitMap *dest_bitmap, UBYTE light_level);
void freeTrabantLight(void);

#endif // #ifndef FX_ROUTINES