#include <genesis.h>

fix32 inline fix32Min(fix32 a, fix32 b){
	if (a < b)
		return a;
	else
		return b;
}

fix32 inline fix32Max(fix32 a, fix32 b){
	if (a > b)
		return a;
	else
		return b;
}

fix32 inline fix32RangeAdjust(fix32 val, fix32 in_lower, fix32 in_upper, fix32 out_lower, fix32 out_upper){
	/* return (val - in_lower) / (in_upper - in_lower) * (out_upper - out_lower) + out_lower */
    return fix32Add(fix32Div(fix32Sub(val, in_lower), fix32Mul(fix32Sub(in_upper, in_lower), fix32Sub(out_upper, out_lower))), out_lower);
}

fix32 inline fix32Clamp(fix32 x, fix32 in_lower, fix32 in_upper){
	return fix32Min(fix32Max(x, in_lower), in_upper);
}

int inline fix32CeilToInt(fix32 x)
{	return (fix32ToInt(x) + 1);	}

int inline fix32FloorToInt(fix32 x)
{	return fix32ToInt(x);	}

fix32 inline fix32InvCoef(fix32 x)
{	return fix32Sub(FIX32(1.0), x); }

fix32 fix32mapValueToArray(fix32 val, fix32 in_lower, fix32 in_upper, const fix32* mapping_array, const int mapping_array_len){
	val = fix32RangeAdjust(val, in_lower, in_upper, FIX32(0.0), FIX32(1.0));
	val = fix32Clamp(val, FIX32(0.0), FIX32(1.0));
	fix32 array_pos = fix32Mul(val, FIX32(mapping_array_len - 1));
	int ceil_pos = fix32CeilToInt(array_pos);
	int floor_pos = fix32FloorToInt(array_pos);
	fix32 k = fix32Sub(FIX32(ceil_pos), array_pos);
	return fix32Add(fix32Mul(mapping_array[floor_pos], k), fix32Mul(mapping_array[ceil_pos], fix32InvCoef(k)));
	// return mapping_array[floor_pos] * float(ceil_pos - array_pos) + mapping_array[ceil_pos] * (1.0 - (float(ceil_pos - array_pos)))
}

void utils_unit_tests(void)
{
	char str[32];

	BMP_drawText("fix32Min(-0.5,0.5)", 0, 0);
	fix32ToStr(fix32Min(FIX32(-0.5),FIX32(0.5)), str, 4);
	BMP_drawText(str, 32, 0);

	BMP_drawText("fix32Min(1.0,-0.5)", 0, 1);
	fix32ToStr(fix32Min(FIX32(-0.5),FIX32(0.5)), str, 4);
	BMP_drawText(str, 32, 1);	

	BMP_drawText("fix32Max(-0.5,0.5)", 0, 2);
	fix32ToStr(fix32Max(FIX32(-0.5),FIX32(0.5)), str, 4);
	BMP_drawText(str, 32, 2);

	BMP_drawText("fix32Max(0.5,-1.0)", 0, 3);
	fix32ToStr(fix32Max(FIX32(0.5),FIX32(-1.0)), str, 4);
	BMP_drawText(str, 32, 3);

	BMP_drawText("fix32InvCoef(0.25)", 0, 4);
	fix32ToStr(fix32InvCoef(FIX32(0.25)), str, 4);
	BMP_drawText(str, 32, 4);

	BMP_drawText("fix32Clamp(10, -1.0, 1.0)", 0, 5);
	fix32ToStr(fix32Clamp(FIX32(10.0), FIX32(-1.0), FIX32(1.0)), str, 4);
	BMP_drawText(str, 32, 5);

	BMP_drawText("fix32Clamp(-10, -1.0, 1.0)", 0, 5);
	fix32ToStr(fix32Clamp(FIX32(-10.0), FIX32(-1.0), FIX32(1.0)), str, 4);
	BMP_drawText(str, 32, 5);

	BMP_drawText("fix32RangeAdjust(3,0,10,0,1)", 0, 6);
	fix32ToStr(fix32RangeAdjust(FIX32(3.0), FIX32(0.0), FIX32(10.0), FIX32(0.0), FIX32(1.0)), str, 4);
	BMP_drawText(str, 32, 6);

	BMP_drawText("fix32CeilToInt(3.25)", 0, 7);
	intToStr(fix32CeilToInt(FIX32(3.25)), str, 0);
	BMP_drawText(str, 32, 7);

	BMP_drawText("fix32FloorToInt(3.25)", 0, 8);
	intToStr(fix32FloorToInt(FIX32(3.25)), str, 0);
	BMP_drawText(str, 32, 8);

	const fix32 test_coef[] = {FIX32(0.0), FIX32(0.25), FIX32(0.5), FIX32(0.75), FIX32(1.0)};
	BMP_drawText("fix32mapValueToArray(0.35)", 0, 9);
	fix32ToStr(fix32mapValueToArray(FIX32(0.35), FIX32(0.0), FIX32(1.0), test_coef, 5), str, 4);
	BMP_drawText(str, 32, 9);

	BMP_drawText("fix32mapValueToArray(5.0)", 0, 10);
	fix32ToStr(fix32mapValueToArray(FIX32(5.0), FIX32(0.0), FIX32(10.0), test_coef, 5), str, 4);
	BMP_drawText(str, 32, 10);		
}