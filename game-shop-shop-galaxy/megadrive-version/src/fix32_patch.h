#include <genesis.h>

#ifndef FIX32_PATCH
#define FIX32_PATCH

#define RSE_fix32Mul(val1, val2)        (((val1) * (val2)) >> FIX32_FRAC_BITS)
#define RSE_fix32Div(val1, val2)        (((val1) << FIX32_FRAC_BITS) / (val2))

#endif