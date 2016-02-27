#include <genesis.h>

typedef struct
{
    fix32 x;
    fix32 y;
    fix32 z;
} Vect3D_f32;

#define fix32InvCoef(K)  (fix32Sub(FIX32(1.0), K))
#define fix32DivFloats(A,B) fix32Div(FIX32(A), FIX32(B))

fix32 fix32Min(fix32 a, fix32 b);
fix32 fix32Max(fix32 a, fix32 b);
fix32 fix32RangeAdjust(fix32 val, fix32 in_lower, fix32 in_upper, fix32 out_lower, fix32 out_upper);
fix32 fix32Clamp(fix32 x, fix32 in_lower, fix32 in_upper);
fix32 fix32mapValueToArray(fix32 val, fix32 in_lower, fix32 in_upper, const fix32* mapping_array, const u16 mapping_array_len);

void utils_unit_tests(void);