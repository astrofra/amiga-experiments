#include <genesis.h>

fix16 inline fix16Min(fix16 a, fix16 b){
	if (a < b)
		return a;
	else
		return b;
}

fix16 inline fix16Max(fix16 a, fix16 b){
	if (a > b)
		return a;
	else
		return b;
}

fix16 inline fix16RangeAdjust(fix16 val, fix16 in_lower, fix16 in_upper, fix16 out_lower, fix16 out_upper){
	/* return (val - in_lower) / (in_upper - in_lower) * (out_upper - out_lower) + out_lower */
    return fix16Add(fix16Div(fix16Sub(val, in_lower), fix16Mul(fix16Sub(in_upper, in_lower), fix16Sub(out_upper, out_lower))), out_lower);
}

fix16 inline fix16Clamp(fix16 x, fix16 in_lower, fix16 in_upper){
	return fix16Min(fix16Max(x, in_lower), in_upper);
}

u16 inline fix16CeilToU16(fix16 x)
{	return (fix16Int(x) + 1);	}

u16 inline fix16FloorToU16(fix16 x)
{	return fix16Int(x);	}

fix16 inline fix16InvCoef(fix16 x)
{	return fix16Sub(FIX16(1.0), x); }

fix16 fix16mapValueToArray(fix16 val, fix16 in_lower, fix16 in_upper, const fix16* mapping_array, const u16 mapping_array_len){
	val = fix16RangeAdjust(val, in_lower, in_upper, FIX16(0.0), FIX16(1.0));
	val = fix16Clamp(val, FIX16(0.0), FIX16(1.0));
	fix16 array_pos = fix16Mul(val, FIX16(mapping_array_len - 1));
	u16 ceil_pos = fix16CeilToU16(array_pos);
	u16 floor_pos = fix16FloorToU16(array_pos);
	fix16 k = fix16Sub(ceil_pos, array_pos);
	return fix16Add(fix16Mul(mapping_array[floor_pos], k), fix16Mul(mapping_array[ceil_pos], fix16InvCoef(k)));
	// return mapping_array[floor_pos] * float(ceil_pos - array_pos) + mapping_array[ceil_pos] * (1.0 - (float(ceil_pos - array_pos)))
}