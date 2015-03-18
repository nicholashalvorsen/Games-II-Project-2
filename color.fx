//=============================================================================
// color.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms and colors geometry.
//=============================================================================

cbuffer cbPerObject
{
	float4x4 gWVP; 
	int mode;
	float diff;
};

void VS(float3 iPosL  : POSITION,
        float4 iColor : COLOR,
        out float4 oPosH  : SV_POSITION,
        out float4 oColor : COLOR)
{
	// Transform to homogeneous clip space.
	oPosH = mul(float4(iPosL, 1.0f), gWVP);
	
	switch(mode) {
	case 0:		// Player
		oColor = float4(0.0f, 0.0f, 1.0f, 1.0f);	// Blue
		break;
	/*case 1:		// Larger than me
		oColor = float4(1.0f, 0.0f, 0.0f, 1.0f);	// Red
		break;*/
	case 2:		// Smaller than me
		oColor = float4(0.0f, 1.0f, 0.0f, 1.0f);	// Green
		break;
	case 3:		// Experimental shading
		float red = lerp(0.0f, 1.0f, diff);
		float green = lerp(1.0f, 0.0f, diff);
		oColor = float4(red, green, 0.0f, 1.0f);
		break;
	case 4:
		oColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		break;
	default:
		// Just pass vertex color into the pixel shader.
		oColor = iColor;
		break;
	}
}

float4 PS(float4 posH  : SV_POSITION,
          float4 color : COLOR) : SV_Target
{
    return color;
}

technique10 ColorTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
