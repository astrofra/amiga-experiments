#include <genesis.h>

fix16 fix16Min(fix16 a, fix16 b){
	if (a < b)
		return a;
	else
		return b;
}

fix16 fix16Max(fix16 a, fix16 b){
	if (a > b)
		return a;
	else
		return b;
}

fix16 fix16RangeAdjust(fix16 val, fix16 in_lower, fix16 in_upper, fix16 out_lower, fix16 out_upper){
	/* return (val - in_lower) / (in_upper - in_lower) * (out_upper - out_lower) + out_lower */
    return fix16Add(fix16Div(fix16Sub(val, in_lower), fix16Mul(fix16Sub(in_upper, in_lower), fix16Sub(out_upper, out_lower))), out_lower);
}

fix16 fix16Clamp(fix16 x, fix16 in_lower, fix16 in_upper){
	return fix16Min(fix16Max(x, in_lower), in_upper);
}

u16 fix16CeilToU16(fix16 x)
{	return (fix16ToInt(x) + 1);	}

u16 fix16FloorToU16(fix16 x)
{	return fix16ToInt(x);	}

fix16 fix16InvCoef(fix16 x)
{	return fix16Sub(FIX16(1.0), x); }

fix16 fix16mapValueToArray(fix16 val, fix16 in_lower, fix16 in_upper, const fix16* mapping_array, const u16 mapping_array_len){
	val = fix16RangeAdjust(val, in_lower, in_upper, FIX16(0.0), FIX16(1.0));
	val = fix16Clamp(val, FIX16(0.0), FIX16(1.0));
	fix16 array_pos = fix16Mul(val, FIX16(mapping_array_len - 1));
	u16 ceil_pos = fix16CeilToU16(array_pos);
	u16 floor_pos = fix16FloorToU16(array_pos);
	fix16 k = fix16Sub(FIX16(ceil_pos), array_pos);
	return fix16Add(fix16Mul(mapping_array[floor_pos], k), fix16Mul(mapping_array[ceil_pos], fix16InvCoef(k)));
	// return mapping_array[floor_pos] * float(ceil_pos - array_pos) + mapping_array[ceil_pos] * (1.0 - (float(ceil_pos - array_pos)))
}

// void array_mapper_unit_tests(void)
// {
// 	const fix16 persp_coef[] = [];
// 	fix16mapValueToArray()
// }

void utils_unit_tests(void)
{
	char str[32];

	BMP_drawText("fix16Min(-0.5,0.5)", 0, 0);
	fix16ToStr(fix16Min(FIX16(-0.5),FIX16(0.5)), str, 4);
	BMP_drawText(str, 24, 0);

	BMP_drawText("fix16Min(1.0,-0.5)", 0, 1);
	fix16ToStr(fix16Min(FIX16(-0.5),FIX16(0.5)), str, 4);
	BMP_drawText(str, 24, 1);	

	BMP_drawText("fix16Max(-0.5,0.5)", 0, 2);
	fix16ToStr(fix16Max(FIX16(-0.5),FIX16(0.5)), str, 4);
	BMP_drawText(str, 24, 2);

	BMP_drawText("fix16Max(0.5,-1.0)", 0, 3);
	fix16ToStr(fix16Max(FIX16(0.5),FIX16(-1.0)), str, 4);
	BMP_drawText(str, 24, 3);

	BMP_drawText("fix16InvCoef(0.25)", 0, 4);
	fix16ToStr(fix16InvCoef(FIX16(0.25)), str, 4);
	BMP_drawText(str, 24, 4);

	BMP_drawText("fix16Clamp(10, -1.0, 1.0)", 0, 5);
	fix16ToStr(fix16Clamp(FIX16(10.0), FIX16(-1.0), FIX16(1.0)), str, 4);
	BMP_drawText(str, 32, 5);

	BMP_drawText("fix16Clamp(-10, -1.0, 1.0)", 0, 5);
	fix16ToStr(fix16Clamp(FIX16(-10.0), FIX16(-1.0), FIX16(1.0)), str, 4);
	BMP_drawText(str, 32, 5);

	BMP_drawText("fix16RangeAdjust(3,0,10,0,1)", 0, 6);
	fix16ToStr(fix16RangeAdjust(FIX16(3.0), FIX16(0.0), FIX16(10.0), FIX16(0.0), FIX16(1.0)), str, 4);
	BMP_drawText(str, 32, 6);

	BMP_drawText("fix16CeilToU16(3.25)", 0, 7);
	intToStr(fix16CeilToU16(FIX16(3.25)), str, 4);
	BMP_drawText(str, 32, 7);

	BMP_drawText("fix16FloorToU16(3.25)", 0, 8);
	intToStr(fix16FloorToU16(FIX16(3.25)), str, 4);
	BMP_drawText(str, 32, 8);	
}