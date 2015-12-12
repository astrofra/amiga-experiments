#include <intuition/intuition.h>

extern const char ilkke_font_glyph_array[25];
extern const int ilkke_font_x_pos_array[375];

extern UWORD ilkke_font_paldataRGB4[3];
extern UWORD ilkke_font_imdata[800];

struct Image ilkke_font_image = {
	0, 0, 400, 16, 2, ilkke_font_imdata,
	3, 0, NULL
};
