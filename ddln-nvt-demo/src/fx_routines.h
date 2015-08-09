#ifndef FX_ROUTINES
#define FX_ROUTINES

#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

void setCityCopperList(struct ViewPort *vp);
void loadElementCity(void);
void drawElementCity(struct BitMap *dest_bitmap);
void freeElementCity(void);

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