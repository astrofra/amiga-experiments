#include <intuition/intuition.h>

extern UWORD cat_copper_paldataRGB4[7];
extern UWORD cat_copper_imdata[3864];

struct Image cat_copper_image = {
	0, 0, 108, 184, 3, cat_copper_imdata,
	7, 0, NULL
};
