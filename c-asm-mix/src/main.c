/*
	Secret (demo) Project
	by Mandarine
*/

#include "includes.prl"
#include "time.h"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>
#include <hardware/custom.h>
#include <graphics/gfxmacros.h>

extern int mul_by_ten(short input);

void main()
{
	short i;

	for(i = 0; i < 10; i++)
		printf("10 * %d = %d\n", i, mul_by_ten(i));
}
