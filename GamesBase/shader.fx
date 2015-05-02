#include "lighthelper.fx"

cbuffer cbPerFrame
{
	Light gLight;
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP;
	float4 color;
	float4 spec;
	float4x4 gTexMtx;
	int hasTexture;
};

Texture2D gDiffuseMap;//new
Texture2D gSpecMap; //new

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float2 texC    : TEXCOORD; //New
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
	float2 texC    : TEXCOORD; //New
};

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = MIRROR;
	AddressV = MIRROR;
};

VS_OUT VS(VS_IN vIn) {
	VS_OUT vOut;

	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);

	// Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	
	// Output vertex attributes for interpolation across triangle.
	vOut.diffuse = color;
	vOut.spec    = spec;
	if (hasTexture) vOut.texC  = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx);

	return vOut;
}

float4 PS(VS_OUT pIn) : SV_Target {

	if (hasTexture) {
		// Get materials from texture maps.
		pIn.diffuse = gDiffuseMap.Sample( gTriLinearSam, pIn.texC );
		pIn.spec    = gSpecMap.Sample( gTriLinearSam, pIn.texC );
	
		// Map [0,1] --> [0,256]
		pIn.spec.a *= 256.0f;
	}

	// Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);

	SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec};

	float3 litColor;
	litColor = ParallelLight(v, gLight, gEyePosW);

    return float4(litColor, pIn.diffuse.a);
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
