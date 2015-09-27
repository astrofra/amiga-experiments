// shadow mapping common support API

#ifndef SHADOW_COMMON_I
#define SHADOW_COMMON_I

float SampleShadowMap(texShadow shadow_map, vec3 p)
{
#if GLSL_API < 330
	return shadow2D(shadow_map, p).r;
#else
	return shadow2D(shadow_map, p);
#endif
}

float ComputeShadowPCF(vec3 pixel_view_pos, mat4 projection, texShadow shadow_map)
{
	vec4 pixel_light_pos = _mtx_mul(projection, vec4(pixel_view_pos - vec3(0.0, 0.0, vLightState.w), 1.0));
	vec3 pixel_shadow_uv = pixel_light_pos.xyz / pixel_light_pos.w;

	#define SampleShadowPCF(u, v)\
	{\
		vec3 offset = pixel_shadow_uv + vec3(u, v, 0.0) * vInverseShadowMapSize;\
		pcf_tap += SampleShadowMap(shadow_map, offset);\
	}

	float pcf_tap = 0.0;

	SampleShadowPCF(-1.0, -1.0);
	SampleShadowPCF( 0.0, -1.0);
	SampleShadowPCF( 1.0, -1.0);
	SampleShadowPCF(-1.0,  0.0);
	SampleShadowPCF( 0.0,  0.0);
	SampleShadowPCF( 1.0,  0.0);
	SampleShadowPCF(-1.0,  1.0);
	SampleShadowPCF( 0.0,  1.0);
	SampleShadowPCF( 1.0,  1.0);

	return pcf_tap * (1.0 / 9.0);
}

float ComputeLinearLightShadowPCF(vec3 pixel_view_pos)
{
	if (pixel_view_pos.z < vLightPSSMSliceDistance.x)
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[0], vLightShadowMap);
	else if (pixel_view_pos.z < vLightPSSMSliceDistance.y)
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[1], vLightShadowMap);
	else if (pixel_view_pos.z < vLightPSSMSliceDistance.z)
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[2], vLightShadowMap);
	else if (pixel_view_pos.z < vLightPSSMSliceDistance.w) {
		float pcf = ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[3], vLightShadowMap);
		float ramp_len = (vLightPSSMSliceDistance.w - vLightPSSMSliceDistance.z) * 0.25;
		float ramp_k = clamp((pixel_view_pos.z - (vLightPSSMSliceDistance.w - ramp_len)) / ramp_len, 0.0, 1.0);
		return pcf * (1.0 - ramp_k) + ramp_k;
	}
	return 1.0;
}

float ComputePointLightShadowPCF(vec3 pixel_view_pos)
{
	vec3 dir = normalize(_mtx_mul(vViewToLightMatrix, vec4(pixel_view_pos, 1.0)).xyz);

	if ((dir.z > 0.0) && (dir.x > -dir.z) && (dir.x < dir.z) && (dir.y > -dir.z) && (dir.y < dir.z))
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[0], vLightShadowMap);
	else if ((dir.z < 0.0) && (dir.x > dir.z) && (dir.x < -dir.z) && (dir.y > dir.z) && (dir.y < -dir.z))
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[2], vLightShadowMap);

	else if ((dir.x > 0.0) && (dir.y > -dir.x) && (dir.y < dir.x) && (dir.z > -dir.x) && (dir.z < dir.x))
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[1], vLightShadowMap);
	else if ((dir.x < 0.0) && (dir.y > dir.x) && (dir.y < -dir.x) && (dir.z > dir.x) && (dir.z < -dir.x))
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[3], vLightShadowMap);

	else if ((dir.y > 0.0) && (dir.x > -dir.y) && (dir.x < dir.y) && (dir.z > -dir.y) && (dir.z < dir.y))
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[5], vLightShadowMap);
	else if ((dir.y < 0.0) && (dir.x > dir.y) && (dir.x < -dir.y) && (dir.z > dir.y) && (dir.z < -dir.y))
		return ComputeShadowPCF(pixel_view_pos, vLightShadowMatrix[4], vLightShadowMap);

	return 1.0;
}

#endif // SHADOW_COMMON_I
