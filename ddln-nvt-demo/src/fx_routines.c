#include "includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>

#include "board.h"
#include "screen_size.h"
#include "color_routines.h"
#include "bitmap_routines.h"
#include "cosine_table.h"

extern struct GfxBase *GfxBase;
extern struct ViewPort view_port1;
extern struct ViewPort view_port2;
extern struct ViewPort view_port3;