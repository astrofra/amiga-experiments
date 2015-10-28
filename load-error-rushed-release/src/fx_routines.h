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

void loadAstronautSprite(void);
void drawAstronautSprite(struct BitMap *dest_bitmap);
void freeAstronautSprite(void);

void loadTitleSprite(void);
void drawTitleSprite(struct BitMap *dest_bitmap);
void freeTitleSprite(void);

void loadFaceSprite(void);
void drawFaceSprite(struct BitMap *dest_bitmap);
void freeFaceSprite(void);

void loadGuardSprite(void);
void drawGuardSprite(struct BitMap *dest_bitmap);
void freeGuardSprite(void);

void loadMountainSprite(void);
void drawMountainSprite(struct BitMap *dest_bitmap);
void freeMountainSprite(void);

void loadMummySprite(void);
void drawMummySprite(struct BitMap *dest_bitmap);
void freeMummySprite(void);

void loadUfoSprite(void);
void drawUfoSprite(struct BitMap *dest_bitmap);
void freeUfoSprite(void);

#endif // #ifndef FX_ROUTINES