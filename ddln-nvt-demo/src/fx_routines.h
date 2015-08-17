#ifndef FX_ROUTINES
#define FX_ROUTINES

#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

BOOL progressiveClearRaster(struct RastPort *rp, unsigned int fx_clock, const int max_width, const int max_height);

void setCityCopperList(struct ViewPort *vp);
void loadElementCity(void);
void drawElementCity(struct RastPort *rp, struct BitMap *dest_bitmap);
void freeElementCity(void);
void loadElementTree(void);
void drawElementTree(struct BitMap *dest_bitmap);
void freeElementTree(void);
BOOL drawElementCityRefl(struct BitMap *dest_bitmap, unsigned int fx_clock);
void loadElementBridge(void);
void drawElementBridge(struct BitMap *dest_bitmap);
void freeElementBridge(void);

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

void loadTrabantSideCar(void);
void drawTrabantSideCar(struct BitMap *dest_bitmap, UBYTE door_step);
void freeTrabantSideCar(void);

void loadAndDrawMistralTitle(struct BitMap *dest_bitmap, UBYTE title_number);

#endif // #ifndef FX_ROUTINES