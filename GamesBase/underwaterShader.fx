#include "lighthelper.fx"

cbuffer cbPerFrame
{
	Light gLight;
	int gLightType; 
	float3 gEyePosW;
	float3 playerPos;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP;
	float4 colorOverride;
};

cbuffer cbFixed
{
	float  gFogStart = 40.0f;
	float  gFogRange = 40.0f;
	float3 gFogColor = { 0.8f, 0.8f, 0.8f };
	float3 gFogColor2 = { 0.15f, 0.15f, 0.15f };
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
	float  fogLerp : FOG;
};

VS_OUT VS(VS_IN vIn) {
	VS_OUT vOut;

	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);

	// Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	
	// Output vertex attributes for interpolation across triangle.
	vOut.diffuse = colorOverride;
	vOut.spec    = vIn.spec;

	float d = distance(vOut.posW, gEyePosW);
	vOut.fogLerp = saturate((d - gFogStart) / gFogRange);

	return vOut;
}

float4 PS(VS_OUT pIn) : SV_Target {

	// Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);

	if (pIn.posW.z < 0.25f && pIn.posW.z > -0.25 &&
		pIn.posW.x < playerPos.x + 0.25f && pIn.posW.x > playerPos.x - 0.25f &&
		pIn.posW.y < playerPos.y - 0.5f)
		pIn.diffuse *= float4(0.5f, 0.5f, 0.5f, 1);

	SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec};

	float3 litColor;
	if( gLightType == 0 ) // Parallel
		litColor = ParallelLight(v, gLight, gEyePosW);
    else if( gLightType == 1 ) // Point
		litColor = PointLight(v, gLight, gEyePosW);
	else if (gLightType == 2) //Spot
		litColor = Spotlight(v, gLight, gEyePosW);
	else 
		litColor = ParallelLight(v, gLight, gEyePosW);

	// Blend the fog color and the lit color.
	float3 foggedColor = lerp(litColor, gFogColor, pIn.fogLerp);
	float3 foggedColor2 = lerp(litColor, gFogColor2, pIn.fogLerp);

	const int layer2Height = 50;

	if (playerPos.y > layer2Height && pIn.posW.z < layer2Height + 10)
	{
		float dist = abs(layer2Height - 1 - playerPos.y) / 10;

		foggedColor[0] -= dist * (foggedColor[0] - foggedColor2[0]);
		foggedColor[1] -= dist * (foggedColor[1] - foggedColor2[1]);
		foggedColor[2] -= dist * (foggedColor[2] - foggedColor2[2]);

		if (foggedColor[0] < foggedColor2[0])
			foggedColor[0] = foggedColor2[0];

		if (foggedColor[1] < foggedColor2[1])
			foggedColor[1] = foggedColor2[1];

		if (foggedColor[2] < foggedColor2[2])
			foggedColor[2] = foggedColor2[2];

	}
	
	foggedColor[2] = 0.7;

    return float4(foggedColor, pIn.diffuse.a);
}

technique10 Tech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
