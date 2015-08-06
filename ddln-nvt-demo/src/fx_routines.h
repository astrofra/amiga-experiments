#ifndef FX_ROUTINES
#define FX_ROUTINES

#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

void drawElementCity(struct BitMap *dest_bitmap);
void setCityCopperList(struct ViewPort *vp);

void loadTrabantFacingGround(void);
void drawTrabantFacingGround(struct BitMap *dest_bitmap);
void freeTrabantFacingGround(void);

void loadTrabantFacingCar(void);
void drawTrabantFacingCar(struct BitMap *dest_bitmap);
void freeTrabantFacingCar(void);

void loadTrabantLight(void);
void drawTrabantLight(struct BitMap *dest_bitmap, struct RastPort *rp, UBYTE light_level);
void freeTrabantLight(void);

void loadTrabantSideGround(void);
void drawTrabantSideGround(struct BitMap *dest_bitmap);
void freeTrabantSideGround(void);

#endif // #ifndef FX_ROUTINES