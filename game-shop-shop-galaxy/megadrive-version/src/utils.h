#include <genesis.h>

#define fix16InvCoef(K)  (fix16Sub(FIX16(1.0), K))
#define fix16DivFloats(A,B) fix16Div(FIX16(A), FIX16(B))

fix16 fix16Min(fix16 a, fix16 b);
fix16 fix16Max(fix16 a, fix16 b);
fix16 fix16RangeAdjust(fix16 val, fix16 in_lower, fix16 in_upper, fix16 out_lower, fix16 out_upper);
fix16 fix16Clamp(fix16 x, fix16 in_lower, fix16 in_upper);
fix16 fix16mapValueToArray(fix16 val, fix16 in_lower, fix16 in_upper, const fix16* mapping_array, const u16 mapping_array_len);
